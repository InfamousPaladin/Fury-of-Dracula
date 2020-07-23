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
#include <string.h>

#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"
// add your own #includes here

#define TURN_CHARS	8	// chars each turn takes in play string (w space)
#define POS_ACTIONS 6 	// player actions; 2 for location; 4 for rest
#define PLRACT_STRING 7 // each player string length
#define ROUND_CHARS	40 	// chars each round takes in play string (w space)

// each player is given player information
// this information is mapped to playerID
typedef struct playerInfo {
	int health;
	Player name;
	Place location;
} playerInfo;

struct gameView {
	int score;
	Round round;
	Player currPlayer;
	Place imvampireLocation;
	playerInfo playerID[NUM_PLAYERS];
	// Maybe: add a field for messages array here
	char *playString; // Stores all past plays (i.e. game log)
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

GameView GvNew(char *pastPlays, Message messages[]) {
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION

	// Basically summarises the current state of the game
	// pastPlays variable = gamelog
	// messages array holds each play (same number of elements as pastPlays)
	// first play will be at index 0.
	GameView new = malloc(sizeof(struct gameView));
	new->playString = pastPlays;
	/*
	GameView new = malloc(sizeof(*new));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}
	*/

	return new;
}

void GvFree(GameView gv) {
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	free(gv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round GvGetRound(GameView gv) {

	int numMoves = 1;
	for (int i = 0; gv->playString[i] != '\0'; i++) {
		if (gv->playString[i] == ' ') numMoves++;
	}

	gv->round = numMoves / NUM_PLAYERS;
	return gv->round;
}

Player GvGetPlayer(GameView gv) {

	int playerNum = 1;
	for (int i = 0; gv->playString[i] != '\0'; i++) {
		
		// calculates which player is next in turn
		if (gv->playString[i] == 'G' || gv->playString[i] == 'S' ||
            gv->playString[i] == 'H' || gv->playString[i] == 'M' ||
            gv->playString[i] == 'D') {
			i += POS_ACTIONS;
			playerNum++;
		}

		// returns back to playerID 1 after playerID 5
		if (playerNum == NUM_PLAYERS + 1) playerNum = 1;
	}

	if (playerNum == 1) {
		gv->currPlayer = PLAYER_LORD_GODALMING;
	} else if (playerNum == 2) {
		gv->currPlayer = PLAYER_DR_SEWARD;
	} else if (playerNum == 3) {
		gv->currPlayer = PLAYER_VAN_HELSING;
	} else if (playerNum == 4) {
		gv->currPlayer = PLAYER_MINA_HARKER;
	} else if (playerNum == 5) {
		gv->currPlayer = PLAYER_DRACULA;
	}

	return gv->currPlayer;
}

int GvGetScore(GameView gv) {
	return gv->score;
}

int GvGetHealth(GameView gv, Player player) {
	return gv->playerID[player].health;
}

// helper function for GvGetPlayerLocation()
static PlaceId GvDraculaDoubleBack(GameView gv, Place playerLoc, int roundBack) {

	int numMoves = 0;
	for (int i = 0; gv->playString[i] != '\0'; i++) {

		// calculates number of moves in a given
		// range of rounds
		if (gv->playString[i] == ' ') numMoves++;
		int countRound = numMoves / 5;
		if (countRound == roundBack) break;
	}

	// calculating number of individual chars in a given range of rounds
	// finding Dracula's position after his use of DOUBLE_BACK_N
	for (int i = 0; i <= (numMoves + PLRACT_STRING * NUM_PLAYERS * roundBack); i++) {

		if (gv->playString[i] == 'D' && gv->playString[i + 1] != '.' && 
			gv->playString[i + 2] != '.') {

			// obtain two initials of place
			playerLoc.abbrev[0] = gv->playString[i + 1];
			playerLoc.abbrev[1] = gv->playString[i + 2];
			playerLoc.abbrev[2] = '\0';

			// get placeID
			playerLoc.id = placeAbbrevToId(playerLoc.abbrev);
		}
	}

	return playerLoc.id;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player) {

	Place playerLoc;
	int roundBack = 0;
	char nameInitials;
	char placeAbbrev[3];
	bool foundLocation = false;
	playerLoc.abbrev = placeAbbrev;

	// mapping to initials
	if (player == PLAYER_LORD_GODALMING) {
		nameInitials = 'G';
	} else if (player == PLAYER_DR_SEWARD) {
		nameInitials = 'S';
	} else if (player == PLAYER_VAN_HELSING) {
		nameInitials = 'H';
	} else if (player == PLAYER_MINA_HARKER) {
		nameInitials = 'M';
	} else if (player == PLAYER_DRACULA) {
		nameInitials = 'D';
	}

	for (int i = 0; gv->playString[i] != '\0'; i++) {
		
		// need to fix this dual-condition thing
		if (gv->playString[i] == nameInitials && i == 0) {

			// obtain two initials of place
			playerLoc.abbrev[0] = gv->playString[i + 1];
			playerLoc.abbrev[1] = gv->playString[i + 2];
			playerLoc.abbrev[2] = '\0';

			// get placeID
			playerLoc.id = placeAbbrevToId(playerLoc.abbrev);

			//playerNum++;
			i += POS_ACTIONS;
			foundLocation = true;			

		} else if (gv->playString[i] == nameInitials && 
				   gv->playString[i - 1] == ' '      && i > 0) {

			// obtain two initials of place
			playerLoc.abbrev[0] = gv->playString[i + 1];
			playerLoc.abbrev[1] = gv->playString[i + 2];
			playerLoc.abbrev[2] = '\0';

			// get placeID
			playerLoc.id = placeAbbrevToId(playerLoc.abbrev);

			//playerNum++;
			i += POS_ACTIONS;
			foundLocation = true;
		}
	}

	// various conditions obtained from rules
	if (foundLocation == false) {

		return NOWHERE;

	} else if (player == PLAYER_DRACULA) {

		// Dracula gets teleported to CASTLE_DRACULA
		if (playerLoc.id == TELEPORT) {

			gv->playerID[PLAYER_DRACULA].location.id = CASTLE_DRACULA;
			return gv->playerID[PLAYER_DRACULA].location.id;

		// Not really sure about his, under work
		} else if (playerLoc.id == HIDE) {

			if (gv->playerID[PLAYER_DRACULA].location.id != 0) {
				return gv->playerID[PLAYER_DRACULA].location.id;
			}

		// getting location back if Dracula uses DOUBLE_BACK_N
		} else if (playerLoc.id >= DOUBLE_BACK_1 && playerLoc.id <= DOUBLE_BACK_5) {

			int movebackBy = 0;
			if (playerLoc.id == DOUBLE_BACK_1) {
				movebackBy = 1;
			} else if (playerLoc.id == DOUBLE_BACK_2) {
				movebackBy = 2;
			} else if (playerLoc.id == DOUBLE_BACK_3) {
				movebackBy = 3;
			} else if (playerLoc.id == DOUBLE_BACK_4) {
				movebackBy = 4;
			} else if (playerLoc.id == DOUBLE_BACK_5) {
				movebackBy = 5;
			}

			roundBack = GvGetRound(gv) - movebackBy;
			while (playerLoc.id > SEA_UNKNOWN) {
				playerLoc.id = GvDraculaDoubleBack(gv, playerLoc, roundBack);
				roundBack--;
			}
		}
	}

	gv->playerID[player].location.id = playerLoc.id;
	return gv->playerID[player].location.id;
}

PlaceId GvGetVampireLocation(GameView gv) {

	char placeAbbrev[3];
	Place immvampireLoc;
	bool foundLocation = false;
	immvampireLoc.abbrev = placeAbbrev;

	for (int i = 0; gv->playString[i] != '\0'; i++) {

		if (gv->playString[i] == 'D' && gv->playString[i + 4] == 'V') {

			// obtain two initials of place
			immvampireLoc.abbrev[0] = gv->playString[i + 1];
			immvampireLoc.abbrev[1] = gv->playString[i + 2];
			immvampireLoc.abbrev[2] = '\0';

			// get placeID
			foundLocation = true;
			immvampireLoc.id = placeAbbrevToId(immvampireLoc.abbrev);

		} else if (gv->playString[i] == 'D' && gv->playString[i + 5] == 'V') {

			// vampire has hatched
			gv->imvampireLocation.id = NOWHERE;
			return gv->imvampireLocation.id;
		}
	}

	// cases where vampire is not present
	// or hunter killed it before it matured
	if (foundLocation == false) {
		return NOWHERE;
	} else if (GvGetPlayerLocation(gv, PLAYER_DR_SEWARD) == immvampireLoc.id) {
		if (GvGetRound(gv) < 6) return NOWHERE;
	} else if (GvGetPlayerLocation(gv, PLAYER_VAN_HELSING) == immvampireLoc.id) {
		if (GvGetRound(gv) < 6) return NOWHERE;
	} else if (GvGetPlayerLocation(gv, PLAYER_MINA_HARKER) == immvampireLoc.id) {
		if (GvGetRound(gv) < 6) return NOWHERE;
	} else if (GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == immvampireLoc.id) {
		if (GvGetRound(gv) < 6) return NOWHERE;
	}

	gv->imvampireLocation.id = immvampireLoc.id;
	return gv->imvampireLocation.id;
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
