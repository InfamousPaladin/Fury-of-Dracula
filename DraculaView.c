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

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "DraculaView.h"
#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"
// add your own #includes here

#define MAX_ARRAY_SIZE		1000
#define INVALID_LOC 		-1000

// TODO: ADD YOUR OWN STRUCTS HERE
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
	Map map; 							// map of the board
	int nMapLocs; 						// number of locations on map
	
	Round currRound; 					// current round of game
	int score; 							// current score of the game

	playerInfo playerID[NUM_PLAYERS];	// array that contains each player info
	PlaceId imVampireLoc; 				// location of immature vampires

	PlaceId *activeTrapLocs;			// locations of all active traps
	int nTraps;							// number of active traps

};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

// TODO: Gabriel
DraculaView DvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	DraculaView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate DraculaView\n");
		exit(EXIT_FAILURE);
	}

	new->gameState = GvNew(pastPlays, messages);
	new->playString = pastPlays;
	new->messages = messages;		// TODO: recheck if right
	

	// initialising a new map and number of places on map
	new->map = MapNew();
	new->nMapLocs = MapNumPlaces(new->map);

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

	// get the current score of the game
	new->score = DvGetScore(new);

	// get traps on the map
	new->nTraps = 0;
	new->activeTrapLocs = DvGetTrapLocations(new, &new->nTraps);

	// getting vampire locations
	new->imVampireLoc = DvGetVampireLocation(new);

	return new;
}

