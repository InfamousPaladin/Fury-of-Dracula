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

#define PLAY_CHARS	8 // how much chars a play takes for each player (w space)
#define NUM_PLAYERS	4

// TODO: ADD YOUR OWN STRUCTS HERE
typedef struct player *PlayerInfo
struct player {
	int health;
	Player name;
	PlaceId currLocation;
};

struct gameView {
	// TODO: ADD FIELDS HERE
	// Maybe: add a field for messages array here
	// displays what round it is.
	Round round;
	Map map;
	Player currPlayer;
	int score;
	PlayerInfo *players[NUM_PLAYERS];
	char *playString; // Stores all past plays (i.e. game log)
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

GameView GvNew(char *pastPlays, Message messages[])
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION

	// Basically summarises the current state of the game
	// pastPlays variable = gamelog
	// messages array holds each play (same number of elements as pastPlays)
	// first play will be at index 0.
	GameView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}
	// initialising all players in the game by turn order
	new->players[0].name = PLAYER_LORD_GODALMING;
	new->players[1].name = PLAYER_DR_SEWARD;
	new->players[2].name = PLAYER_VAN_HELSING;
	new->players[3].name = PLAYER_MINA_HARKER;
	new->players[4].name = PLAYER_DRACULA;
	// getting the current location of players
	new->players[0].location = GvGetPlayerLocation(new, PLAYER_LORD_GODALMING);
	new->players[1].location = GvGetPlayerLocation(new, PLAYER_DR_SEWARD);
	new->players[2].location = GvGetPlayerLocation(new, PLAYER_VAN_HELSING);
	new->players[3].location = GvGetPlayerLocation(new, PLAYER_MINA_HARKER);
	new->players[4].location = GvGetPlayerLocation(new, PLAYER_DRACULA);
	// the game just started
	if (pastPlays[0] == NULL) {
		new->score = GAME_START_SCORE;
		new->round = 0;
		// initialising players health at the start of the game
		new->players[0].health = GAME_START_HUNTER_LIFE_POINTS;
		new->players[1].health = GAME_START_HUNTER_LIFE_POINTS;
		new->players[2].health = GAME_START_HUNTER_LIFE_POINTS;
		new->players[3].health = GAME_START_HUNTER_LIFE_POINTS;
		new->players[4].health = GAME_START_BLOOD_POINTS;
	} else {
		// the game has been going on.
		int i = 0;
		// pastPlays keeps track of the number of rounds, through indexs
		while (pastPlays[i] != NULL) i++;
		new->round = i;
		// calculating the gamescore
		new->score = GvGetScore(new);
		new->players[0].health = GvGetHealth(new, PLAYER_LORD_GODALMING);
		new->players[1].health = GvGetHealth(new, PLAYER_DR_SEWARD);
		new->players[2].health = GvGetHealth(new, PLAYER_VAN_HELSING);
		new->players[3].health = GvGetHealth(new, PLAYER_MINA_HARKER);
		new->players[4].health = GvGetHealth(new, PLAYER_DRACULA);
	}
	return new;
}

void GvFree(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	free(gv->players);
	MapFree(gv->map);
	free(gv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round GvGetRound(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

Player GvGetPlayer(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return PLAYER_LORD_GODALMING;
}

int GvGetScore(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

int GvGetHealth(GameView gv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NOWHERE;
}

PlaceId GvGetVampireLocation(GameView gv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return NOWHERE;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numTraps = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Game History

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	// Steps
	// 1) Get number of turns/moves made from playString
	// 2) malloc a moves array
	// 3) Locate player and find where he moved to on each round
	// 4) find place id using functions and struc from places.h
	// 5) store id into moves array
	// 6) set *numReturnedMoves to the number of moves returned
	// 7) decide whether canFree is set to T/F. Determined by the caller
	//	  i.e. If the array is part of the  GameView  data
	//    structure,  you may not want the caller to modify or free it.

	// moves are always separated by a single space character in real pastPlays strings

	// e.g. command
	// Dracula's move/location history
	// {
	// 	int numMoves = 0; bool canFree = false;
	// 	PlaceId *moves = GvGetMoveHistory(gv, PLAYER_DRACULA,
	// 	                                  &numMoves, &canFree);
	// 	assert(numMoves == 6);
	// 	assert(moves[0] == STRASBOURG);
	// 	assert(moves[1] == CITY_UNKNOWN);
	// 	assert(moves[2] == CITY_UNKNOWN);
	// 	assert(moves[3] == DOUBLE_BACK_3);
	// 	assert(moves[4] == HIDE);
	// 	assert(moves[5] == CITY_UNKNOWN);
	// 	if (canFree) free(moves);
	// }

	gv->playString

	int round = 0;
	while (playString[playerID] have a chars in each turn) {
		round++:
	}
	char *moves = malloc(sizeof(char) * round);
	while (for every round) {
		// Use places.h specifically its sructs and function to convert place to
		// ID
		increment index for player for next round
	}

	*numReturnedMoves = round;
	*canFree = false;
	return moves;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{

	// NOTE: This  function is very similar to GvGetMoveHistory, except that
	//       it gets only the last `numMoves` moves rather than the complete
	//       move history.
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedMoves = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	// If  the given player is a hunter, this function should behave exactly
	// the same as GvGetMoveHistory. If the given  player  is  Dracula,  the
	// PlaceIds  returned  should  either  correspond  to real places, or be
	// CITY_UNKNOWN or SEA_UNKNOWN. See the comment for  GvGetPlayerLocation
	// for more details.

	// 1) determine if player is a hunter or dracula
	// 2) if player is dracula, then reveal moves
	// i.e.
	// {
	// 	int numLocs = 0; bool canFree = false;
	// 	PlaceId *locs = GvGetLocationHistory(gv, PLAYER_DRACULA,
	// 	                                     &numLocs, &canFree);
	// 	assert(numLocs == 6);
	// 	assert(locs[0] == STRASBOURG);
	// 	assert(locs[1] == CITY_UNKNOWN);
	// 	assert(locs[2] == CITY_UNKNOWN);
	// 	assert(locs[3] == STRASBOURG);
	// 	assert(locs[4] == STRASBOURG);
	// 	assert(locs[5] == CITY_UNKNOWN);
	// 	if (canFree) free(locs);
	// }

	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	*canFree = false;
	return NULL;
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	// NOTE: This function is very similar to  GvGetLocationHistory,  except
	// that  it gets only the last `numLocs` locations rather than the
    // complete location history.

	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	*canFree = false;
	return 0;
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

	PlaceId *reachable = malloc(sizeof(PlaceId) * gv->Links);


	// update this variable
	*numReturnedLocs = 0;
	// return locations in a dynamically allocated array.
	return NULL;
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

	*numReturnedLocs = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
