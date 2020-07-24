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

#define IMMATURE_VAMPIRE 'V'
#define TRAP     	     'T'
#define NOT_A_TRAP       '.'

// TODO: ADD YOUR OWN STRUCTS HERE
typedef struct player *PlayerInfo
struct player {
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
	PlayerInfo *players[NUM_PLAYERS]; // 
	char *playString; // Stores all past plays (i.e. game log)
	char *trapLocations; // Stores all the locations where traps are 
	// (can store multiple copies of a place if there is more than one trap)
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
	return gv->round;
}

Player GvGetPlayer(GameView gv)
{
	return gv->currPlayer;
}

int GvGetScore(GameView gv)
{
	// Check all ways scores can be reduced
	// Each round that goes by that Dracula is alive
	if (gv->players[4]->health > 0) gv->score -= SCORE_LOSS_DRACULA_TURN;
	// Hunters dying
	for (int i = 0; i < NUM_PLAYERS; i++) {
		if (gv->players[i]->health <= 0) gv->score -= SCORE_LOSS_HUNTER_HOSPITAL;
	} 
	// A vampire matures
	if (GvGetVampireLocation(gv) != NOWHERE) {
		PlaceId vampLocation = GvGetVampireLocation(gv);
		PlaceId dracTrail = GvGetLastMoves(gv, PLAYER_DRACULA, TRAIL_SIZE, TRAIL_SIZE, true);
		// Check trail if the immature vampire is still on the trail
		int vampMature = 1;
		for (int i = 0; i < TRAIL_SIZE; i++) {
			// If it is there, then the vampire isn't matured yet
			if (vampLocation == dracTrail[i]) vampMature = 0;
		}
		if (vampMature == 1) gv->score -= SCORE_LOSS_VAMPIRE_MATURES;
	}
	return gv->score;
}

int GvGetHealth(GameView gv, Player player)
{
	// Return the health of the given player
	// String to hold location initials
	Place location;
	char placeAbbrev[2];
	location.abbrev = placeAbbrev;
	// First get the round number
	gv->round = GvGetRound(gv);
	// Extract intial of player
	int playerID = player;
	if (playerID == 0) playerID = 'G';
	if (playerID == 1) playerID = 'S';
	if (playerID == 2) playerID = 'H';
	if (playerID == 3) playerID = 'M';
	if (playerID == 4) playerID = 'D';
	// Traverse through playString round to find 'G', 'S', 'H' or 'M'
	for (int i = 8 * gv->round; gv->playString[i] != '\0'; i += 8) {
		// Check the hunter's POV
		if (gv->playString[i] == playerID && playerID != 'D') {
			// Check the last four characters to see if anything has happened
			for (int j = 3; j < 7; j++) {
				if (gv->playString[i + j] != '.') {
					if (gv->playString[i + j] == TRAP) {
						gv->players[player]->health -= LIFE_LOSS_TRAP_ENCOUNTER;
					} else if (gv->playString[i + j] == IMMATURE_VAMPIRE) {
						// I'm not sure what happens here
					} else if (gv->playString[i + j] == 'D') {
						gv->players[player]->health -= LIFE_LOSS_DRACULA_ENCOUNTER;
						gv->players[4]->health -= LIFE_LOSS_HUNTER_ENCOUNTER;
					}
				} 
				// Check if the health is below 0
				if (gv->players[4]->health <= 0) {
					// Dracula is dead (game is over)
					break;
				}
				if (gv->players[player]->health <= 0) {
					gv->players[player]->health = 0;
					// The player is dead and is teleported to the hospital
					gv->players[player]->location = HOSPITAL_PLACE;
					break;
				}
			}
			// Check if the hunter rested
			if (gv->round != 0) {
				if (gv->playString[i + 1] == gv->playString[i + 1 - 32]) {
					if (gv->playString[i + 2] == gv->playString[i + 2 - 32]) {
						// This means the hunter stayed in the same location
						gv->players[player]->health += LIFE_GAIN_REST;
					}
				}
				// Cap the hunter's health at 9 if they get above it
				if (gv->players[player]->health > 9) gv->players[player]->health = 9;
			}
		} 
		// Check Dracula's POV
		if (gv->playString[i] == playerID && playerID == 'D') {
			// Check if Dracula TP to his castle
			location.abbrev[0] = gv->playString[i + 1];
			location.abbrev[1] = gv->playString[i + 2];
			// Convert the abbrev to name
			location.id = placeAbbrevToId(traps.abbrev);
			if (location.id == TELEPORT || location.id == CASTLE_DRACULA) {
				gv->players[player]->location = CASTLE_DRACULA;
				gv->players[player]->health += LIFE_GAIN_CASTLE_DRACULA;
			// Check if Dracula is at sea
			} else if (placeIsSea(location.id) == true) {
				gv->players[player]->health -= LIFE_LOSS_SEA;
			}
			// Check if Dracula is dead
			if (gv->players[4]->health <= 0) {
				// Dracula is dead (game is over)
				break;
			}
		}
	}
	return gv->players[player]->health;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	return gv->playerID[player]->location;
}