// TODO: Gabriel
void DvFree(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	free(dv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

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

////////////////////////////////////////////////////////////////////////
// Making a Move
// TODO: Christian

// Add DOUBLE_BACK moves to the validMoves array
void addDoubleBack(PlaceId trail[], int trailSize, PlaceId reachLocs[], 
						int nReach, PlaceId validMoves[], int *nValid)
{
	int validIndex = *nValid;
	for (int i = trailSize - 1; i >= 0; i--) {
		bool isAdjacent = false;
		for (int j = 0; j < nReach; j++) {
			// If a location in the trail exists in reachLocs, it means it
			// is adjacent or is the current location
			if (trail[i] == reachLocs[j]) {
				isAdjacent = true;
				break;
			}
		}
		if (isAdjacent) {
			int last = trailSize - 1;
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
	*nValid = validIndex;
}

void addHide(PlaceId reachLocs[], int nReach, PlaceId validMoves[], 
				int *nValid, PlaceId curr)
{
	int validIndex = *nValid;
	for (int i = 0; i < nReach; i++) {
		if (reachLocs[i] == curr) {
			validMoves[validIndex] = HIDE;
			validIndex++;
			break;
		}
	}
	*nValid = validIndex;				
}

void addReachable(PlaceId trail[], int trailSize, PlaceId reachLocs[], 
					int nReach, PlaceId validMoves[], int *nValid, PlaceId curr)
{
	// Remove invalid rechable locations in reachLocs
	for (int i = 0; i < trailSize; i++) {
		for (int j = 0; j < nReach; j++) {
			if (trail[i] == reachLocs[j] || reachLocs[j] == curr) {
				reachLocs[j] = INVALID_LOC;
			}
		}
	}

	int validIndex = *nValid;
	// Copy valid rechable locations in reachLocs to validMoves array
	for (int i = 0; i < nReach; i++) {
		if (reachLocs[i] != INVALID_LOC) {
			validMoves[validIndex] = reachLocs[i];
			validIndex++;
		}
	}
	*nValid = validIndex;
}

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{

	// getting draculas current "trail"
	bool canFree = true;
	int trailSize = -1;
	PlaceId *trail = GvGetLastMoves(dv->gameState, PLAYER_DRACULA, TRAIL_SIZE, 
									&trailSize, &canFree);
	PlaceId currLoc = GvGetPlayerLocation(dv->gameState, PLAYER_DRACULA);
	// getting all rechable locations from currLoc
	int nReach = 0;
	PlaceId *reachLocs = GvGetReachable(dv->gameState, PLAYER_DRACULA, 
										dv->currRound, currLoc, &nReach);
	// counter to keep track of the number of valid moves
	PlaceId validMoves[MAX_ARRAY_SIZE];
	int nValid = 0;

	// Checking if HIDE or DOUBLE_BACK have been used previously in trail
	bool hideExist = false;
	bool dbExist = false;
	for (int i = 0; i < trailSize; i++) {
		// Checking if the move has been called within the last 5 rounds
		if (trail[i] == HIDE) {
			hideExist = true;
		} else if (trail[i] == DOUBLE_BACK_1 || trail[i] == DOUBLE_BACK_2 ||
				   trail[i] == DOUBLE_BACK_3 || trail[i] == DOUBLE_BACK_4 ||
				   trail[i] == DOUBLE_BACK_5) {
			
			dbExist = true;
		}
	}

	// Check if HIDE is available to use
	if (!hideExist && placeIsLand(currLoc)) {
		addHide(reachLocs, nReach, validMoves, &nValid, currLoc);
	}

	// Check if DOUBLE_BACK is available to use
	if (!dbExist) {
		addDoubleBack(trail, trailSize, reachLocs, 
						nReach, validMoves, &nValid);
	}
	addReachable(trail, trailSize, reachLocs, nReach, validMoves, 
					&nValid, currLoc);
	
	free(trail);
	free(reachLocs);
	*numReturnedMoves = nValid;

	if (nValid == 0) {
		return NULL;
	} else {
		PlaceId *moves = malloc(sizeof(PlaceId) * nValid);
		for (int i = 0; i < nValid; i++) {
			moves[i] = validMoves[i];
		}
		return moves;
	}
}

// TODO: Christian
PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	// Remember Dracula cannot go to the hospital, via rail, or to any city in 
	// his trail without the use of a DOUBLE_BACK or HIDE
	// Dracula cannot HIDE at sea

	// For the trail the function GvGetLastMoves would be usefull
	// Function GvGetLastLocations
	// Consider if HIDE & DOUBLE_BACK cases, if it is in the trail
	bool canFree = true;
	PlaceId *trail = GvGetLastMoves(dv->gameState, PLAYER_DRACULA, TRAIL_SIZE, numReturnedLocs, &canFree);
	int numLocs = 0;
	// getting current location
	PlaceId currLoc = GvGetPlayerLocation(dv->gameState, PLAYER_DRACULA);
	// getting all possible locations
	PlaceId *validLocs = GvGetReachable(dv->gameState, PLAYER_DRACULA, dv->currRound, currLoc, &numLocs);
	// counter to keep track of the number of valid moves
	int nValidLocs = 0;
	// getting locations that can be moved to
	for (int i = 0; i < numLocs; i++) {
		int notInTrail = 0;
		for (int j = 1; j < TRAIL_SIZE; j++) {
			// comparing from the second element in the trail
			if (validLocs[i] != trail[j]) {
				notInTrail++;
			}
			if (notInTrail == 5) {
				nValidLocs++;
				validLocs[i] = validLocs[i];
			}
		} 
	}
	// there are no locations to be found
	if (nValidLocs == 0) {
		*numReturnedLocs = 0;
		return NULL;
	}
	PlaceId *moves = malloc(sizeof(PlaceId) * nValidLocs);
	*numReturnedLocs = nValidLocs;
	for (int i = 0; i < nValidLocs; i++) {
		moves[i] = validLocs[i];
	}
	free(validLocs);
	free(trail);

	return moves;
}

// TODO: Christian
PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	bool canFree = true;
	PlaceId *trail = GvGetLastMoves(dv->gameState, PLAYER_DRACULA, TRAIL_SIZE, numReturnedLocs, &canFree);
	int numLocs = 0;
	// getting current location
	PlaceId currLoc = GvGetPlayerLocation(dv->gameState, PLAYER_DRACULA);
	// getting all possible locations
	PlaceId *validLocs = GvGetReachableByType(dv->gameState, PLAYER_DRACULA, dv->currRound, currLoc, road, false, boat, &numLocs);
	// counter to keep track of the number of valid moves
	int nValidLocs = 0;
	// getting locations that can be moved to
	for (int i = 0; i < numLocs; i++) {
		int notInTrail = 0;
		for (int j = 1; j < TRAIL_SIZE; j++) {
			// comparing from the second element in the trail
			if (validLocs[i] != trail[j]) {
				notInTrail++;
			}
			if (notInTrail == 5) {
				nValidLocs++;
				validLocs[i] = validLocs[i];
			}
		} 
	}
	// there are no locations to be found
	if (nValidLocs == 0) {
		numReturnedLocs = 0;
		return NULL;
	}
	PlaceId *moves = malloc(sizeof(PlaceId) * nValidLocs);
	*numReturnedLocs = nValidLocs;
	for (int i = 0; i < nValidLocs; i++) {
		moves[i] = validLocs[i];
	}
	free(validLocs);
	free(trail);
	return moves;
}

// TODO: Gabriel
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

// TODO: Gabriel
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

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
