////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// HunterView.c: the HunterView ADT implementation
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
#include "HunterView.h"
#include "Map.h"
#include "Places.h"

// add your own #includes here
#define TURN_CHARS	8	// chars each turn takes in play string (w space)
#define ROUND_CHARS	40 	// chars each round takes in play string (w space)
#define POS_ACTIONS 6 	// player actions; 2 for location; 4 for rest
#define PLRACT_STRING 7 // each player string length
#define	START_RAIL_DIST	1
#define UNINTIALISED	-1	// path not found yet

// each player is given player information
// this information is mapped to playerInfo
typedef struct playerInfo {
	int health;
	Player name;
	Place location;
} playerInfo;

struct hunterView {
	Round round; // keeps track of the round
	Map map; // map of the board
	Player currPlayer; // whos turn
	int score; // current score of the game
	playerInfo playerID[NUM_PLAYERS];
	char *playString; // Stores all past plays (i.e. game log)
	int nPlaces; // number of places/cities in map
	Place imvampireLocation; // keeps track of vampires
};

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

HunterView HvNew(char *pastPlays, Message messages[])
{
	// asserts
	assert(pastPlays != NULL);
	assert(messages != NULL);

	// allocating memory for HunterView
	HunterView new = malloc(sizeof(struct hunterView));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}
	// assigning a playstring
	new->playString = pastPlays;

	return new;
}

