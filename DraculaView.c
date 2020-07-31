////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// DraculaView.c: the DraculaView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

// Written in the month of July 2020 for COMP2521 Assignment 2.

// This is the implementation file for DraculaView.h ADT. Implementation of the
// following code was completed by Gabriel Ting and Christian Ilagan.

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "DraculaView.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"

#define MAX_ARRAY_SIZE		1000
#define INVALID_LOC 		-1000

typedef struct playerInfo
{
	Player name;						// name of player
	PlaceId location;					// current location of player
	int health;							// current player health
} playerInfo;

struct draculaView {
	GameView gameState;					// Stores current game state

	char *playString; 					// Stores all past plays
	Message *messages;					// TODO: pointer to messages
	
	Round currRound; 					// current round of game
	playerInfo playerID[NUM_PLAYERS];	// array that contains each player info

};

// Prototypes for helper functions.
static void checkHideDoubleBack(
	PlaceId trail[], 
	int trailSize, 
	bool *hideExist, 
	bool *dbExist);
static void addHide(
	DraculaView dv,
	PlaceId validMoves[], 
	int *nValid, 
	PlaceId currLoc);
static void addDoubleBack(
	DraculaView dv, 
	PlaceId validMoves[], 
	int *nValid, 
	PlaceId currLoc);
static void addReachable(
	DraculaView dv, 
	PlaceId validMoves[], 
	int *nValid, 
	PlaceId curr);
static void findLocation(
	DraculaView dv, 
	int i, 
	PlaceId validMoves[]);
static void insertLocs(
	PlaceId loc, 
	PlaceId validLocs[], 
	int *nValidLocs);

//**************************************************************************//
//                		   Constructor/Deconstructor 		 	            //
//**************************************************************************//

DraculaView DvNew(char *pastPlays, Message messages[])
{
	DraculaView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate DraculaView\n");
		exit(EXIT_FAILURE);
	}

	new->gameState = GvNew(pastPlays, messages);
	new->playString = pastPlays;
	new->messages = messages;		// TODO: recheck if right

	// set up current state of game
	new->currRound = DvGetRound(new);

	// set up current information of the players
	new->playerID[0].name = PLAYER_LORD_GODALMING;
	new->playerID[1].name = PLAYER_DR_SEWARD;
	new->playerID[2].name = PLAYER_VAN_HELSING;
	new->playerID[3].name = PLAYER_MINA_HARKER;
	new->playerID[4].name = PLAYER_DRACULA;
	new->playerID[0].location = DvGetPlayerLocation(new, PLAYER_LORD_GODALMING);
	new->playerID[1].location = DvGetPlayerLocation(new, PLAYER_DR_SEWARD);
	new->playerID[2].location = DvGetPlayerLocation(new, PLAYER_VAN_HELSING);
	new->playerID[3].location = DvGetPlayerLocation(new, PLAYER_MINA_HARKER);
	new->playerID[4].location = DvGetPlayerLocation(new, PLAYER_DRACULA);
	new->playerID[0].health = DvGetHealth(new, PLAYER_LORD_GODALMING);
	new->playerID[1].health = DvGetHealth(new, PLAYER_DR_SEWARD);
	new->playerID[2].health = DvGetHealth(new, PLAYER_VAN_HELSING);
	new->playerID[3].health = DvGetHealth(new, PLAYER_MINA_HARKER);
	new->playerID[4].health = DvGetHealth(new, PLAYER_DRACULA);

	return new;
}

void DvFree(DraculaView dv)
{
	free(dv->gameState);
	free(dv);
}

//**************************************************************************//
//                		      Game State Information 		 	            //
//**************************************************************************//

Round DvGetRound(DraculaView dv)
{
	return GvGetRound(dv->gameState);
}

int DvGetScore(DraculaView dv)
{
	return GvGetScore(dv->gameState);
}

int DvGetHealth(DraculaView dv, Player player)
{
	return GvGetHealth(dv->gameState, player);
}

PlaceId DvGetPlayerLocation(DraculaView dv, Player player)
{
	return GvGetPlayerLocation(dv->gameState, player);
}

PlaceId DvGetVampireLocation(DraculaView dv)
{
	return GvGetVampireLocation(dv->gameState);
}

