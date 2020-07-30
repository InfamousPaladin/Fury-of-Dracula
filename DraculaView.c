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
	Player currPlayer; 					// whos turn
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
	dv->currRound = GvGetRound(dv->gameState);
	return dv->currRound;
}

int DvGetScore(DraculaView dv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetScore(dv->gameState);
}

int DvGetHealth(DraculaView dv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetHealth(dv->gameState, player);
}

PlaceId DvGetPlayerLocation(DraculaView dv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return GvGetPlayerLocation(dv->gameState, player);
}

// TODO: Gabriel
PlaceId DvGetVampireLocation(DraculaView dv)
{
	return GvGetVampireLocation(dv->gameState);
}

// TODO: Gabriel
PlaceId *DvGetTrapLocations(DraculaView dv, int *numTraps)
{
	return GvGetTrapLocations(dv->gameState, &*numTraps);
}

////////////////////////////////////////////////////////////////////////
// Making a Move
// TODO: Christian
PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	// Remember that dracula has a trail, meaning that he cannot
	// move to any city hes been in within the last 6 rounds, unless he uses
	// a HIDE or a DOUBLE_BACK
	// For the trail the function GvGetLastMoves would be usefull
	
	// NOTE: This function keeps track of the location moves (reachable), as well
	// as the special moves

	// getting draculas current "trail"
	bool canFree = true;
	int lastNMoves = -1;
	PlaceId *trail = GvGetLastMoves(dv->gameState, PLAYER_DRACULA, TRAIL_SIZE, &lastNMoves, &canFree);
	PlaceId validMoves[100];
	PlaceId currLoc = GvGetPlayerLocation(dv->gameState, PLAYER_DRACULA);
	int numLocs = 0;
	// getting all possible locations
	PlaceId *reachLocs = GvGetReachable(dv->gameState, PLAYER_DRACULA, dv->currRound, currLoc, &numLocs);
	// counter to keep track of the number of valid moves
	int nElements = 0;
	// remember Double_back requires dracula to still be adjacent to it
	// checking if HIDE or DOUBLE_BACK can be used
	bool hideExist = false;
	bool dbExist = false;
	for (int j = 1; j < TRAIL_SIZE; j++) {
		// Checking if the move has been called within the last 5 rounds
		if (trail[j] == HIDE) {
			hideExist = true;
		}
		if (trail[j] == DOUBLE_BACK_1) {
			dbExist = true;
		} else if (trail[j] == DOUBLE_BACK_2) {
			dbExist = true;
		} else if (trail[j] == DOUBLE_BACK_3) {
			dbExist = true;
		} else if (trail[j] == DOUBLE_BACK_4) {
			dbExist = true;
		} else if (trail[j] == DOUBLE_BACK_5) {
			dbExist = true;
		}
	}
	// if the place is on land and hide is available
	if (placeIsLand(currLoc) && !hideExist) {
		for (int i = 0; i < numLocs; i++) {
			if (reachLocs[i] == currLoc) {
				validMoves[nElements] = HIDE;
				nElements++;
				break;
			}
		}
	}
	// considering the Double_back moves
	if (!dbExist) {
		bool isAdjacent = false;
		for (int i = lastNMoves - 1; i >= 0; i--) {
			for (int j = 0; j < numLocs; j++) {
				if (trail[i] == reachLocs[j]) {
					// if it exists in the trail, it means its adjacent
					// or it is the current location
					isAdjacent = true;
					break;
				}
			}
			if (isAdjacent) {
				int last = lastNMoves - 1;
				if (last - i == 1) {
					validMoves[nElements] = DOUBLE_BACK_1;
				} else if (last - i == 2) {
					validMoves[nElements] = DOUBLE_BACK_2;
				} else if (last - i == 3) {
					validMoves[nElements] = DOUBLE_BACK_3;
				} else if (last - i == 4) {
					validMoves[nElements] = DOUBLE_BACK_4;
				} else if (last - i == 5) {
					validMoves[nElements] = DOUBLE_BACK_5;
				}
				nElements++; // TODO:
			}
		}
	}
	for (int i = 0; i < lastNMoves; i++) {
		for (int j = 0; j < numLocs; j++) {
			if (trail[i] == reachLocs[j] || reachLocs[j] == currLoc) {
				reachLocs[j] = INVALID_LOC;
			}
		}
	}
	// now adding any reachable locations 
	for (int i = 0; i < numLocs; i++) {
		if (reachLocs[i] != INVALID_LOC) {
			validMoves[nElements] = reachLocs[i];
			nElements++;
		}
	}
	// if there are no valid moves
	if (nElements == 0) {
		*numReturnedMoves = 0;
		return NULL;
	}
	*numReturnedMoves = nElements;
	PlaceId *moves = malloc(sizeof(PlaceId) * nElements);
	for (int i = 0; i < nElements; i++) {
		moves[i] = validMoves[i];
	}
	free(trail);

	return moves;
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
	return GvGetReachable(dv->gameState, player, dv->currRound, dv->playerID[player].location, &*numReturnedLocs);
}

// TODO: Gabriel
PlaceId *DvWhereCanTheyGoByType(DraculaView dv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	return GvGetReachableByType(dv->gameState, player, dv->currRound, 
								dv->playerID[player].location, 
								road, rail, boat, &*numReturnedLocs);
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
