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
	Round currRound;
	int score;

	hunterInfo hunterID[4];
	PlaceId allHunterLocs[NUM_REAL_PLACES * 4];
	int totalHunterLocs;

	PlaceId dangerHunterLocs[NUM_REAL_PLACES * 4];

	PlaceId currDracLoc;

	PlaceId *DracShouldGoLocs;
	int nShouldLocs;

	PlaceId *dracLocs;
	int nLocs;

	PlaceId *dracMoves;
	int nMoves;

	PlaceId dracHealth;

	PlaceId *activeTrapLocs;
	int nTraps;

	DraculaView dv;

} GameState;

int maxScoreIndex(int scoreMove[], int nDracPlays);
int calScore(DraculaView dv, PlaceId move);
bool isDbHide(DraculaView dv, PlaceId loc);
bool castleArea(GameState gameInfo);
bool hunterNotMove(DraculaView dv, Player player);
bool doSouthLoop(GameState gameInfo);

void decideDraculaMove(DraculaView dv)
{
	registerBestPlay("BI", "Default");
	GameState gameInfo;

	time_t t;
	srand((unsigned) time(&t));

	gameInfo.dv = dv;

    if (DvGetRound(dv) == 0) {
        int allMovesScore[NUM_REAL_PLACES];
        for (int i = 0; i < NUM_REAL_PLACES; i++) {
            PlaceId move = i;
            allMovesScore[i] = calScore(dv, move);
            int score = allMovesScore[i];
            // Add additional scoring factors
            // 1) Consider if its on land
            // if (placeIsLand(move)) score += 75;
            // 2) Go to CASTLE_DRACULA when health is critical
            if (move == CASTLE_DRACULA || move == HOSPITAL_PLACE) score -= 1000;
			// Check how many hunters can go to that location
			for (int i = 0; i < 4; i++) {
				int nPlays = 0;
				PlaceId *hunterPlays = DvWhereCanTheyGoByType(dv, i, true, true, false, &nPlays);
				for (int j = 0; j < nPlays; j++) {
					if (move == hunterPlays[j]) {
						score -= 1000;
					}
				}
				if (move == DvGetPlayerLocation(dv, i)) score -= 1000;
				free(hunterPlays);
			}
			if (placeIsSea(move)) score += 1000;
            allMovesScore[i] = score;
        }
		bool goodMove = false;
		int move = maxScoreIndex(allMovesScore, NUM_REAL_PLACES);
		int repeat = 0;
		int i;
		while (!goodMove && repeat < 100) {
			i = rand() % NUM_REAL_PLACES;
			if (allMovesScore[move] == allMovesScore[i]) break;
			repeat++;
		}
		move = i;
        char *play = (char *) placeIdToAbbrev(move);
        registerBestPlay(play, "begin");
        return;
    }


    // Get all possible locations all hunters can go to
    // int nHunterPlays = 0;
    // PlaceId hunterPlays[NUM_REAL_PLACES * 4];
    // for (int player = PLAYER_LORD_GODALMING; player < PLAYER_DRACULA; player++) {
    //     int nPlayerLocs = 0;
    //     PlaceId *playerPlays = DvWhereCanTheyGoByType(dv, player, true, true, false, &nPlayerLocs);
    //     for (int i = 0; i < nPlayerLocs; i++) {
    //         hunterPlays[nHunterPlays] = playerPlays[i];
    //         nHunterPlays++;
    //     }
    //     free(playerPlays);
    // }

    // Get all rechable locations from dracula
    int nDracPlays = 0;
    PlaceId *dracPlays = DvWhereCanIGo(dv, &nDracPlays);

	if (nDracPlays == 0) {
	    registerBestPlay("TP", "Goodbye");
	}

	else {
		int scoreMove[nDracPlays];
		for (int i = 0; i < nDracPlays; i++) {
			// Check if hunter and dracula loc overlap
			PlaceId move = dracPlays[i];
			int score = START_SCORE;

			// Check how many hunters can go to that location
			int hunterInLoc = 0;
			for (int i = 0; i < 4; i++) {
				int nPlays = 0;
				PlaceId *hunterPlays = DvWhereCanTheyGoByType(dv, i, true, true, false, &nPlays);
				for (int j = 0; j < nPlays; j++) {
					if (move == hunterPlays[j]) {
						score -= 1000;
						hunterInLoc++;
					}
				}
				free(hunterPlays);
			}

			// Consider how many traps are in that location
			int nTraps = 0;
			PlaceId *traps = DvGetTrapLocations(dv, &nTraps);
			for (int i = 0; i < nTraps; i++) {
				if (move == traps[i] && hunterInLoc > 0) {
					score += 5;
				}
			}
			free(traps);

			// Consider if dracula can take out the hunter
			for (int i = 0; i < 4; i++) {
				int health = DvGetHealth(dv, i);
				if (health <= 4 && DvGetHealth(dv, PLAYER_DRACULA) > 30 && hunterInLoc > 0) score += 20;
			}

			// Consider if hunter has not moved and was disqualified
			for (int i = 0; i < 4; i++) {
				if (hunterNotMove(dv, i) && DvGetPlayerLocation(dv, i) != move) {
					score += 1000;
				}
			}

			// Check health emergency
			int pathLen = 0;
			int nDracMoves = 0;	bool canFree = true;
			PlaceId *dracMoves = DvGetLastMoves(dv, PLAYER_DRACULA, 1, &nDracMoves, &canFree);
            PlaceId *pathToCastle = DvGetShortestPathTo(dv, CASTLE_DRACULA, &pathLen);
			if (DvGetHealth(dv, PLAYER_DRACULA) <= 25 && move == pathToCastle[0]) score += 10000;
			else if (pathLen < 3 && move == pathToCastle[0] && hunterInLoc <= 1) score += 1000;             

            if (canFree) free(dracMoves);
			free(pathToCastle);
			
			// Add additional scoring factors
			if (placeIsLand(move) && DvGetRound(dv) % 13 == 0) score += 5;
			if (isDbHide(dv, move)) {
				score -= 20;
			}
            if (placeIsLand(move) && DvGetHealth(dv, PLAYER_DRACULA) <= 10) score += 100;
			if (placeIsSea(move) && DvGetHealth(dv, PLAYER_DRACULA) >= 40) score += 20;
			else if (placeIsSea(move) && DvGetHealth(dv, PLAYER_DRACULA) >= 35) score += 15;
			else if (placeIsSea(move) && DvGetHealth(dv, PLAYER_DRACULA) >= 30) score += 10;
			else if (placeIsSea(move) && DvGetHealth(dv, PLAYER_DRACULA) >= 25) score += 5;


            // 4) Go to the south loop
            PlaceId *pathToMadrid = DvGetShortestPathTo(dv, MADRID, &pathLen);
            if (DvGetHealth(dv, PLAYER_DRACULA) > 30 && doSouthLoop(gameInfo) && hunterInLoc == 0) {
                int nMove = 0;  bool canFree = true;
                PlaceId *lastMove = DvGetLastMoves(dv, PLAYER_DRACULA, 1, &nMove, &canFree);
                if (nMove == 1 && (lastMove[nMove - 1] == LISBON || lastMove[nMove - 1] == CADIZ || lastMove[nMove - 1] == GRANADA ||
                    lastMove[nMove - 1] == ALICANTE || lastMove[nMove - 1] == MADRID || lastMove[nMove - 1] == SANTANDER)) {
                        score += 10;
                    }
				if (move == LISBON || move == CADIZ || move == GRANADA || move == ALICANTE || move == MADRID || move == SANTANDER) {
						score += 1000;
				}
				score += 1000;
				free(lastMove);
            }
			int nMove = 0;
			PlaceId *lastMove = DvGetLastMoves(dv, PLAYER_DRACULA, 1, &nMove, &canFree);
            if (pathLen > 0) free(pathToMadrid);

			if (DvGetHealth(dv, PLAYER_DRACULA) > 40 && nMove == 1 && castleArea(gameInfo) && lastMove[0] == CASTLE_DRACULA && hunterInLoc > 0) score += 10000;

			scoreMove[i] = score;
		}
		int i = maxScoreIndex(scoreMove, nDracPlays);
        PlaceId move = DvConvertLocToMove(dv, dracPlays[i]);
        char *play = (char *) placeIdToAbbrev(move);
        registerBestPlay(play, "luck");
		return;
	}

    // // Find how many locs of dracula overlap with current hunter location
    // int badLoc = 0;
    // for (int i = 0; i < nHunterPlays; i++) {
    //     for (int j = 0; j < nDracPlays; j++) {
    //         if (dracPlays[j] == hunterPlays[i]) {
    //             dracPlays[j] = DONT;
    //             badLoc++;
    //         }
    //     }
    // }

    // // Case where there is no more valid moves to make
    // if (nDracPlays == 0) {
    //     registerBestPlay("TP", "Goodbye");
    // }



    // // Case where all valid moves may have a hunter encounter
    // else if (nDracPlays == badLoc) {
	// 	free(dracPlays);
	// 	dracPlays = DvWhereCanIGo(dv, &nDracPlays);
    //     // Calculate how well the next move will be
    //     int scoreMove[NUM_REAL_PLACES];
    //     for (int i = 0; i < nDracPlays; i++) {
    //         PlaceId move = dracPlays[i];

	// 		int score = START_SCORE;
	// 		if (isDbHide(dv, move)) score -= 10;
	// 		if (placeIsSea(move) && DvGetHealth(dv, PLAYER_DRACULA) <= 15) score -= 100;
	// 		else if (isDbHide(dv, move) && placeIsSea(move)) score += 25;
	// 		else if (placeIsSea(move)) score += 50;
	// 		if (move == CASTLE_DRACULA && DvGetHealth(dv, PLAYER_DRACULA) <= 20) score += 50;
	// 		else if (move == CASTLE_DRACULA) score += 15;

	// 		// Check how many hunters can go to that location
	// 		for (int i = 0; i < 4; i++) {
	// 			int nPlays = 0;
	// 			PlaceId *hunterPlays = DvWhereCanTheyGoByType(dv, i, true, true, false, &nPlays);
	// 			for (int j = 0; j < nPlays; j++) {
	// 				if (move == hunterPlays[j]) score -= 30;
	// 			}
	// 			free(hunterPlays);
	// 		}

	// 		// Consider how many traps are in that location
	// 		int nTraps = 0;
	// 		PlaceId *traps = DvGetTrapLocations(dv, &nTraps);
	// 		for (int i = 0; i < nTraps; i++) {
	// 			if (move == traps[i]) {
	// 				score += 5;
	// 			}
	// 		}
	// 		free(traps);

	// 		// Consider if dracula can take out the hunter
	// 		int dracHealth = DvGetHealth(dv, PLAYER_DRACULA);
	// 		// Check hunter health and go there
	// 		for (int i = 0; i < 4; i++) {
	// 			int health = DvGetHealth(dv, i);
	// 			if (health <= 4 && dracHealth > 30) score += 20;
	// 		}

	// 		// Consider if hunter has not moved and was disqualified
	// 		for (int i = 0; i < 4; i++) {
	// 			if (hunterNotMove(dv, i) && DvGetPlayerLocation(dv, i) != move) {
	// 				score += 100;
	// 			}
	// 		}

	// 		int pathLen = 0;
    //         PlaceId *pathToCastle = DvGetShortestPathTo(dv, CASTLE_DRACULA, &pathLen);
    //         if (DvGetHealth(dv, PLAYER_DRACULA) <= 15 && move == pathToCastle[0]) {
    //             score += 1000;
    //         }
    //         if (pathLen > 0) free(pathToCastle);
	// 		scoreMove[i] = score;
    //     }
    //     int i = maxScoreIndex(scoreMove, nDracPlays);
    //     PlaceId move = DvConvertLocToMove(dv, dracPlays[i]);
    //     char *play = (char *) placeIdToAbbrev(move);
    //     registerBestPlay(play, "luck");
	// 	return;
    // }
    // else {
    //     // Get all moves that cant be reach by hunters
    //     PlaceId shouldGo[nDracPlays];
    //     int nShouldGo = 0;
    //     for (int i = 0; i < nDracPlays; i++) {
    //         if (dracPlays[i] != DONT) {
    //             shouldGo[nShouldGo] = dracPlays[i];
    //             nShouldGo++;
    //         }
    //     }

    //     int scoreMove[nShouldGo];
    //     for (int i = 0; i < nShouldGo; i++) {
    //         PlaceId move = shouldGo[i];
    //         // scoreMove[i] = calScore(dv, move);
    //         int score = START_SCORE;

	// 		if (placeIsLand(move) && DvGetRound(dv) % 13 == 0) score += 10;
	// 		if (isDbHide(dv, move)) {
	// 			score -= 100;
	// 		}
    //         // Add additional scoring factors
    //         // 1) Consider if its on land
    //         if (placeIsLand(move) && DvGetHealth(dv, PLAYER_DRACULA) <= 10) score -= 1500;
	// 		if (placeIsSea(move) && DvGetHealth(dv, PLAYER_DRACULA) >= 40) score += 25;
	// 		else if (placeIsSea(move) && DvGetHealth(dv, PLAYER_DRACULA) >= 35) score += 20;
	// 		else if (placeIsSea(move) && DvGetHealth(dv, PLAYER_DRACULA) >= 30) score += 15;
	// 		else if (placeIsSea(move) && DvGetHealth(dv, PLAYER_DRACULA) >= 25) score += 10;
    //         // 2) Go to CASTLE_DRACULA when health is critical
    //         int pathLen = 0;
    //         PlaceId *pathToCastle = DvGetShortestPathTo(dv, CASTLE_DRACULA, &pathLen);
    //         if (!castleArea(gameInfo) && pathLen > 0) {
    //             if (move == CASTLE_DRACULA) score += 100;
    //             else if (move == pathToCastle[0]) score += 10;
    //         } else if (DvGetHealth(dv, PLAYER_DRACULA) <= 25 && move == pathToCastle[0]) score += 1000;
    //         if (pathLen > 0) free(pathToCastle);
    //         // 3) Check if hide or double back
    //         //if (isDbHide(dv, move)) score -= 100;
    //         // 4) Go to the south loop
    //         PlaceId *pathToMadrid = DvGetShortestPathTo(dv, MADRID, &pathLen);
    //         if (DvGetHealth(dv, PLAYER_DRACULA) > 30 && doSouthLoop(gameInfo) && pathLen == 1) {
    //             int nMove = 0;  bool canFree = true;
    //             PlaceId *lastMove = DvGetLastMoves(dv, PLAYER_DRACULA, 1, &nMove, &canFree);
    //             if (nMove == 1 && (lastMove[nMove - 1] == LISBON || lastMove[nMove - 1] == CADIZ || lastMove[nMove - 1] == GRANADA ||
    //                 lastMove[nMove - 1] == ALICANTE || lastMove[nMove - 1] == MADRID || lastMove[nMove - 1] == SANTANDER)) {
    //                     score += 100;
    //                 }
	// 			free(lastMove);
    //         }
    //         if (pathLen > 0) free(pathToMadrid);
    //         scoreMove[i] = score;
	// 		// printf("Move: %d, Score: %d\n", move, score);
    //     }
    //     int i = maxScoreIndex(scoreMove, nShouldGo);
    //     PlaceId move = DvConvertLocToMove(dv, shouldGo[i]);
    //     char *play = (char *) placeIdToAbbrev(move);
    //     registerBestPlay(play, "yolo v");
    // }
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
		for (int i = 0; i < gameInfo.totalHunterLocs; i++) {
			if (gameInfo.allHunterLocs[i] == CASTLE_DRACULA ||
				gameInfo.allHunterLocs[i] == KLAUSENBURG	||
				gameInfo.allHunterLocs[i] == GALATZ 		|| 
				gameInfo.allHunterLocs[i] == BUCHAREST 		||
				gameInfo.allHunterLocs[i] == SZEGED 		||
				gameInfo.allHunterLocs[i] == CONSTANTA) 
			{
				nearCastle = true;
				nPlayers++;
				break;
			}
		}
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

int calScore(DraculaView dv, PlaceId move) {
    int score = START_SCORE;

    if (isDbHide(dv, move)) score -= 10;
    if (isDbHide(dv, move) && placeIsSea(move)) score += 25;
    else if (placeIsSea(move)) score += 50;
    if (move == CASTLE_DRACULA && DvGetHealth(dv, PLAYER_DRACULA) < 20) score += 50;
    else if (move == CASTLE_DRACULA) score += 25;
    if (placeIsLand(move) && DvGetRound(dv) % 13 == 0) score += 5;
    if (move != DvGetVampireLocation(dv)) score += 10;

    // Check how many hunters can go to that location
    for (int i = 0; i < 4; i++) {
        int nPlays = 0;
        PlaceId *hunterPlays = DvWhereCanTheyGoByType(dv, i, true, true, false, &nPlays);
        for (int j = 0; j < nPlays; j++) {
            if (move == hunterPlays[j]) score -= 30;
        }
        free(hunterPlays);
    }

    // Consider how many traps are in that location
    int nTraps = 0;
    PlaceId *traps = DvGetTrapLocations(dv, &nTraps);
    for (int i = 0; i < nTraps; i++) {
        if (move == traps[i]) {
            score += 10;
        }
    }
    free(traps);

    // Consider if dracula can take out the hunter
    int dracHealth = DvGetHealth(dv, PLAYER_DRACULA);
    // Check hunter health and go there
    for (int i = 0; i < 4; i++) {
        int health = DvGetHealth(dv, i);
        if (health <= 4 && dracHealth > 30) score += 20;
    }

    // Consider if hunter has not moved and was disqualified
    for (int i = 0; i < 4; i++) {
        if (hunterNotMove(dv, i) && DvGetPlayerLocation(dv, i) != move) {
            score += 100;
        }
    }

    return score;
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