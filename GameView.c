////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// GameView.c: GameView ADT implementation
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10   v3.0    Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"
// add your own #includes here

#define TURN_CHARS	8	// chars each turn takes in play string (w space)
#define ROUND_CHARS	40 	// chars each round takes in play string (w space)

// TODO: ADD YOUR OWN STRUCTS HERE
typedef struct playerinformation *PlayerInfo;
struct playerinformation {
	int health;
	Player name;
	PlaceId location;
};


struct gameView 
{
	Round round; // keeps track of the round
	Map map; // map of the board
	Player currPlayer; // whos turn
	int score; // current score of the game
	PlayerInfo players[NUM_PLAYERS]; // 
	char *playString; // Stores all past plays (i.e. game log)
};

// Helper Function Prototypes
static Place getPlaceId (GameView gv, Player player, int round);
////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

GameView GvNew(char *pastPlays, Message messages[])
{
	// // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION

	// Basically summarises the current state of the game
	// pastPlays variable = gamelog
	// messages array holds each play (same number of elements as pastPlays)
	// first play will be at index 0.
	assert(pastPlays != NULL);
	assert(messages != NULL);

	GameView new = malloc(sizeof(struct gameView));
	assert(new != NULL);

	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}

	new->map = MapNew();
	// PlayerInfo s = malloc(sizeof(PlayerInfo) * NUM_PLAYERS);
	// getting the current location of players
	new->players[0]->location = GvGetPlayerLocation(new, PLAYER_LORD_GODALMING);
	new->players[1]->location = GvGetPlayerLocation(new, PLAYER_DR_SEWARD);
	new->players[2]->location = GvGetPlayerLocation(new, PLAYER_VAN_HELSING);
	new->players[3]->location = GvGetPlayerLocation(new, PLAYER_MINA_HARKER);
	new->players[4]->location = GvGetPlayerLocation(new, PLAYER_DRACULA);
	// the game just started
	if (pastPlays[0] == '\0') {
		new->score = GAME_START_SCORE;
		new->round = 0;
		// initialising players health at the start of the game
		new->players[0]->health = GAME_START_HUNTER_LIFE_POINTS;
		new->players[1]->health = GAME_START_HUNTER_LIFE_POINTS;
		new->players[2]->health = GAME_START_HUNTER_LIFE_POINTS;
		new->players[3]->health = GAME_START_HUNTER_LIFE_POINTS;
		new->players[4]->health = GAME_START_BLOOD_POINTS;
	} else {
		// the game has been going on.
		int i = 0;
		// pastPlays keeps track of the number of rounds, through indexs
		while (pastPlays[i] != '\0') i++;
		new->round = i;
		// calculating the gamescore
		new->score = GvGetScore(new);
		new->players[0]->health = GvGetHealth(new, PLAYER_LORD_GODALMING);
		new->players[1]->health = GvGetHealth(new, PLAYER_DR_SEWARD);
		new->players[2]->health = GvGetHealth(new, PLAYER_VAN_HELSING);
		new->players[3]->health = GvGetHealth(new, PLAYER_MINA_HARKER);
		new->players[4]->health = GvGetHealth(new, PLAYER_DRACULA);
	}
	// getting the current player
	new->currPlayer = GvGetPlayer(new);
	new->playString = pastPlays;
	return new;
}