PlaceId *DvGetTrapLocations(DraculaView dv, int *numTraps)
{
	return GvGetTrapLocations(dv->gameState, &*numTraps);
}

//**************************************************************************//
//                		    	 Making a Move  		 	                //
//**************************************************************************//

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
	*numReturnedMoves = 0;
	// getting draculas current "trail"
	bool canFree = true;
	int trailSize = -1;
	PlaceId *trail = GvGetLastMoves(dv->gameState, PLAYER_DRACULA, TRAIL_SIZE, 
									&trailSize, &canFree);
	if (trailSize == 0) return NULL;

	PlaceId currLoc = GvGetPlayerLocation(dv->gameState, PLAYER_DRACULA);
	
	PlaceId validMoves[MAX_ARRAY_SIZE];
	int nValid = 0;

	// Checking if HIDE or DOUBLE_BACK have been used previously in trail
	bool hideExist = false;
	bool dbExist = false;
	checkHideDoubleBack(trail, trailSize, &hideExist, &dbExist);
	if (canFree) free(trail);

	// Check if HIDE is available to use
	if (!hideExist && placeIsLand(currLoc)) {
		addHide(dv, validMoves, &nValid, currLoc);
	}

	// Check if DOUBLE_BACK is available to use
	if (!dbExist) {
		addDoubleBack(dv, validMoves, &nValid, currLoc);
	}

	
	addReachable(dv, validMoves, &nValid, currLoc);

	*numReturnedMoves = nValid;
	if (nValid == 0) return NULL;

	PlaceId *moves = malloc(sizeof(PlaceId) * nValid);
	for (int i = 0; i < nValid; i++) {
		moves[i] = validMoves[i];
	}

	return moves;
}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
	*numReturnedLocs = 0;

	// getting Draculas valid moves for this round
	int nMoves = -1;
	PlaceId *validMoves = DvGetValidMoves(dv, &nMoves);
	if (nMoves == 0) return NULL;

	int nValidLocs = 0;
	PlaceId validLocs[MAX_ARRAY_SIZE];

	bool canFree = true;
	int trailSize = -1;
	PlaceId *trailLocs = GvGetLastLocations(dv->gameState, PLAYER_DRACULA, 
											TRAIL_SIZE, &trailSize, &canFree);
	
	for (int i = 0; i < nMoves; i++) {
		if (!placeIsReal(validMoves[i])) {
			if (validMoves[i] == HIDE || validMoves[i] == DOUBLE_BACK_1) {
				validMoves[i] = trailLocs[trailSize - 1];
			} else if (validMoves[i] == DOUBLE_BACK_2) {
				validMoves[i] = trailLocs[trailSize - 2];
			} else if (validMoves[i] == DOUBLE_BACK_3) {
				validMoves[i] = trailLocs[trailSize - 3];
			} else if (validMoves[i] == DOUBLE_BACK_4) {
				validMoves[i] = trailLocs[trailSize - 4];
			} else if (validMoves[i] == DOUBLE_BACK_5) {
				validMoves[i] = trailLocs[trailSize - 5];
			}
		}
		insertLocs(validMoves[i], validLocs, &nValidLocs);
	}
	*numReturnedLocs = nValidLocs;

	PlaceId *locs = malloc(sizeof(PlaceId) * nValidLocs);
	for (int i = 0; i < nValidLocs; i++) {
		locs[i] = validLocs[i];
	}

	free(validMoves);
	free(trailLocs);

	return locs;
}

PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
    *numReturnedLocs = 0;

    // Get Dracula's valid moves for this round
    int nMoves = -1;
    PlaceId *validMoves = DvGetValidMoves(dv, &nMoves);
    if (nMoves == 0) return NULL;

    PlaceId currLoc = GvGetPlayerLocation(dv->gameState, PLAYER_DRACULA);

    // Get all reachable places from current location
    int nReach = -1;
    PlaceId *reachLocs = GvGetReachableByType(dv->gameState, PLAYER_DRACULA, 
                            dv->currRound, currLoc, road, false, boat, &nReach);

    int nValidLocs = 0;
    PlaceId validLocs[MAX_ARRAY_SIZE];

    for (int i = 0; i < nMoves; i++) {
        findLocation(dv, i, validMoves);
        for (int j = 0; j < nReach; j++) {
            if (validMoves[i] == reachLocs[j]) {
                insertLocs(validMoves[i], validLocs, &nValidLocs);
                break;
            }
        }
    }

    if (nValidLocs == 0) {
        free(validMoves);
        return NULL;
    }
    PlaceId *locs = malloc(sizeof(PlaceId) * nValidLocs);
    for (int i = 0; i < nValidLocs; i++) {
        locs[i] = validLocs[i];
    }

    free(validMoves);

    *numReturnedLocs = nValidLocs;
    return locs;
}