void HvFree(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	free(hv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round HvGetRound(HunterView hv)
{
	int numMoves = 1;
	for (int i = 0; hv->playString[i] != '\0'; i++) {
		if (hv->playString[i] == ' ') numMoves++;
	}

	hv->round = numMoves / NUM_PLAYERS;
	return hv->round;
}

Player HvGetPlayer(HunterView hv)
{
	int playerNum = 1;
	for (int i = 0; hv->playString[i] != '\0'; i++) {
		
		// calculates which player is next in turn
		if (hv->playString[i] == 'G' || hv->playString[i] == 'S' ||
            hv->playString[i] == 'H' || hv->playString[i] == 'M' ||
            hv->playString[i] == 'D') {
			i += POS_ACTIONS;
			playerNum++;
		}

		// returns back to playerID 1 after playerID 5
		if (playerNum == NUM_PLAYERS + 1) playerNum = 1;
	}

	if (playerNum == 1) {
		hv->currPlayer = PLAYER_LORD_GODALMING;
	} else if (playerNum == 2) {
		hv->currPlayer = PLAYER_DR_SEWARD;
	} else if (playerNum == 3) {
		hv->currPlayer = PLAYER_VAN_HELSING;
	} else if (playerNum == 4) {
		hv->currPlayer = PLAYER_MINA_HARKER;
	} else if (playerNum == 5) {
		hv->currPlayer = PLAYER_DRACULA;
	}

	return hv->currPlayer;
}

int HvGetScore(HunterView hv)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

int HvGetHealth(HunterView hv, Player player)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	return 0;
}

// helper function for GvGetPlayerLocation()
static PlaceId HvDraculaDoubleBack(HunterView hv, Place playerLoc, int roundBack) {

	int numMoves = 0;
	for (int i = 0; hv->playString[i] != '\0'; i++) {
		// calculates number of moves in a given
		// range of rounds
		if (hv->playString[i] == ' ') numMoves++;
		int countRound = numMoves / 5;
		if (countRound == roundBack) break;
	}

	// calculating number of individual chars in a given range of rounds
	// finding Dracula's position after his use of DOUBLE_BACK_N
	for (int i = 0; i <= (numMoves + PLRACT_STRING * NUM_PLAYERS * roundBack); i++) {

		if (hv->playString[i] == 'D' && hv->playString[i + 1] != '.' && 
			hv->playString[i + 2] != '.') {

			// obtain two initials of place
			playerLoc.abbrev[0] = hv->playString[i + 1];
			playerLoc.abbrev[1] = hv->playString[i + 2];
			playerLoc.abbrev[2] = '\0';

			// get placeID
			playerLoc.id = placeAbbrevToId(playerLoc.abbrev);
		}
	}

	// printf("%s\n", placeIdToName(playerLoc.id));

	return playerLoc.id;
}

PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{
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

	// uncomment when get health
	// has been implemented
	// return Hospital if dead
	/*
	if (player != PLAYER_DRACULA) {
		if (player == PLAYER_DR_SEWARD && 
			HvGetHealth(hv, PLAYER_DR_SEWARD) == 0) {
			return HOSPITAL_PLACE;
		} else if (player == PLAYER_VAN_HELSING && 
			HvGetHealth(hv, PLAYER_VAN_HELSING) == 0) {
			return HOSPITAL_PLACE;
		} else if (player == PLAYER_MINA_HARKER && 
			HvGetHealth(hv, PLAYER_MINA_HARKER) == 0) {
			return HOSPITAL_PLACE;
		} else if (player == PLAYER_LORD_GODALMING && 
			HvGetHealth(hv, PLAYER_LORD_GODALMING) == 0) {
			return HOSPITAL_PLACE;
		}
	}
	*/

	for (int i = 0; hv->playString[i] != '\0'; i++) {
		
		// need to fix this dual-condition thing
		if (hv->playString[i] == nameInitials && i == 0) {

			// obtain two initials of place
			playerLoc.abbrev[0] = hv->playString[i + 1];
			playerLoc.abbrev[1] = hv->playString[i + 2];
			playerLoc.abbrev[2] = '\0';

			// get placeID
			playerLoc.id = placeAbbrevToId(playerLoc.abbrev);

			//playerNum++;
			i += POS_ACTIONS;
			foundLocation = true;			

		} else if (hv->playString[i] == nameInitials && 
				   hv->playString[i - 1] == ' '      && i > 0) {

			// obtain two initials of place
			playerLoc.abbrev[0] = hv->playString[i + 1];
			playerLoc.abbrev[1] = hv->playString[i + 2];
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

			hv->playerID[PLAYER_DRACULA].location.id = CASTLE_DRACULA;
			return hv->playerID[PLAYER_DRACULA].location.id;

		// Not really sure about his, under work
		} else if (playerLoc.id == HIDE) {

			if (hv->playerID[PLAYER_DRACULA].location.id != 0) {
				return hv->playerID[PLAYER_DRACULA].location.id;
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

			roundBack = HvGetRound(hv) - movebackBy;
			while (playerLoc.id > SEA_UNKNOWN) {
				playerLoc.id = HvDraculaDoubleBack(hv, playerLoc, roundBack);
				roundBack--;
			}
		}
	}

	hv->playerID[player].location.id = playerLoc.id;
	return hv->playerID[player].location.id;
}

PlaceId HvGetVampireLocation(HunterView hv)
{
	Place playerLoc;
	char playerPlace[3];
	char placeAbbrev[3];
	Place immvampireLoc;
	bool foundLocation = false;
	playerLoc.abbrev = playerPlace;
	immvampireLoc.abbrev = placeAbbrev;

	// finding immvampires location
	for (int i = 0; hv->playString[i] != '\0'; i++) {

		if (hv->playString[i] == 'D' && hv->playString[i + 4] == 'V') {

			// obtain two initials of place
			immvampireLoc.abbrev[0] = hv->playString[i + 1];
			immvampireLoc.abbrev[1] = hv->playString[i + 2];
			immvampireLoc.abbrev[2] = '\0';

			// get placeID
			foundLocation = true;
			immvampireLoc.id = placeAbbrevToId(immvampireLoc.abbrev);

		} else if (hv->playString[i] == 'D' && hv->playString[i + 5] == 'V') {

			// vampire has hatched
			hv->imvampireLocation.id = NOWHERE;
			return hv->imvampireLocation.id;
		}
	}

	// immvampire not found
	if (foundLocation == false) return NOWHERE;

	// checks if hunter has been in the same place as the
	// immvampire for the last 6 rounds and kills the vampire if yes
	// therefore, location of immvampire is NOWHERE as he is dead
	for (int i = 0; hv->playString[i] != '\0'; i++) {
		
		// need to fix this dual-condition thing
		if (hv->playString[i] == 'G' || hv->playString[i] == 'S' ||
            hv->playString[i] == 'H' || hv->playString[i] == 'M') {

			// obtain two initials of place
			playerLoc.abbrev[0] = hv->playString[i + 1];
			playerLoc.abbrev[1] = hv->playString[i + 2];
			playerLoc.abbrev[2] = '\0';

			// get placeID
			playerLoc.id = placeAbbrevToId(playerLoc.abbrev);

			// immvampire encountered and killed instantly
			if (playerLoc.id == immvampireLoc.id) {
				if (HvGetRound(hv) < 6) return NOWHERE;
			}
			//playerNum++;
			i += POS_ACTIONS;
		}
	}

	hv->imvampireLocation.id = immvampireLoc.id;
	return hv->imvampireLocation.id;
}

////////////////////////////////////////////////////////////////////////
// Utility Functions - Done by Prathamesh

PlaceId HvGetLastKnownDraculaLocation(HunterView hv, Round *round)
{
	Place playerLoc;
	char playerPlace[3];
	playerLoc.abbrev = playerPlace;
	int roundBack = HvGetRound(hv) - 1;
	playerLoc.id = HvGetPlayerLocation(hv, PLAYER_DRACULA);

	while (playerLoc.id > ZURICH) {
		// can't find a location
		if (roundBack < 0) return NOWHERE;
		playerLoc.id = HvDraculaDoubleBack(hv, playerLoc, roundBack);
		roundBack--;
	}

	*round = roundBack;
	return playerLoc.id;
}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*pathLength = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Making a Move

// Prathamesh
PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player,
                          int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

// Prathamesh
PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numReturnedLocs = 0;
	return NULL;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
