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

// Written in the month of July 2020 for COMP2521 T2 Assignment 2.

// This is the implementation file for the HunterView.h ADT. Implementation of 
// the following code was completed by Tam Do and Prathamesh Jagtap.

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
#define TURN_CHARS	     8	// chars each turn takes in play string (w space)
#define ROUND_CHARS	     40 // chars each round takes in play string (w space)
#define POS_ACTIONS      6 	// player actions; 2 for location; 4 for rest
#define PLRACT_STRING    7 	// each player string length
#define	START_RAIL_DIST	 1
#define UNINTIALISED	-1	// path not found yet

// each player is given player information
// this information is mapped to playerInfo
typedef struct shortestPath {
	PlaceId *storeShortestPath; // stores the shortest path
} shortestPath;

struct hunterView {
	Map map; // map of the board
	GameView view; // GameView struct skeleton
	char *playString; // playString
	shortestPath playerID[NUM_PLAYERS];	// array that contains each player info
};

// Prototypes for helper functions.
static PlaceId HvDraculaDoubleBack(
	HunterView hv, 
	Place playerLoc, 
	int roundBack);
static void HvGetShortestPath(
	HunterView hv, 
	PlaceId visitTransport[], 
	PlaceId dest, 
	Player hunter, 
	bool *foundPath, 
	PlaceId src);

//**************************************************************************//
//                		   Constructor/Deconstructor 		 	            //
//**************************************************************************//

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
	new->playString = pastPlays;
	new->view = GvNew(pastPlays, messages);

	return new;
}

void HvFree(HunterView hv)
{
	free(hv->view);
	free(hv);
}

//**************************************************************************//
//                		      Game State Information 		 	            //
//**************************************************************************//

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

PlaceId HvGetPlayerLocation(HunterView hv, Player player)
{
	return GvGetPlayerLocation(hv->view, player);
}

PlaceId HvGetVampireLocation(HunterView hv)
{
	return GvGetVampireLocation(hv->view);
}

//**************************************************************************//
//                		       Utility Functions  		 	                //
//**************************************************************************//

PlaceId HvGetLastKnownDraculaLocation(HunterView hv, Round *round)
{
	Place playerLoc;
	int hideOffset = 0;
	char playerPlace[3];
	playerLoc.id = CITY_UNKNOWN;
	int roundBack = HvGetRound(hv);
	playerLoc.abbrev = playerPlace;

	while (playerLoc.id > ZURICH || playerLoc.id == UNKNOWN_PLACE) {
		// can't find a location
		if (roundBack < 0) return NOWHERE;
		playerLoc.id = HvDraculaDoubleBack(hv, playerLoc, roundBack);
		if (playerLoc.id == HIDE) hideOffset++;
		roundBack--;
	}

	*round = roundBack + hideOffset;
	return playerLoc.id;
}

PlaceId *HvGetShortestPathTo(HunterView hv, Player hunter, PlaceId dest,
                             int *pathLength)
{
	// player's current location
	PlaceId src = HvGetPlayerLocation(hv, hunter);

	// needed arrays for conducting calculations and holding data
	PlaceId transportPath[NUM_REAL_PLACES];
	PlaceId visitTransport[NUM_REAL_PLACES];
	bool foundPath = false;
	HvGetShortestPath(hv, visitTransport, dest, hunter, &foundPath, src); 
	
	// path is found
	if (foundPath == true) {
		
		PlaceId pathLink;
		int numCities = 0;
		transportPath[numCities] = dest;
		bool reachedEnd = false;
		numCities++;

		// calculating the number of locations
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

		// dynamically allocating the final path of locations
		PlaceId *finalPath = malloc(sizeof(ConnList) * (numCities - 1));
		
		// reading the found path into a dynamically allocated array
		int j = 0;
		for (int i = numCities - 1; i >= 0; i--, j++) {
			finalPath[j] = transportPath[i];
		}

		// storing path distinct to player in struct
		hv->playerID[hunter].storeShortestPath = finalPath;

		if (src == dest) numCities--;

		*pathLength = numCities;
		return finalPath;
	}

	return NULL;
}

PlaceId *HvGetLastMoves(HunterView hv, Player player, int numMoves,
										int *numReturnedMoves, bool *canFree) {
		
	PlaceId *lastMoves = GvGetLastMoves(hv->view, player, numMoves, 
													numReturnedMoves, canFree);
	return lastMoves;
}

//**************************************************************************//
//                		    	 Making a Move  		 	                //
//**************************************************************************//

PlaceId *HvWhereCanIGo(HunterView hv, int *numReturnedLocs)
{
	int numLocations = 0;
	PlaceId *reachablePlaces;

	reachablePlaces = GvGetReachable(hv->view, HvGetPlayer(hv), HvGetRound(hv), 
					HvGetPlayerLocation(hv, HvGetPlayer(hv)), &numLocations);

	*numReturnedLocs = numLocations;
	return reachablePlaces;
}

