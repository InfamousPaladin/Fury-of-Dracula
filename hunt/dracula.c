////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include "dracula.h"
#include "DraculaView.h"
#include "Game.h"
#include "Map.h"
#include <stdlib.h>
#include <time.h> 
#include <stdio.h>

#define DONT 	-1000
#define LIMIT	100
#define START_SCORE 1000

typedef struct hunter
{
	PlaceId currLoc;					// current location of player
	int health;							// current player health
	PlaceId *reachable;
	int nReach;
} hunterInfo;

typedef struct gameState 
{
	DraculaView dv;
    Map map;

} GameState;

int maxScoreIndex(int scoreMove[], int nDracPlays);
bool isDbHide(DraculaView dv, PlaceId loc);
bool castleArea(GameState gameInfo);
bool hunterNotMove(DraculaView dv, Player player);
bool doSouthLoop(GameState gameInfo);

void decideDraculaMove(DraculaView dv)
{
	registerBestPlay("TS", "Default");
	GameState gameInfo;
	time_t t;
	srand((unsigned) time(&t));

	gameInfo.dv = dv;
    gameInfo.map = MapNew();

    if (DvGetRound(dv) == 0) {
        int allMovesScore[NUM_REAL_PLACES];
        for (int i = 0; i < NUM_REAL_PLACES; i++) {
            // Scoring Factors

            // 1) Check if hunter and dracula loc overlap
			PlaceId move = i;
			int score = START_SCORE;
			// Check how many hunters can go to that location
			int hunterInLoc = 0;
			for (int i = 0; i < 4; i++) {
				int nPlays = 0;
				PlaceId *hunterPlays = DvWhereCanTheyGoByType(dv, i, true, true, 
                                                                false, &nPlays);
				for (int j = 0; j < nPlays; j++) {
					if (move == hunterPlays[j]) {
						score -= 5000;
						hunterInLoc++;
					}
				}
				free(hunterPlays);
			}

            // 2) Location Factor - Land, Sea, Hospital, Castle Dracula
            ConnList conn = MapGetConnections(gameInfo.map, move);
            ConnList curr = conn;
            while (curr != NULL) {
                // ConnList prev = curr;
                if (placeIdToType(curr->p) == SEA) score += 10;
                curr = curr->next;
                // free(prev);
            }
            if (placeIsSea(move)) score += 100;
            if (move == HOSPITAL_PLACE) score -= 10000;
            if (move == CASTLE_DRACULA) score += 50;
            
            allMovesScore[i] = score;
        }
        // Get a random location
		bool goodMove = false;
		int move = maxScoreIndex(allMovesScore, NUM_REAL_PLACES);
        int maxScore = allMovesScore[move];
		int repeat = 0;
        int scoreNum = 0;
		while (!goodMove && repeat < 100) {
			scoreNum = rand() % NUM_REAL_PLACES;
			if (allMovesScore[scoreNum] == maxScore) break;
			repeat++;
		}
        if (repeat > 100) move = maxScoreIndex(allMovesScore, NUM_REAL_PLACES);
        else move = scoreNum;
        char *play = (char *) placeIdToAbbrev(move);
        registerBestPlay(play, "begin");
    }
    else {
        int nDracPlays = 0;
        PlaceId *dracPlays = DvWhereCanIGo(dv, &nDracPlays);

        // No more valid moves
        if (nDracPlays == 0) {
            registerBestPlay("TP", "Goodbye");
        }
        else {
            // Calculate how each valid move does
            int scoreMove[nDracPlays];
            for (int i = 0; i < nDracPlays; i++) {
                PlaceId move = dracPlays[i];
                int score = START_SCORE;

                // 1) Check how many hunters can go to that location
                int hunterInLoc = 0;
                for (int i = 0; i < 4; i++) {
                    int nPlays = 0;
                    PlaceId *hunterPlays = DvWhereCanTheyGoByType(dv, i, true, 
                                                        true, false, &nPlays);
                    for (int j = 0; j < nPlays; j++) {
                        if (move == hunterPlays[j]) {
                            score -= 1000;
                            hunterInLoc++;
                        }
                    }
                    //free(hunterPlays);
                    // Check if move will go to player's current location
                    if (move == DvGetPlayerLocation(dv, i)) score -= 2000;
                }

                // 2) Consider how many traps are in that location if there are 
                // hunters in the location
                if (hunterInLoc == 0) score += 50;
                else {
                    int nTraps = 0;
                    PlaceId *traps = DvGetTrapLocations(dv, &nTraps);
                    for (int i = 0; i < nTraps; i++) {
                        if (move == traps[i]) {
                            score += 10;
                        }
                    }
                    //free(traps);
                }
                

                // 3) Consider if dracula can take out the hunter
                if (hunterInLoc == 0) score += 50;
                else if (hunterInLoc == 1) {
                    for (int i = 0; i < 4; i++) {
                        int hunterHealth = DvGetHealth(dv, i);
                        if (DvGetHealth(dv, PLAYER_DRACULA) > 30 &&
                            hunterHealth <= 4) score += 20;
                        // else {
                        //     score += 1000;
                        // }
                    }
                } 
                else if (hunterInLoc == 2) score -=10;
                else if (hunterInLoc == 3) score -=20;
                else score -=30;


                // 4) Consider if hunter has not moved and was disqualified
                for (int i = 0; i < 4; i++) {
                    if (hunterNotMove(dv, i) && DvGetPlayerLocation(dv, i) != move
                        && hunterInLoc == 0 && placeIsLand(move)) {
                        score += 100;
                    } else if (hunterNotMove(dv, i) && DvGetPlayerLocation(dv, i) != move
                        && hunterInLoc == 0 && placeIsSea(move)) {
                        score += 70;
                    }
                }

                // 5) Check health emergency
                int pathLen = 0;
                int nDracMoves = 0;	bool canFree = true;
                PlaceId *dracMoves = DvGetLastMoves(dv, PLAYER_DRACULA, TRAIL_SIZE, 
                                                    &nDracMoves, &canFree);
                PlaceId *pathToCastle = DvGetShortestPathTo(dv, CASTLE_DRACULA, &pathLen);
                if (DvGetHealth(dv, PLAYER_DRACULA) <= 25 && move == pathToCastle[0] 
                    && hunterInLoc == 0) 
                    score += 10000;
                else if (DvGetHealth(dv, PLAYER_DRACULA) <= 25 && move == pathToCastle[0] 
                    && hunterInLoc == 1) 
                    score += 5000;
                else if (DvGetHealth(dv, PLAYER_DRACULA) <= 25 && move == pathToCastle[0] 
                    && hunterInLoc == 2) 
                    score += 2500;
                else if (pathLen < 3 && move == pathToCastle[0] && hunterInLoc <= 1) 
                    score += 100000;           
                else score -= 10000;
                free(pathToCastle);
                
                // 4) Type of move scoring factors
                if (placeIsLand(move) && DvGetRound(dv) % 13 == 0) score += 5;
                if (isDbHide(dv, move)) score -= 20;
                if (placeIsLand(move)) score += 10;
                if (placeIsLand(move) && DvGetHealth(dv, PLAYER_DRACULA) <= 10) score += 20;
                if (placeIsSea(move) && DvGetHealth(dv, PLAYER_DRACULA) > 40) score += 20;
                else if (placeIsSea(move) && DvGetHealth(dv, PLAYER_DRACULA) > 35) score += 15;
                else if (placeIsSea(move) && DvGetHealth(dv, PLAYER_DRACULA) > 30) score += 10;
                else if (placeIsSea(move) && DvGetHealth(dv, PLAYER_DRACULA) > 25) score += 5;


                // 5) Go to the south loop
                PlaceId *pathToMadrid = DvGetShortestPathTo(dv, MADRID, &pathLen);
                if (!doSouthLoop(gameInfo) && (move == LISBON || move == CADIZ ||
                    move == GRANADA || move == ALICANTE || move == MADRID || move == SANTANDER)) {
                        score -= 1000;
                    }
                if (DvGetHealth(dv, PLAYER_DRACULA) > 30 && doSouthLoop(gameInfo) && 
                    hunterInLoc == 0 && move == pathToMadrid[0]) {
                    // if (dracMoves[nDracMoves - 1] == MADRID && move == SARAGOSSA)
                    //     score -= 1000;
                    // Signifies on the loop
                    if (dracMoves[nDracMoves - 1] == LISBON && move == CADIZ) {
                        score += 50;
                    } else if (dracMoves[nDracMoves - 1] == CADIZ && move == GRANADA) {
                        score += 50;
                    } else if (dracMoves[nDracMoves - 1] == GRANADA && move == ALICANTE) {
                        score += 50;
                    } else if (dracMoves[nDracMoves - 1] == ALICANTE && move == MADRID) {
                        score += 50;
                    } else if (dracMoves[nDracMoves - 1] == MADRID && move == SANTANDER) {
                        score += 50;
                    } else if (dracMoves[nDracMoves - 1] == SANTANDER && move == LISBON) {
                        score += 50;
                    } 
                    // Other way
                    else if (move == LISBON && dracMoves[nDracMoves - 1] == CADIZ) {
                        score += 50;
                    } else if (move == CADIZ && dracMoves[nDracMoves - 1] == GRANADA) {
                        score += 50;
                    } else if (move == GRANADA && dracMoves[nDracMoves - 1] == ALICANTE) {
                        score += 50;
                    } else if (move == ALICANTE && dracMoves[nDracMoves - 1] == MADRID) {
                        score += 100;
                    } else if (move == MADRID && dracMoves[nDracMoves - 1] == SANTANDER) {
                        score += 50;
                    } else if (move == SANTANDER && dracMoves[nDracMoves - 1] == LISBON) {
                        score += 50;
                    }

                    if (move == MADRID && (dracMoves[nDracMoves - 1] == LISBON ||
                        dracMoves[nDracMoves - 1] == CADIZ || 
                        dracMoves[nDracMoves - 1] == GRANADA)) score -= 200;
                    else if (dracMoves[nDracMoves - 1] == MADRID && (move == LISBON ||
                    move == CADIZ || move == GRANADA)) score -=200;
                    score += 100;
                }
                if (pathLen > 0) free(pathToMadrid);

                // Case where dracula's move was to go to CASTLE_DRACULA
                if (DvGetHealth(dv, PLAYER_DRACULA) > 40 && castleArea(gameInfo) && 
                    dracMoves[nDracMoves - 1] == CASTLE_DRACULA && move == CASTLE_DRACULA) 
                    score -= 10000;
                else if (DvGetHealth(dv, PLAYER_DRACULA) <= 40 && !castleArea(gameInfo) && 
                        move == CASTLE_DRACULA) 
                    score += 10000;

                if (dracMoves[nDracMoves - 1] == CASTLE_DRACULA && move == KLAUSENBURG &&
                    hunterInLoc >= 1) {
                        score -= 10000; 
                } else if (dracMoves[nDracMoves - 1] == CASTLE_DRACULA && move == GALATZ &&
                    hunterInLoc >= 1) {
                        score -= 10000; 
                }
                if (dracMoves[nDracMoves - 1] == CASTLE_DRACULA && move == GALATZ &&
                    hunterInLoc == 0) {
                        score += 10000; 
                } else if (dracMoves[nDracMoves - 1] == CASTLE_DRACULA && move == KLAUSENBURG &&
                    hunterInLoc == 0) {
                        score += 10000; 
                }

                    
                scoreMove[i] = score;
                free(dracMoves);
            }
            int i = maxScoreIndex(scoreMove, nDracPlays);
            PlaceId move = DvConvertLocToMove(dv, dracPlays[i]);
            char *play = (char *) placeIdToAbbrev(move);
            registerBestPlay(play, "luck");
        }
        free(dracPlays);
	}
    MapFree(gameInfo.map);
	return;
}
bool doSouthLoop(GameState gameInfo) {
    // Strategy One: Go to LS CA GR AL MA SN then repeat
    bool doSouthLoop = true;
    if (DvGetRound(gameInfo.dv) >= TRAIL_SIZE) {
        for (int i = PLAYER_LORD_GODALMING; i < PLAYER_DRACULA; i++) {
			int nPlays = 0;
			PlaceId *hunterPlays = DvWhereCanTheyGo(gameInfo.dv, i, &nPlays);
            for (int j = 0; j < nPlays; j++) {
                if (hunterPlays[j] == LISBON ||
                hunterPlays[j] == CADIZ ||
                hunterPlays[j] == GRANADA ||
                hunterPlays[j] == ALICANTE ||
                hunterPlays[j] == MADRID ||
                hunterPlays[j] == SANTANDER ||
                hunterPlays[j] == SARAGOSSA ||
                hunterPlays[j] == BARCELONA ||
                hunterPlays[j] == BORDEAUX ||
                hunterPlays[j] == TOULOUSE) {
                    doSouthLoop = false;
                    break;
                }
            }
        }
    }
    return doSouthLoop;
}