PlaceId *DvWhereCanTheyGo(DraculaView dv, Player player,
                          int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	if (player == PLAYER_DRACULA) {
		return DvWhereCanIGo(dv, &*numReturnedLocs);
	} else {
		return GvGetReachable(dv->gameState, player, dv->currRound, 
							  dv->playerID[player].location, &*numReturnedLocs);	
	}
}

PlaceId *DvWhereCanTheyGoByType(DraculaView dv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	if (player == PLAYER_DRACULA) {
		return DvWhereCanIGoByType(dv, road, boat, &*numReturnedLocs);
	} else {
		return GvGetReachableByType(dv->gameState, player, dv->currRound, 
									dv->playerID[player].location, 
									road, rail, boat, &*numReturnedLocs);
	}
}

//**************************************************************************//
//                		    	Helper Functions 		 	                //
//**************************************************************************//

// Check if HIDE or DOUBLE_BACK have been used previously in trail
static void checkHideDoubleBack(PlaceId trail[], int trailSize, bool *hideExist, 
							bool *dbExist)
{
	for (int i = 1; i < trailSize; i++) {
		// Checking if the move has been called within the last 5 rounds
		if (trail[i] == HIDE) {
			*hideExist = true;
		} else if (trail[i] == DOUBLE_BACK_1 || trail[i] == DOUBLE_BACK_2 ||
				   trail[i] == DOUBLE_BACK_3 || trail[i] == DOUBLE_BACK_4 ||
				   trail[i] == DOUBLE_BACK_5) {

			*dbExist = true;
		}
	}
}

// Add HIDE move to the validMoves array
static void addHide(DraculaView dv, PlaceId validMoves[], int *nValid, PlaceId currLoc)
{
	int validIndex = *nValid;
	// getting all rechable locations from currLoc
	int nReach = 0;
	PlaceId *reachLocs = GvGetReachable(dv->gameState, PLAYER_DRACULA, 
										dv->currRound, currLoc, &nReach);
	for (int i = 0; i < nReach; i++) {
		if (reachLocs[i] == currLoc) {
			validMoves[validIndex] = HIDE;
			validIndex++;
			break;
		}
	}
	*nValid = validIndex;
}

// Add DOUBLE_BACK moves to the validMoves array
static void addDoubleBack(DraculaView dv, PlaceId validMoves[], int *nValid, 
					PlaceId currLoc)
{
	int validIndex = *nValid;
	// getting draculas current "trail"
	bool canFree = true;
	int trailSize = -1;
	PlaceId *trailLocs = GvGetLastLocations(dv->gameState, PLAYER_DRACULA, 
											TRAIL_SIZE, &trailSize, &canFree);
	// getting all rechable locations from currLoc
	int nReach = 0;
	PlaceId *reachLocs = GvGetReachable(dv->gameState, PLAYER_DRACULA, 
										dv->currRound, currLoc, &nReach);

	for (int i = trailSize - 1; i >= 0; i--) {
		bool isAdjacent = false;
		for (int j = 0; j < nReach; j++) {
			// If a location in the trail exists in reachLocs, it means it
			// is adjacent or is the current location
			if (trailLocs[i] == reachLocs[j]) {
				isAdjacent = true;
				break;
			}
		}
		int last = trailSize - 1;
		if (isAdjacent && last - i < 5) {
			if (last - i == 0) {
				validMoves[validIndex] = DOUBLE_BACK_1;
			} else if (last - i == 1) {
				validMoves[validIndex] = DOUBLE_BACK_2;
			} else if (last - i == 2) {
				validMoves[validIndex] = DOUBLE_BACK_3;
			} else if (last - i == 3) {
				validMoves[validIndex] = DOUBLE_BACK_4;
			} else if (last - i == 4) {
				validMoves[validIndex] = DOUBLE_BACK_5;
			}
			validIndex++;
		}
	}

	if (canFree) {
		free(trailLocs);
		free(reachLocs);
	}

	*nValid = validIndex;
}

