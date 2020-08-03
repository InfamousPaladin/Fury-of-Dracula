////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// hunter.c: your "Fury of Dracula" hunter AI.
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
 
#include "Game.h"
#include "hunter.h"
#include "HunterView.h"

// storing visited locations
typedef struct shortestPath {
	PlaceId *storeShortestPath; // stores the shortest path
} shortestPath;

typedef struct playerInfo {
	shortestPath playerID[NUM_PLAYERS];	// array that contains each player info
} playerInfo;

// helper functions
static void randomMove(HunterView hv, Player currPlayer);
static void headtoCastleDracula(HunterView hv, Player currPlayer);

void decideHunterMove(HunterView hv) {

	// obtaining round info
	Round roundNum = HvGetRound(hv);
	Player currPlayer = HvGetPlayer(hv);

	// starting positions
	if (currPlayer == PLAYER_LORD_GODALMING && roundNum == 0) {
		registerBestPlay("SW", "LETS GO HUNTERS");
	} else if (currPlayer == PLAYER_DR_SEWARD && roundNum == 0) {
		registerBestPlay("LS", "LETS GO HUNTERS");
	} else if (currPlayer == PLAYER_VAN_HELSING && roundNum == 0) {
		registerBestPlay("MR", "LETS GO HUNTERS");
	} else if (currPlayer == PLAYER_MINA_HARKER && roundNum == 0) {
		registerBestPlay("HA", "LETS GO HUNTERS");
	}

	// getting Drac's location
	PlaceId dracLocation = HvGetPlayerLocation(hv, PLAYER_DRACULA);

	// handle cases where Dracula's location is unknown
	// immediately reveal Dracula's 6th location in trail
	if ((dracLocation == NOWHERE || dracLocation == CITY_UNKNOWN ||
		 dracLocation == SEA_UNKNOWN) && roundNum == 6) {
		
		if (currPlayer == PLAYER_LORD_GODALMING) {

			char *placeAbbrev = (char *) placeIdToAbbrev(HvGetPlayerLocation(hv, 
														PLAYER_LORD_GODALMING));
			registerBestPlay(placeAbbrev, "Godalming on the move");

		} else if (currPlayer == PLAYER_DR_SEWARD) {

			char *placeAbbrev = (char *) placeIdToAbbrev(HvGetPlayerLocation(hv, 
														PLAYER_DR_SEWARD));
			registerBestPlay(placeAbbrev, "Seward on the move");

		} else if (currPlayer == PLAYER_VAN_HELSING) {

			char *placeAbbrev = (char *) placeIdToAbbrev(HvGetPlayerLocation(hv, 
														PLAYER_VAN_HELSING));
			registerBestPlay(placeAbbrev, "Helsing on the move");

		} else if (currPlayer == PLAYER_MINA_HARKER) {

			char *placeAbbrev = (char *) placeIdToAbbrev(HvGetPlayerLocation(hv, 
														PLAYER_MINA_HARKER));
			registerBestPlay(placeAbbrev, "Harker on the move");

		}
		
		return;
	} 

	// finds Vamp's location and finds the shortest path to kill it
	if (HvGetVampireLocation(hv) < CITY_UNKNOWN && HvGetVampireLocation(hv) != 
		NOWHERE && currPlayer == PLAYER_LORD_GODALMING) {

		int pathLength = -1;
		PlaceId *pathtoVampire = HvGetShortestPathTo(hv, currPlayer, 
													 HvGetVampireLocation(hv), 
													 &pathLength);

		char *placeAbbrev = (char *) placeIdToAbbrev(pathtoVampire[0]);
		registerBestPlay(placeAbbrev, "Found a vampire, I'm gonna kill it");
		return;
	}

	int lastDracRound = -1;
	PlaceId lastDracLoc = HvGetLastKnownDraculaLocation(hv, &lastDracRound);

	// if Hunter is on 2 health or less, make them rest unless they are dead
	if (HvGetHealth(hv, currPlayer) < 3 && 
		(HvGetPlayerLocation(hv, currPlayer) != HOSPITAL_PLACE)) {

		char *placeAbbrev = (char *) placeIdToAbbrev(HvGetPlayerLocation(hv, 
																currPlayer));
		registerBestPlay(placeAbbrev, "Resting for health");

		return;
	}

	// if Drac's location has been found, find all possible locations
	// he can go to and send a hunter to each of those locations and invoke
	// the random move function from there to find Dracula
	if ((lastDracLoc < CITY_UNKNOWN) && (lastDracLoc != NOWHERE) &&
		(lastDracLoc != UNKNOWN_PLACE)) {
		
		int indexLoc = 0;
		PlaceId *toDracPath;
		int numDracLocations = -1;

		// if Drac's location has been revealed recently, chase him
		// else given Drac's last known location, find possible places to chase
		if ((HvGetRound(hv) - lastDracRound) < 2) {
			toDracPath = HvGetShortestPathTo(hv, currPlayer, lastDracLoc, 
															&numDracLocations);
		} else {
			toDracPath = HvWhereCanTheyGo(hv, PLAYER_DRACULA, 
															&numDracLocations);
			srand(numDracLocations);
			indexLoc = (rand() % numDracLocations) - 1;

		}

		int pathLength = -1;
		PlaceId *pathtoDracula = HvGetShortestPathTo(hv, currPlayer, 
											toDracPath[indexLoc], &pathLength);

		// location reached; head to CD
		// Dracula might be heading there aswell
		if (pathtoDracula[0] == HvGetPlayerLocation(hv, currPlayer)) {
			headtoCastleDracula(hv, currPlayer);
			return;
		}

		char *placeAbbrev = (char *) placeIdToAbbrev(pathtoDracula[0]);
		registerBestPlay(placeAbbrev, "We're coming after you");
		return;
	}

	// in case Drac's location is still hidden, head to CD 
	// then make random moves from there to find Dracula
	randomMove(hv, currPlayer);
	return;
}

