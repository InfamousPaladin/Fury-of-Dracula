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
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
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

static PlaceId *HvGetShortestPathMode(HunterView hv, PlaceId *visitTransport, 
									  PlaceId dest, Player hunter, bool road, 
									  bool rail, bool boat, bool *foundPath,
									  PlaceId src) {
	
	*foundPath = false;
	int roundoffSet = -1;
	int numlocTransport = 0;
	printf("\033[1;32m");
	printf("================= START OF ALGORITHM =================\n");
	printf("\033[0m");
	// -1 to indicate not visited
	for (int i = 0; i < NUM_REAL_PLACES; i++) visitTransport[i] = UNKNOWN_PLACE;

	// bfs path find by train
	visitTransport[src] = src;
	Queue bfsQueue = newQueue();
	QueueJoin(bfsQueue, src);
	while (*foundPath == false && QueueIsEmpty(bfsQueue) != 1) {
		PlaceId v = QueueLeave(bfsQueue);
		if (v == dest) {
			*foundPath = true;
		} else {
			printf("==================== FROM %s ====================\n", placeIdToName(v));
			// printf("%s %d\n", placeIdToName(v), HvGetRound(hv) + roundoffSet);
			if (visitTransport[v] != UNKNOWN_PLACE) roundoffSet++;
			PlaceId *travelConnect = GvGetReachableByType(hv->view, hunter, 
			(HvGetRound(hv) + roundoffSet), v, road, rail, boat, &numlocTransport);
			for (int i = 0; i < numlocTransport; i++) {
				if (visitTransport[travelConnect[i]] == UNKNOWN_PLACE) {
					printf("%s %d\n", placeIdToName(travelConnect[i]), HvGetRound(hv) + roundoffSet);
					visitTransport[travelConnect[i]] = v;
					QueueJoin(bfsQueue, travelConnect[i]);
				}
			}
		}
		// roundoffSet++;
	}
	dropQueue(bfsQueue);

	return visitTransport;
}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
	// arrays
	// needs to be dynamically allocated
	PlaceId src = HvGetPlayerLocation(hv, hunter);
	PlaceId *finalPath = malloc(sizeof(ConnList) * NUM_REAL_PLACES);
	
	// path created for all three modes of travel
	PlaceId *transportPath = malloc(sizeof(ConnList) * NUM_REAL_PLACES);
	PlaceId *visitTransport1 = malloc(sizeof(ConnList) * NUM_REAL_PLACES);
	PlaceId *visitTransport2 = malloc(sizeof(ConnList) * NUM_REAL_PLACES);

	int pathLength1 = 0;
	int pathLength2 = 0;

	PlaceId pathLink;
	PlaceId dest1 = dest;
	PlaceId dest2 = dest;
	bool foundPath = false;

	bool road = false;
	bool rail = true;
	bool boat  = true;

	visitTransport1 = HvGetShortestPathMode(hv, visitTransport1, dest, hunter, 
	road, rail, boat, &foundPath, src); 

	int reachedEnd = false;
	while (reachedEnd == false) {
		pathLink = visitTransport1[dest1];
		if (pathLink == src) {
			reachedEnd = true;
		} else {
			dest1 = pathLink;
			pathLength1++;
		}
	}

	road = true;
	rail = true;
	boat  = true;

	visitTransport2 = HvGetShortestPathMode(hv, visitTransport2, dest, hunter, 
	road, rail, boat, &foundPath, src); 

 	reachedEnd = false;
	while (reachedEnd == false) {
		pathLink = visitTransport2[dest2];
		if (pathLink == src) {
			reachedEnd = true;
		} else {
			dest2 = pathLink;
			pathLength2++;
		}
	}

	printf("P1: %d\n", pathLength1);
	printf("P2: %d\n", pathLength2);
	
	// i index is destination
	// i.e. lhs index vals is where dest is
	if (foundPath == true && pathLength1 < pathLength2) {

		PlaceId pathLink;
		int numCities = 0;
		transportPath[numCities] = dest;
		bool reachedEnd = false;
		numCities++;

		while (reachedEnd == false) {
			pathLink = visitTransport1[dest];
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

	} else if (foundPath == true && pathLength1 >= pathLength2) {

		PlaceId pathLink;
		int numCities = 0;
		transportPath[numCities] = dest;
		bool reachedEnd = false;
		numCities++;

		while (reachedEnd == false) {
			pathLink = visitTransport2[dest];
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