// Add all valid rechable locations to the validMoves array
static void addReachable(DraculaView dv, PlaceId validMoves[], int *nValid, 
					PlaceId currLoc)
{
	int validIndex = *nValid;
	// getting draculas current "trail"
	bool canFree = true;
	int trailSize = -1;
	PlaceId *trailMoves = GvGetLastMoves(dv->gameState, PLAYER_DRACULA, 
											TRAIL_SIZE, &trailSize, &canFree);
	PlaceId *trailLocs = GvGetLastLocations(dv->gameState, PLAYER_DRACULA, 
											TRAIL_SIZE, &trailSize, &canFree);
	
	// getting all rechable locations from currLoc
	int nReach = 0;
	PlaceId *reachLocs = GvGetReachable(dv->gameState, PLAYER_DRACULA, 
										dv->currRound, currLoc, &nReach);

	// Consider the last move in the trail as reachable
	if (trailSize == TRAIL_SIZE) {
		for (int i = 0; i < nReach; i++) {
			if (trailLocs[0] == reachLocs[i] && reachLocs[i] != currLoc) {
				validMoves[validIndex] = reachLocs[i];
				reachLocs[i] = INVALID_LOC;
				validIndex++;
			}
		}
	}

	// Remove invalid rechable locations in reachLocs
	for (int i = 0; i < trailSize; i++) {
		for (int j = 0; j < nReach; j++) {
			if (trailMoves[i] == reachLocs[j] || reachLocs[j] == currLoc) {
				reachLocs[j] = INVALID_LOC;
			}
		}
	}

	// Copy valid rechable locations in reachLocs to validMoves array
	for (int i = 0; i < nReach; i++) {
		if (reachLocs[i] != INVALID_LOC) {
			validMoves[validIndex] = reachLocs[i];
			validIndex++;
		}
	}

	if (canFree) {
		free(trailMoves);
		free(trailLocs);
		free(reachLocs);
	}

	*nValid = validIndex;
}

// Finds the corresponding location for Dracula's special moves (i.e. HIDE and 
// DOUBLE_BACK)
static void findLocation(DraculaView dv, int i, PlaceId validMoves[]) 
{
	bool canFree = true;
	int trailSize = -1;
	PlaceId *trailLocs = GvGetLastLocations(dv->gameState, PLAYER_DRACULA, 
											TRAIL_SIZE, &trailSize, &canFree);
	if (!placeIsReal(validMoves[i])) {
		if (validMoves[i] == HIDE || validMoves[i] == DOUBLE_BACK_1) {
			validMoves[i] = trailLocs[trailSize - 1];
		} else if (validMoves[i] == DOUBLE_BACK_2) {
			validMoves[i] = trailLocs[trailSize - 2];
		} else if (validMoves[i] == DOUBLE_BACK_3) {
			validMoves[i] = trailLocs[trailSize - 3];
		} else if (validMoves[i] == DOUBLE_BACK_4) {
			validMoves[i] = trailLocs[trailSize - 4];
		} else if (validMoves[i] == DOUBLE_BACK_5) {
			validMoves[i] = trailLocs[trailSize - 5];
		}
	}
	if (canFree) free(trailLocs);
}

static void insertLocs(PlaceId loc, PlaceId validLocs[], int *nValidLocs) {
    int nLocs = *nValidLocs;
    bool isUniqueLoc = true;
    if (nLocs == 0) {
        validLocs[nLocs] = loc;
        nLocs++;
    } else {
        for (int i = 0; i < nLocs; i++) {
            if (validLocs[i] == loc) {
                isUniqueLoc = false;
                break;
            }
        }
        if (isUniqueLoc) {
            validLocs[nLocs] = loc;
            nLocs++;
        }
    }

    *nValidLocs = nLocs;
}