PlaceId GvGetVampireLocation(GameView gv)
{
	// Dracula's playerID is 5
	return gv->imvampireLocation;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	// Number of traps in playStrings ('T')
	*numTraps = 0;
	// String to hold location initials
	Place traps;
	char placeAbbrev[2];
	traps.abbrev = placeAbbrev;
	// Counter for location storage
	int counterLocation = 0;
	// Transverses through the stirng at the position 'D'
	for (int i = 32; gv->playString[i] != '\0'; i += 32) {
		// Check every D for a 'T'
		if (gv->playString[i + 3] == TRAP) {
			// Increase the number of traps
			numTraps++;
			// Extract the location name
			traps.abbrev[0] = gv->playString[i + 1];
			traps.abbrev[1] = gv->playString[i + 2];
			// Convert the abbrev to name
			traps.id = placeAbbrevToId(traps.abbrev);
			// Store the location in the array
			gv->trapLocations[counterLocation] = traps.id;
			counterLocation++;
		}
		// Decrease the numTraps if the trap has expired
		if (gv->playString[i + 5] == 'M') {
			numTraps--;
			// Remove the oldest location (first element)
			for (int j = 1; j < counterLocation; j++) {
				gv->trapLocations[j - 1] = gv->trapLocations[j];
			}
		}
	}
	// Variable to store last known location of the trap
	PlaceId lastKnownTrap = gv->trapLocations[counterLocation - 1];
	return lastKnownTrap;
}

////////////////////////////////////////////////////////////////////////
// Game History

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
		Place curr;
		char placeAbbrev[2];
		curr.abbrev = placeAbbrev;

		// Formula to calculate index of the player location in a given round
		int currTurn = TURN_CHARS * player + ROUND_CHARS * i;
		curr.abbrev[0] = gv->playString[currTurn + 1]; 
		curr.abbrev[1] = gv->playString[currTurn + 2];

		curr.id = placeAbbrevToId(curr.abbrev);
		moves[i] = curr.id;
	}
	*numReturnedMoves = i;

	*canFree = false;
	return moves;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{

	// NOTE: This  function is very similar to GvGetMoveHistory, except that
	//       it gets only the last `numMoves` moves rather than the complete
	//       move history.

	// Test - placeholder data
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
	// Error checks
	if (startIndex < 0) startIndex = 0;
	if (numMoves > gv->round) numMoves = gv->round;
	PlaceId *moves = malloc(sizeof(PlaceId) * numMoves);


	int i;
	for (i = 0; i < numMoves; i++, startIndex++) {
		Place curr;
		char placeAbbrev[2];
		curr.abbrev = placeAbbrev;

		// Formula to calculate index of the player location in a given round
		int currTurn = TURN_CHARS * player + ROUND_CHARS * startIndex;
		curr.abbrev[0] = gv->playString[currTurn + 1]; 
		curr.abbrev[1] = gv->playString[currTurn + 2];
		
		curr.id = placeAbbrevToId(curr.abbrev);
		moves[i] = curr.id;
	}
	*numReturnedMoves = i;

	*canFree = false;
	return moves;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
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

	if (player != PLAYER_DRACULA) {
		return GvGetMoveHistory(gv, player, numReturnedLocs, canFree);
	} else {
		if (player < gv->currPlayer) gv->round++; 
		PlaceId *moves = malloc(sizeof(PlaceId) * (gv->round));

		int i;
		for (i = 0; i < gv->round; i++) {
			Place curr;
			char placeAbbrev[2];
			curr.abbrev = placeAbbrev;

			// Formula to calculate index of the player location in a given round
			int currTurn = TURN_CHARS * player + ROUND_CHARS * i;
			curr.abbrev[0] = gv->playString[currTurn + 1]; 
			curr.abbrev[1] = gv->playString[currTurn + 2];

			curr.id = placeAbbrevToId(curr.abbrev);
			
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

			moves[i] = curr.id;
		}
		*numReturnedLocs = i;

		*canFree = false;
		return moves;
	
	}
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	// NOTE: This function is very similar to  GvGetLocationHistory,  except
	// that  it gets only the last `numLocs` locations rather than the
    // complete location history.

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

	if (player != PLAYER_DRACULA) {
		return GvGetLastMoves(gv, player, numLocs, numReturnedLocs, canFree);
	} else {
		// TODO
		return 0;
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
