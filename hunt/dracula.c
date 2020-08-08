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
				PlaceId *hunterPlays = DvWhereCanTheyGo(dv, i, &nPlays);
				for (int j = 0; j < nPlays; j++) {
					if (move == hunterPlays[j] && placeIsLand(move)) {
						score -= 20000;
						hunterInLoc++;
					}
				}
				free(hunterPlays);
			}

            if (placeIsSea(move)) score += 1000;
            if (move == ST_JOSEPH_AND_ST_MARY) score -= 100000;
            //if (move == CASTLE_DRACULA) score += 1000;

            // for (int i = 0; i < 4; i++) {
            //     int pathLen = 0;
            //     PlaceId *pathAway = DvGetShortestPathTo(dv, move, DvGetPlayerLocation(dv, i), &pathLen);
                
            //     // if (pathLen < 5) score += pathLen * 200;
            //     // else if (pathLen < 10) score += 1000;
            //     // else score += 1250;
            //     score += pathLen * 100;
            //     free(pathAway);
            // }

            if (move == CASTLE_DRACULA && castleArea(gameInfo)) {
                score -= 10000;
            }
            
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
			if (allMovesScore[scoreNum] == maxScore && scoreNum != ST_JOSEPH_AND_ST_MARY) break;
			repeat++;
		}
        if (repeat > 100) move = maxScoreIndex(allMovesScore, NUM_REAL_PLACES);
        else move = scoreNum;
        char *play = (char *) placeIdToAbbrev(move);
        registerBestPlay(play, "begin");
        // registerBestPlay("TS", "stan");
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
                
                // Find how many hunters can reach the given move.
                int hunterInMove = 0;
                for (int player = 0; player < 4; player++) {
                    int nPlays = 0;
                    PlaceId *huntPlays = DvWhereCanTheyGo(dv, player, &nPlays);
                    for (int j = 0; j < nPlays; j++) {
                        if (move == huntPlays[j]) {
                            score -= 10000;
                            hunterInMove++;
                        }
                    }
                }

                // This means that this is a good move where hunter cant reach
                // drac.
                if (hunterInMove == 0) {
                    score += 100000;

                    if (placeIsLand(move)) score += 1000;
                    if (isDbHide(dv, move)) score -= 3000;    
                    if (placeIsLand(move) && DvGetRound(dv) % 13 == 0) score += 1000;  
                    if (DvGetHealth(dv, PLAYER_DRACULA) > 30 && placeIsSea(move)) score += 2000;              

                } else {
                    // 1) Consider how many traps are in that location if there are 
                    // hunters in the location
                    int nTraps = 0;
                    PlaceId *traps = DvGetTrapLocations(dv, &nTraps);
                    for (int i = 0; i < nTraps; i++) {
                        if (move == traps[i]) {
                            score += 500;
                        }
                    }
                    free(traps);

                    // 2) Consider if dracula can take out the hunter
                    for (int i = 0; i < 4; i++) {
                        int hunterHealth = DvGetHealth(dv, i);
                        if (DvGetHealth(dv, PLAYER_DRACULA) > 30 &&
                            hunterHealth <= 4) score += 500;
                    }

                    // 3) Consider if hunter has not moved and was disqualified
                    for (int i = 0; i < 4; i++) {
                        if (hunterNotMove(dv, i) && DvGetPlayerLocation(dv, i) != move
                            && hunterInMove == 0 && placeIsLand(move)) {
                            score += 3000;
                        } else if (hunterNotMove(dv, i) && DvGetPlayerLocation(dv, i) != move
                            && hunterInMove == 0 && placeIsSea(move)) {
                            score += 1500;
                        }
                    }

                    // 4) Type of move scoring factors
                    if (placeIsLand(move) && DvGetRound(dv) % 13 == 0 && hunterInMove == 1) score += 1000;
                    if (isDbHide(dv, move)) score -= 2000;
                    if (placeIsLand(move) && DvGetHealth(dv, PLAYER_DRACULA) <= 30) score += 2000;
                    if (placeIsSea(move) && DvGetHealth(dv, PLAYER_DRACULA) > 30) score += 2000;
                }

                // Check for health emergency
                int pathLen = 0;
                PlaceId *pathToCastle = DvGetShortestPathTo(dv, DvGetPlayerLocation(dv, PLAYER_DRACULA),
                CASTLE_DRACULA, &pathLen);

                if (pathLen > 0 && DvGetHealth(dv, PLAYER_DRACULA) < 20 && pathToCastle[0] == move) score += 30000;
                free(pathToCastle);

                // Check distance to current locs of hunters
                for (int i = 0; i < 4; i++) {
                    PlaceId currLoc = DvGetPlayerLocation(dv, i);
                    int pathLen = 0;
                    DvGetShortestPathTo(dv, move, currLoc, &pathLen);
                    if (pathLen > 7) score += 3600;
                    else score += pathLen * 500;
                }
                if (DvGetHealth(dv, PLAYER_DRACULA) < 20 && move == CASTLE_DRACULA) score += 7000;

                scoreMove[i] = score;
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