void GvFree(GameView gv)
{
	// // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	// free(gv->players);
	// MapFree(gv->map);
	// free(gv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round GvGetRound(GameView gv)
{
	return gv->round;
}

Player GvGetPlayer(GameView gv)
{
	return gv->currPlayer;
}

int GvGetScore(GameView gv)
{
	return gv->score;
}


int GvGetHealth(GameView gv, Player player)
{
	return gv->players[player]->health;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	return gv->players[player]->location;
}

// Returns the placeId (location) of a player for a given round
static Place getPlaceId (GameView gv, Player player, int round)
{
	Place location;
	char placeAbbrev[2];
	location.abbrev = placeAbbrev;

	// Formula to calculate index of the player location in a given round
	int playerTurn = TURN_CHARS * player + ROUND_CHARS * round;
	location.abbrev[0] = gv->playString[playerTurn + 1]; 
	location.abbrev[1] = gv->playString[playerTurn + 2];
	
	location.id = placeAbbrevToId(location.abbrev);


	return location;
}

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	// TODO: Test - placeholder data
	gv->playString =
			"GLS.... SLS.... HLS.... MGE.... DST.V.. "
			"GCA.... SAL.... HAL.... MGE.... DC?T... "
			"GGR.... SBO.... HBO.... MGE.... DC?T... "
			"GAL.... SNA.... HNA.... MGE.... DD3T... "
			"GSR.... SPA.... HPA.... MGE.... DHIT... "
			"GSN.... SST.... HST.... MGE.... DC?T... "
			"GMA.... SFLTTV.";
	gv->round = 6;
	gv->currPlayer = PLAYER_VAN_HELSING;

	if (player < gv->currPlayer) gv->round++; 
	PlaceId *moves = malloc(sizeof(PlaceId) * (gv->round));

	int i;
	for (i = 0; i < gv->round; i++) {
		Place curr = getPlaceId(gv, player, i);
		moves[i] = curr.id;
	}
	*numReturnedMoves = i;

	return moves;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	// TODO: Test - placeholder data
	gv->playString =
			"GLS.... SLS.... HLS.... MGE.... DST.V.. "
			"GCA.... SAL.... HAL.... MGE.... DC?T... "
			"GGR.... SBO.... HBO.... MGE.... DC?T... "
			"GAL.... SNA.... HNA.... MGE.... DD3T... "
			"GSR.... SPA.... HPA.... MGE.... DHIT... "
			"GSN.... SST.... HST.... MGE.... DC?T... "
			"GMA.... SFLTTV.";
	gv->round = 6;
	gv->currPlayer = PLAYER_VAN_HELSING;

	// Formula to find the last accessible move in pastPlay string 
	int startIndex = gv->round - numMoves;
	if (player < gv->currPlayer) startIndex++;

	// Error checks and bounds them
	if (startIndex < 0) startIndex = 0;
	if (numMoves > gv->round) numMoves = gv->round;

	PlaceId *moves = malloc(sizeof(PlaceId) * numMoves);

	int i;
	for (i = 0; i < numMoves; i++, startIndex++) {
		Place curr = getPlaceId(gv, player, startIndex);
		moves[i] = curr.id;
	}
	*numReturnedMoves = i;

	return moves;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	// TODO: Test - placeholder data
	gv->playString =
			"GLS.... SLS.... HLS.... MGE.... DST.V.. "
			"GCA.... SAL.... HAL.... MGE.... DC?T... "
			"GGR.... SBO.... HBO.... MGE.... DC?T... "
			"GAL.... SNA.... HNA.... MGE.... DD3T... "
			"GSR.... SPA.... HPA.... MGE.... DHIT... "
			"GSN.... SST.... HST.... MGE.... DC?T... "
			"GMA.... SFLTTV.";
	gv->round = 6;
	gv->currPlayer = PLAYER_VAN_HELSING;

	// Case where the player is a hunter, this function should behave exactly
	// the same as GvGetMoveHistory.
	if (player != PLAYER_DRACULA) {
		return GvGetMoveHistory(gv, player, numReturnedLocs, canFree);
	} else {
		if (player < gv->currPlayer) gv->round++; 
		PlaceId *moves = malloc(sizeof(PlaceId) * (gv->round));

		int i;
		for (i = 0; i < gv->round; i++) {
			Place curr = getPlaceId(gv, player, i);
			
			// Finds dracula location when performing his special moves
			if (curr.id == HIDE || curr.id == DOUBLE_BACK_1)
				curr.id = moves[i - 1];
			else if (curr.id == DOUBLE_BACK_2)
				curr.id = moves[i - 2];
			else if (curr.id == DOUBLE_BACK_3)
				curr.id = moves[i - 3];
			else if (curr.id == DOUBLE_BACK_4)
				curr.id = moves[i - 4];
			else if (curr.id == DOUBLE_BACK_5)
				curr.id = moves[i - 5];
			else if (curr.id == TELEPORT)
				curr.id = CASTLE_DRACULA;

			moves[i] = curr.id;
		}
		*numReturnedLocs = i;
		return moves;
	}
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	// TODO: Test - placeholder data
	gv->playString =
			"GLS.... SLS.... HLS.... MGE.... DST.V.. "
			"GCA.... SAL.... HAL.... MGE.... DC?T... "
			"GGR.... SBO.... HBO.... MGE.... DC?T... "
			"GAL.... SNA.... HNA.... MGE.... DD3T... "
			"GSR.... SPA.... HPA.... MGE.... DHIT... "
			"GSN.... SST.... HST.... MGE.... DC?T... "
			"GMA.... SFLTTV.";
	gv->round = 6;
	gv->currPlayer = PLAYER_VAN_HELSING;

	// Case where the player is a hunter, this function should behave exactly
	// the same as GvGetLastMoves.
	if (player != PLAYER_DRACULA) {
		return GvGetLastMoves(gv, player, numLocs, numReturnedLocs, canFree);
	} else {
		bool canFreeAllMoves = true;
		PlaceId *allMoves = GvGetLocationHistory(gv, player, numReturnedLocs,
												 &canFreeAllMoves);
		
		// Formula to find the last accessible move in pastPlay string 
		int startIndex = gv->round - numLocs;
		if (player < gv->currPlayer) startIndex++;
		// Error checks and bounds
		if (startIndex < 0) startIndex = 0;
		PlaceId *lastNMoves = malloc(sizeof(PlaceId) * numLocs);

		int i;

		// Copy values last N moves from allMoves array to lastNMoves array
		for (i = 0; i < numLocs; i++, startIndex++) {
			lastNMoves[i] = allMoves[startIndex];
		}
		*numReturnedLocs = i;

		if (canFreeAllMoves) free(allMoves);
		return lastNMoves;
	}
}

