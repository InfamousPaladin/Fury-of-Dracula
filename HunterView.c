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

// The following ADT has adapted for use in this program from the COMP2521 labs.
// It was written by UNSW staff and for the purpose of the assignment, has been
// used to utilise the ADT. Queue.h has been adapted from COMP2521 lab05 2020T2.
#include "Queue.h"

// add your own #includes here
#define TURN_CHARS	8	// chars each turn takes in play string (w space)
#define ROUND_CHARS	40 	// chars each round takes in play string (w space)
#define POS_ACTIONS 6 	// player actions; 2 for location; 4 for rest
#define PLRACT_STRING 7 // each player string length
#define	START_RAIL_DIST	1
#define UNINTIALISED	-1	// path not found yet

// each player is given player information
// this information is mapped to playerInfo
struct hunterView {
	Map map; // map of the board
	GameView view;
	char *playString;
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

	// assigning a map
	new->map = MapNew();
	new->playString = pastPlays;
	new->view = GvNew(pastPlays, messages);

	return new;
}

void HvFree(HunterView hv)
{
	free(hv->view);
	MapFree(hv->map);
	free(hv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round HvGetRound(HunterView hv)
{
	return GvGetRound(hv->view);
}

Player HvGetPlayer(HunterView hv)
{
	return GvGetPlayer(hv->view);
}

int HvGetScore(HunterView hv)
{
	return GvGetScore(hv->view);
}

int HvGetHealth(HunterView hv, Player player)
{
	return GvGetHealth(hv->view, player);
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

	return playerLoc.id;
}

PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{
	return GvGetPlayerLocation(hv->view, player);
}

PlaceId HvGetVampireLocation(HunterView hv)
{
	return GvGetVampireLocation(hv->view);
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
	// num locations
	int roundoffSet = 0;
	int numlocTransport = 0;

	// arrays
	// needs to be dynamically allocated
	PlaceId src = HvGetPlayerLocation(hv, hunter);
	PlaceId *finalPath = malloc(sizeof(ConnList) * NUM_REAL_PLACES);
	PlaceId *transportPath = malloc(sizeof(ConnList) * NUM_REAL_PLACES);
	
	// path created for all three modes of travel
	PlaceId *visitTransport = malloc(sizeof(ConnList) * NUM_REAL_PLACES);

	//============================ Train Travel =============================//
	
	// -1 to indicate not visited
	for (int i = 0; i < NUM_REAL_PLACES; i++) visitTransport[i] = UNKNOWN_PLACE;

	PlaceId newLocation;
	visitTransport[src] = src;
	Queue bfsQueue = newQueue();
	bool foundpathTrain = false;

	// bfs path find by train
	QueueJoin(bfsQueue, src);
	while (foundpathTrain == false && QueueIsEmpty(bfsQueue) != 1) {
		PlaceId v = QueueLeave(bfsQueue);
		newLocation = v;
		roundoffSet++;
		if (v == dest) {
			foundpathTrain = true;
		} else {
			PlaceId *travelConnect = GvGetReachableByType(hv->view, hunter, 
			HvGetRound(hv) + roundoffSet, newLocation, true, true, true, 
			&numlocTransport);
			for (int i = 0; i < numlocTransport; i++) {
				if (visitTransport[travelConnect[i]] == UNKNOWN_PLACE) {
					visitTransport[travelConnect[i]] = v;
					QueueJoin(bfsQueue, travelConnect[i]);
				}
			}
		}
	}
	dropQueue(bfsQueue);
	
	// i index is destination
	// i.e. lhs index vals is where dest is
	if (foundpathTrain == true) {

		PlaceId pathLink;
		int numCities = 0;
		transportPath[numCities] = dest;
		bool reachedEnd = false;
		numCities++;

		while (reachedEnd == false) {
			pathLink = visitTransport[dest];
			if (pathLink == src) {
				reachedEnd = true;
			} else {
				transportPath[numCities] = pathLink;
				dest = pathLink;
				numCities++;
			}
		}
		
		int j = 0;
		for (int i = numCities - 1; i >= 0; i--, j++) {
			finalPath[j] = transportPath[i];
			printf("%s\n", placeIdToName(finalPath[j]));
		}

		*pathLength = j;
		return finalPath;
	}

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
