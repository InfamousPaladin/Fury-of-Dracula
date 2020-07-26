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
// add your own #includes here

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

PlaceId *DvGetValidMoves(DraculaView dv, int *numReturnedMoves)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	return NULL;
}

PlaceId *DvWhereCanIGo(DraculaView dv, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *DvWhereCanIGoByType(DraculaView dv, bool road, bool boat,
                             int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

// TODO: Gabriel
PlaceId *DvWhereCanTheyGo(DraculaView dv, Player player,
                          int *numReturnedLocs)
{
	*numReturnedLocs = 0;
	if (player == PLAYER_DRACULA) {
		return DvWhereCanIGo(dv, &*numReturnedLocs);
	} else {
		return GvGetReachable(dv->gameState, player, dv->currRound, PARIS, &*numReturnedLocs);	
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