////////////////////////////////////////////////////////////////////////
// Making a Move

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION

	// First consider the two types of players, Dracula & Hunters
	// In this function we do not consider Draculas "trail"
	// Remember hunters can move by rail sea road
	// Dracula can't move by rail or move to hospitals.

	// NOTE: Remember that the distance allowed to travel by rail depends
	// on the round number.

	// Steps:
	// 1. Consider the graph of the map, and adjacent verticies (cities)
	// 2. Create a dynamically allocated array (depends on the amount of cities)
	// 3. Determine the number of adjacent cities and set *numReturnedLocs
	// 4. Consider if the player is dracula or hunter
	// if player is dracula restrict the following...
	// there is a hospital adjacent, remove from array, -1 numReturnedLocs
	// if there is a connection via rail, remove from array, -1 numReturnedLocs

	// NOTE: order does not matter in the array, as long as it contains
	// unique elements.

	// NOTE: Refering to the map is very usefull, also look at testGameView.c
	assert(gv != NULL);
	assert(placeIsReal(from));
	// getting the connections from a location
	ConnList reached = MapGetConnections(gv->map, from);
	// dynamically allocating memory in an array
	int numLinks = MapNumConnections(gv->map, ANY);
	PlaceId *reachable = malloc(sizeof(PlaceId) * numLinks);
	// the case where the player is dracula
	if (player == PLAYER_DRACULA) {

		// getting the list of connections from a given place
		// getting the appropiate transportation types
		int i = 0;
		while (reached != NULL) {
			// if it reaches hospital, reject it.
			if (reached->p != HOSPITAL_PLACE) {
				// dracula can use these methods of travel
				if (reached->type == ROAD || reached->type == BOAT) {
					// filling the reachable array
					reachable[i] = reached->p;
				}
			}
			i++;
			reached = reached->next;
		}
		*numReturnedLocs = i + 1;
		return reachable;
	}
	// the player is a hunter
	int i = 0;
	// getting the appropiate hunter numbers from each player
	// the max distance allowed by rail.
	int railDist = (round + player) % 4;
	// calculating the distance allowed to travel by rail
	while (reached != NULL) {
		if (railDist > 1 && reached->type == RAIL) {
			// depending on rail dist, add all possible cities connected
			// by rail
			
			PlaceId railways = reached->p;
			// branching out to get rail connections based on railDist
			for (int j = 1; j <= railDist; j++) {
				// would need to re-initalise this
				ConnList getRail = MapGetConnections(gv->map, railways);
				railways = getRail->p;
				while (getRail != NULL) {
					if (getRail->type == RAIL) {
						reachable[i] = reached->p;
					}
				}
			}
		} else if (reached->type == ROAD || reached->type == BOAT) {
			reachable[i] = reached->p;
		} else if (reached->type == RAIL && railDist == 1) {
			reachable[i] = reached->p;
		}
		i++;
		reached = reached->next;
	}
	*numReturnedLocs = i + 1;

	// update this variable
	// return locations in a dynamically allocated array.
	return reachable;
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION

	// Basically this function considers the connection type based on the 
	// bools inputted into the function. e.g. if road & rail = true, then
	// only include the moves available by road & rail.

	// Steps:
	// 1. Check all available moves (road, rail, boat) using the bool.
	// 2. Consider the graph of the map, and adjacent verticies (cities).
	// 3. Create a dynamically allocated array.
	// 4. Determine the number of appropiate adjacent cities.
	// 5. Consider if player is Dracula or Hunter.
	// PlaceId *reached = GvGetReachable(gv, player, round, from, numReturnedLocs);

	// if (road == TRUE && boat == TRUE && rail == TRUE) {
	// 	return reached;
	// } else if (road == TRUE && rail == TRUE) {
	// 	int numLinks = MapNumConnections(gv, ROAD) + MapNumConnections(gv, RAIL);
	// 	PlaceId *type = malloc(sizeof(PlaceId) * numLinks);
	// } else if (road == TRUE && boat == TRUE) {
	// 	int numLinks = MapNumConnections(gv, ROAD) + MapNumConnections(gv, BOAT);
	// 	PlaceId *type = malloc(sizeof(PlaceId) * numLinks);
	// } else if (boat == TRUE && rail == TRUE) 
	// 	int numLinks = MapNumConnections(gv, BOAT) + MapNumConnections(gv, RAIL);
	// 	PlaceId *type = malloc(sizeof(PlaceId) * numLinks);
	// } else if (road == TRUE) {
	// 	int numLinks = MapNumConnections(gv, ROAD);
	// 	PlaceId *type = malloc(sizeof(PlaceId) * numLinks);
	// } else if (rail == TRUE) {

	// }
	*numReturnedLocs = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
