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
#include <math.h>

#define START_SCORE 1000

// Written in the month of August 2020 for COMP2521 T2 Assignment 2.

// This is the implementation file for the Dracula AI. This implementation aims 
// to find the safest move away from the hunters by considering the path 
// distance and its standard deviation.

typedef struct gameState 
{
	DraculaView dv;
    Map map;

} GameState;

typedef struct moveCalc 
{
	double mean;
    double deviation;
    PlaceId move;
} MoveCalc;


int maxScoreIndex(int scoreMove[], int nDracPlays);
bool isDbHide(DraculaView dv, PlaceId loc);
bool castleArea(GameState gameInfo);
bool hunterNotMove(DraculaView dv, Player player);

void decideDraculaMove(DraculaView dv)
{
	registerBestPlay("TS", "Default");
	// GameState gameInfo;
	// gameInfo.dv = dv;

    // gameInfo.map = MapNew();

    if (DvGetRound(dv) == 0) {
        registerBestPlay("TS", "I dare you to hunt me :D");
        return;
    }

    int nDracPlays = 0;
    PlaceId *dracPlays = DvWhereCanIGo(dv, &nDracPlays);

    // No more valid moves
    if (nDracPlays == 0) {
        registerBestPlay("TP", "Goodbye!");
    }
    else {
        // Check which moves are completely safe
        bool allGoodMoves = true;
        for (int i = 0; i < nDracPlays; i++) {
            PlaceId move = dracPlays[i];
            for (int player = 0; player < 4; player++) {
                int nPlays = 0;
                PlaceId *huntPlays = DvWhereCanTheyGo(dv, player, &nPlays);
                for (int j = 0; j < nPlays; j++) {
                    if (move == huntPlays[j] && placeIsLand(huntPlays[j])) {
                        allGoodMoves = false;
                        break;
                    }
                }
                free(huntPlays);
                if (!allGoodMoves) break;
            }
            if (isDbHide(dv, move)) allGoodMoves = false;
            if (!allGoodMoves) break;
        }

        // Case where all valid moves have no risks
        if (allGoodMoves) {
            MoveCalc moveData[nDracPlays];
            // The following method to calculate the best moves uses these 
            // techniques:
            // 1) For each move, find the path distance to all player's 
            // current location -> thus finding the mean of how many cities 
            // it would take to reach that move by other players
            // 2) Calculate the standard deviation to see whether selected 
            // move have players reletively the same distance to reach that 
            // move
            // 3) Higher mean means more distance. Lower deviation mean that
            // values are closer to the mean. Therefore, find the highest 
            // mean with the lowest standard deviation for best move.
            for (int i = 0; i < nDracPlays; i++) {
                PlaceId move = dracPlays[i];
                // printf("Move: %d\n", move);

                int distToPlayer[4];
                double distance = 0;
                for (int i = 0; i < 4; i++) {
                    int hunterPathLen = 0;

                    #if 1
                        DvGetShortestPathTo(dv, move, DvGetPlayerLocation(dv, i), 
                                            &hunterPathLen);
                    #else
                        HvGetShortestPathTo(dv, i, move, &hunterPathLen);
                    #endif

                    distToPlayer[i] = hunterPathLen;
                    distance += hunterPathLen;
                    
                    // for (int i = 0; i < hunterPathLen; i++) {
                    //     printf("%s ->", placeIdToAbbrev(locs[i]));
                    // }
                    // printf("\nDist for Player %d: %d\n\n", i, hunterPathLen);

                }
                moveData[i].mean = distance/4;

                double variance = 0;
                for(int i = 0; i < 4; i++) {
                    variance += (distToPlayer[i] - moveData[i].mean) * 
                                (distToPlayer[i] - moveData[i].mean);
                }

                moveData[i].deviation = sqrt(variance/4);
                moveData[i].move = move;
            }

            // Order the array with the highest mean at the beginning using
            // bubble sort
            int nSwaps = 0;
            for (int i = 0; i < nDracPlays; i++) {
                nSwaps = 0;
                for (int j = nDracPlays; j > i; j--) {
                    if (moveData[j].mean > moveData[j - 1].mean) {
                        MoveCalc tmp = moveData[j - 1];
                        moveData[j - 1] = moveData[j];
                        moveData[j] = tmp;
                        nSwaps++;
                    }
                }
                if (nSwaps == 0) break;
            }

            // for (int i = 0; i < nDracPlays; i++) 
            //     printf("Mean: %f\n", moveData[i].mean);

            int min = moveData[0].deviation;
            int minIndex = 0;
            int max = 0.5 * nDracPlays;
            for (int i = 0; i < max; i++) {
                if (min > moveData[i].deviation) {
                    min = moveData[i].deviation;
                    minIndex = i;
                }
            }

            PlaceId move = DvConvertLocToMove(dv, moveData[minIndex].move);
            char *play = (char *) placeIdToAbbrev(move);
            registerBestPlay(play, "Safe Keeping!");
        } else {
            #if 1
            MoveCalc moveData[nDracPlays];
            for (int i = 0; i < nDracPlays; i++) {
                PlaceId move = dracPlays[i];
                // printf("Move: %d\n", move);

                int distToPlayer[4];
                double distance = 0;
                for (int i = 0; i < 4; i++) {
                    int hunterPathLen = 0;

                    #if 1
                        DvGetShortestPathTo(dv, move, DvGetPlayerLocation(dv, i), 
                                            &hunterPathLen);
                    #else
                        HvGetShortestPathTo(dv, i, move, &hunterPathLen);
                    #endif

                    distToPlayer[i] = hunterPathLen;
                    distance += hunterPathLen;
                    
                    // for (int i = 0; i < hunterPathLen; i++) {
                    //     printf("%s ->", placeIdToAbbrev(locs[i]));
                    // }
                    // printf("\nDist for Player %d: %d\n\n", i, hunterPathLen);

                }
                moveData[i].mean = distance/4;

                double variance = 0;
                for(int i = 0; i < 4; i++) {
                    variance += (distToPlayer[i] - moveData[i].mean) * 
                                (distToPlayer[i] - moveData[i].mean);
                }

                moveData[i].deviation = sqrt(variance/4);
                moveData[i].move = move;

                // Find how many hunters can reach the given move.
                for (int player = 0; player < 4; player++) {
                    int nPlays = 0;
                    PlaceId *huntPlays = DvWhereCanTheyGo(dv, player, &nPlays);
                    for (int j = 0; j < nPlays; j++) {
                        if (move == huntPlays[j] && placeIsLand(move)) {
                            moveData[i].mean -= 10;
                        }
                    }
                    if (placeIsSea(moveData[i].move)) {
                        moveData[i].mean -= 5;
                    }
                    if (isDbHide(dv, moveData[i].move)) {
                        moveData[i].mean -= 3;
                    }
                }

            }

            // Order the array with the highest mean at the beginning using
            // bubble sort
            int nSwaps = 0;
            for (int i = 0; i < nDracPlays; i++) {
                nSwaps = 0;
                for (int j = nDracPlays; j > i; j--) {
                    if (moveData[j].mean > moveData[j - 1].mean) {
                        MoveCalc tmp = moveData[j - 1];
                        moveData[j - 1] = moveData[j];
                        moveData[j] = tmp;
                        nSwaps++;
                    }
                }
                if (nSwaps == 0) break;
            }

            for (int i = 0; i < nDracPlays; i++) 
                printf("Mean: %f\n", moveData[i].mean);

            int min = moveData[0].deviation;
            int minIndex = 0;
            int max = 0.30 * nDracPlays;
            for (int i = 0; i < max; i++) {
                if (min > moveData[i].deviation) {
                    min = moveData[i].deviation;
                    minIndex = i;
                }
            }

            PlaceId move = DvConvertLocToMove(dv, moveData[minIndex].move);
            char *play = (char *) placeIdToAbbrev(move);
            registerBestPlay(play, "Blasting Away!");

            #else
            // Calculate how each valid move does using a score system

            int scoreMove[nDracPlays];
            // MoveCalc moveData[nDracPlays];
            
            for (int i = 0; i < nDracPlays; i++) {
                PlaceId move = dracPlays[i];
                int score = START_SCORE;
                
                // Find how many hunters can reach the given move.
                int hunterInMove = 0;
                for (int player = 0; player < 4; player++) {
                    int nPlays = 0;
                    PlaceId *huntPlays = DvWhereCanTheyGo(dv, player, &nPlays);
                    for (int j = 0; j < nPlays; j++) {
                        if (move == huntPlays[j] && placeIsLand(move)) {
                            score -= 25000;
                            hunterInMove++;
                        }
                    }
                }

                // This means that this is a good move where hunter cant reach
                // drac.
                if (hunterInMove == 0) {
                    if (placeIsSea(move)) score -= 2000;
                    if (isDbHide(dv, move)) score -= 1000;  

                    if (placeIsLand(move) && DvGetRound(dv) % 13 == 0) 
                        score += 1000;  
                } else {
                    if (placeIsSea(move)) score -= 2000;
                    if (isDbHide(dv, move)) score -= 1000;  

                    // 1) Consider how many traps are in that location if there 
                    // are hunters in the location
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
                            score += 5000;
                        } else if ( hunterNotMove(dv, i) && 
                                    DvGetPlayerLocation(dv, i) != move && 
                                    hunterInMove == 0 && 
                                    placeIsSea(move)) {
                            score += 2500;
                        }
                    }
                }

                // Calculate best move based on standard deviation
                // Check distance to current locs of hunters
                int distToPlayer[4];
                double averageDist = 0;
                for (int i = 0; i < 4; i++) {
                    int hunterPathLen = 0;
                    #if 1
                        DvGetShortestPathTo(dv, move, DvGetPlayerLocation(dv, i), 
                                            &hunterPathLen);
                    #else
                        HvGetShortestPathTo(dv, i, move, &hunterPathLen);
                    #endif
                    distToPlayer[i] = hunterPathLen;
                    averageDist += hunterPathLen;
                }
                
                averageDist /= 4;
                double variance = 0;

                for(int i = 0; i < 4; i++) {
                    variance += (distToPlayer[i] - averageDist) *
                                (distToPlayer[i] - averageDist);
                }

                double deviation = sqrt(variance/4);
                // printf("%f", deviation);
                if (deviation < 0.1) deviation = 0.1;
                int deviationCal = 1000/deviation;
                // if (deviation < 0.1) deviationCal = 0;
                // else deviationCal = 100000/deviation;
                // Add score based on mean and deviation
                score += 8000 * averageDist + deviationCal;

                // TODO: Check for health emergency
                int pathLen = 0;
                PlaceId *pathToCastle = DvGetShortestPathTo(dv, 
                                        DvGetPlayerLocation(dv, PLAYER_DRACULA),
                                        CASTLE_DRACULA, &pathLen);
                if (pathLen > 0 && DvGetHealth(dv, PLAYER_DRACULA) < 10 && 
                    pathToCastle[0] == move) 
                    score += 20000;
                free(pathToCastle);

                if (DvGetHealth(dv, PLAYER_DRACULA) < 10 && 
                    move == CASTLE_DRACULA) 
                    score += 7000;

                scoreMove[i] = score;
            }

            // for (int i = 0; i < nDracPlays; i++) printf("%d - ", scoreMove[i]);
            int i = maxScoreIndex(scoreMove, nDracPlays);
            PlaceId move = DvConvertLocToMove(dv, dracPlays[i]);
            char *play = (char *) placeIdToAbbrev(move);
            registerBestPlay(play, "luck");
            #endif
        }
    }

    free(dracPlays);
    // MapFree(gameInfo.map);

	return;
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