// if Drac's location is unknown, use random moves to find him
static void headtoCastleDracula(HunterView hv, Player currPlayer) {

	// if Drac's in a city, chances are he's heading towards CD or near
	int pathLength = -1;
	PlaceId *pathtoCD = HvGetShortestPathTo(hv, currPlayer, CASTLE_DRACULA, 
											&pathLength);

	// location reached, do random moves until Drac is found
	if (pathtoCD[0] == HvGetPlayerLocation(hv, currPlayer)) {
		randomMove(hv, currPlayer);
		return;
	}

	char *placeAbbrev = (char *) placeIdToAbbrev(pathtoCD[0]);
	registerBestPlay(placeAbbrev, "Heading to the Devil's den");

	return;
}

// random moves until Dracula is found
// do not revist the locations already visited
static void randomMove(HunterView hv, Player currPlayer) {

	// Get the previous location of the hunter
	PlaceId prevLocation = HvGetPlayerLocation(hv, currPlayer);

	int numLocations = -1;
	PlaceId *possibleLocations = HvWhereCanIGo(hv, &numLocations);

	srand(numLocations);
	int randLocID = (rand() % numLocations) - 1;

	// Loop to make sure the next location will not be the prev location
	while (possibleLocations[randLocID] == prevLocation) {
		randLocID = (rand() % numLocations) - 1;
	}

	char *placeAbbrev  = (char *) placeIdToAbbrev(possibleLocations[randLocID]);
	registerBestPlay(placeAbbrev, "Searching for Dracula");

	// if (lastDracLoc == SEA_UNKNOWN) {

	// 	PlaceId *possibleLocations = HvWhereCanIGoByType(hv, true, true, true, 
	// 															&numLocations);
	// 	int locID = (rand() % (numLocations - 1)) - 1;

	// 	char *placeAbbrev  = (char *) placeIdToAbbrev(possibleLocations[locID]);
	// 	registerBestPlay(placeAbbrev, "Searching cities and oceans");

	// }
	return;
}