bool castleArea(GameState gameInfo)
{
	bool nearCastle = false;
	int nPlayers = 0;
	for (int i = 0; i < 4; i++) {
        int nPlays = 0;
        PlaceId *hunterPlays = DvWhereCanTheyGo(gameInfo.dv, i, &nPlays);
		for (int i = 0; i < nPlays; i++) {
			if (hunterPlays[i] == CASTLE_DRACULA ||
				hunterPlays[i] == KLAUSENBURG	||
				hunterPlays[i] == GALATZ 		|| 
				hunterPlays[i] == BUCHAREST 		||
				hunterPlays[i] == SZEGED 		||
				hunterPlays[i] == CONSTANTA) 
			{
				nearCastle = true;
				nPlayers++;
				break;
			}
		}
        free(hunterPlays);
	}
	if (nPlayers < 2) nearCastle = false;	
	return nearCastle;
}

int maxScoreIndex(int scoreMove[], int nDracPlays) {
    int min = 0;
    for (int i = 0; i < nDracPlays; i++) {
        if (scoreMove[i] > scoreMove[min]) {
            min = i;
        }
    }
    return min;
}

bool hunterNotMove(DraculaView dv, Player player) {
    if (DvGetRound(dv) >= TRAIL_SIZE) {
        bool notMove = true;
        int nMoves = 0; bool canFree = true;
        PlaceId *pastLocs = DvGetLastMoves(dv, player, TRAIL_SIZE, &nMoves, &canFree);
        for (int i = 0; i < nMoves - 1; i++) {
            if (pastLocs[i] != pastLocs[i + 1]) {
                notMove = false;
                break;
            }
        }
        if (canFree) free(pastLocs);
        return notMove;
    }
    return false;
}

bool isDbHide(DraculaView dv, PlaceId loc) {
	if (DvGetRound(dv) == 0) return false;
	PlaceId move = DvConvertLocToMove(dv, loc);
	// printf("%d, %d", loc, move);
    if (move == DOUBLE_BACK_1 || move == DOUBLE_BACK_2 ||
        move == DOUBLE_BACK_3 || move == DOUBLE_BACK_4 ||
        move == DOUBLE_BACK_5 || move == HIDE)
    {
        return true;
    }
    return false;
}