PlaceId *HvWhereCanIGoByType(HunterView hv, bool road, bool rail,
                             bool boat, int *numReturnedLocs)
{
	int numLocations = 0;
	PlaceId *reachablebyType;

	reachablebyType = GvGetReachableByType(hv->view, HvGetPlayer(hv), 
						HvGetRound(hv),HvGetPlayerLocation(hv, HvGetPlayer(hv)), 
						road, rail, boat, &numLocations);

	*numReturnedLocs = numLocations;
	return reachablebyType;
}

PlaceId *HvWhereCanTheyGo(HunterView hv, Player player, int *numReturnedLocs)
{	
	int dracRound = 0;
	PlaceId playerLoc;
	int numLocations = 0;
	PlaceId *reachableNext;

	Round round = HvGetRound(hv);
	Player currPlayer = GvGetPlayer(hv->view);
	if (player < currPlayer) round++;

	if (player == PLAYER_DRACULA) {
		playerLoc = HvGetLastKnownDraculaLocation(hv, &dracRound);
	} else {
		playerLoc = HvGetPlayerLocation(hv, player);
	}

	reachableNext = GvGetReachable(hv->view, player, round, playerLoc, 
																&numLocations);

	*numReturnedLocs = numLocations;
	return reachableNext;
}

PlaceId *HvWhereCanTheyGoFromSrc(HunterView hv, Player player, PlaceId src, 
								int *numReturnedLocs)
{	
	int numLocations = 0;
	PlaceId *reachableNext;

	Round round = HvGetRound(hv);
	
	Player currPlayer = GvGetPlayer(hv->view);
	if (player < currPlayer) round++;

	reachableNext = GvGetReachable(hv->view, player, round, src, &numLocations);

	*numReturnedLocs = numLocations;
	return reachableNext;
}

PlaceId *HvWhereCanTheyGoByType(HunterView hv, Player player,
                                bool road, bool rail, bool boat,
                                int *numReturnedLocs)
{
	int numLocations = 0;
	PlaceId *reachableNextType;

	Round round = HvGetRound(hv);
	Player currPlayer = GvGetPlayer(hv->view);
	if (player < currPlayer) round++;
	reachableNextType = GvGetReachableByType(hv->view, player, 
							round, HvGetPlayerLocation(hv, player), 
							road, rail, boat, &numLocations);

	*numReturnedLocs = numLocations;
	return reachableNextType;
}

//**************************************************************************//
//                		    	Helper Functions 		 	                //
//**************************************************************************//

// Helper function for HvGetLastKnownDraculaLocation: Gets Dracula's location 
// given how many rounds before the function should look for.
static PlaceId HvDraculaDoubleBack(HunterView hv, Place playerLoc, 
								   int roundBack) {

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
	for (int i = 0; i <= (numMoves + PLRACT_STRING * NUM_PLAYERS * roundBack); 
																		i++) {

		if (hv->playString[i] == 'D' && hv->playString[i + 6] == '.') {

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


// Helper function for HvGetShortestPathTo: Uses a breadth first search to
// travers the graph and determine the shortest path from `src` to `dest`.
static void HvGetShortestPath(HunterView hv, PlaceId visitTransport[], 
								  PlaceId dest, Player hunter, bool *foundPath, 
								  PlaceId src) {
	
	PlaceId remLocRound[NUM_REAL_PLACES];
	
	*foundPath = false;
	int roundoffSet = 0;
	int numlocTransport = 0;

	// -1 to indicate not visited
	for (int i = 0; i < NUM_REAL_PLACES; i++) remLocRound[i] = UNINTIALISED;
	for (int i = 0; i < NUM_REAL_PLACES; i++) visitTransport[i] = UNINTIALISED;

	// bfs path find
	visitTransport[src] = src;
	Queue bfsQueue = newQueue();
	remLocRound[src] = HvGetRound(hv);

	// joining queue
	QueueJoin(bfsQueue, src);
	while (*foundPath == false && QueueIsEmpty(bfsQueue) != 1) {

		// new starting location is extracted
		PlaceId newLocation = QueueLeave(bfsQueue);

		if (newLocation == dest) {
			*foundPath = true;
		} else {
			// advancement of round relative to movement in location is
			// recorded in an array for GvGetReachable to calculate a path
			int roundNum = remLocRound[newLocation];
			PlaceId *travelConnect = GvGetReachable(hv->view, hunter, roundNum, 
			newLocation, &numlocTransport);

			// all locations are added to the queue
			for (int i = 0; i < numlocTransport; i++) {
				// incrementing the round counter as new location is scanned in
				// recording all data for calculating the shortest path
				if (visitTransport[travelConnect[i]] == UNINTIALISED) {
					remLocRound[travelConnect[i]] = roundNum + 1;
					visitTransport[travelConnect[i]] = newLocation;
					QueueJoin(bfsQueue, travelConnect[i]);
				}
			}
		}
		roundoffSet++;
	}
	dropQueue(bfsQueue);
}