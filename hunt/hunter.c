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
#include <time.h> 
 
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
// static void scatterandSearch(HunterView hv, Player currPlayer);
static void headtoKlausenberg(HunterView hv, Player currPlayer);
// static void headtoLocation(HunterView hv, Player currPlayer, PlaceId dest);

void decideHunterMove(HunterView hv) {	

	time_t t;
	srand((unsigned) time(&t));

	// obtaining round info
	Round roundNum = HvGetRound(hv);
	Player currPlayer = HvGetPlayer(hv);
	PlaceId currLoc = HvGetPlayerLocation(hv, currPlayer);
	int currHealth = HvGetHealth(hv, currPlayer);
	// getting Drac's location
	PlaceId dracLocation = HvGetPlayerLocation(hv, PLAYER_DRACULA);
	// last known drac location
	int lastDracRound = -1;
	PlaceId lastDracLoc = HvGetLastKnownDraculaLocation(hv, &lastDracRound);
	PlaceId vampLoc = HvGetVampireLocation(hv);

	// starting positions
	if (currPlayer == PLAYER_LORD_GODALMING && roundNum == 0) {
		registerBestPlay("VE", "LG - Forward");
		return;
	} else if (currPlayer == PLAYER_DR_SEWARD && roundNum == 0) {
		registerBestPlay("TO", "DS - Forward");
		return;
	} else if (currPlayer == PLAYER_VAN_HELSING && roundNum == 0) {
		registerBestPlay("MN", "VH - Forward");
		return;
	} else if (currPlayer == PLAYER_MINA_HARKER && roundNum == 0) {
		registerBestPlay("SO", "MH - Forward");
		return;
	}

	// handle cases where Dracula's location is unknown and immediately 
	// reveal Dracula's 6th location in trail
	if (!placeIsReal(dracLocation) && roundNum % 7 == 0) {		
		char *placeAbbrev = (char *) placeIdToAbbrev(currLoc);
		registerBestPlay(placeAbbrev, "Resting");
		return;
	} 

	// First consider hunter's health
	// if Hunter is on 2 health or less, make them rest unless they are dead
	if (currHealth < 3 && currLoc != HOSPITAL_PLACE) {
		char *placeAbbrev = (char *) placeIdToAbbrev(currLoc);
		registerBestPlay(placeAbbrev, "Resting for health");
		return;
	}

	// Find immature vampire if it was revealed
	if (placeIsReal(vampLoc)) {
		int distToVamp[4];
		for (int i = 0; i < 4; i++) {
			int pathLength = -1;
			PlaceId *pathtoVampire = HvGetShortestPathTo(hv, currPlayer, 
										HvGetVampireLocation(hv), &pathLength);
			distToVamp[i] = pathLength;
			free(pathtoVampire);
		}

		int minPath = distToVamp[0];
		int recPlayer = -1;
		for (int i = 0; i < 4; i++) {
			if (minPath > distToVamp[i]) {
				minPath = distToVamp[i];
				recPlayer = i;
			}
		}

		if (currPlayer == recPlayer) {
			int pathLength = -1;
			PlaceId *pathtoVampire = HvGetShortestPathTo(hv, currPlayer, 
										vampLoc, &pathLength);
			char *placeAbbrev = (char *) placeIdToAbbrev(pathtoVampire[0]);
			registerBestPlay(placeAbbrev, "Found a vampire, I'm gonna kill it");
			free(pathtoVampire);
			return;
		}

	}

	// if Drac's location has been found, find all possible locations
	// he can go to and send a hunter to each of those locations and invoke
	// the random move function from there to find Dracula
	if (placeIsReal(lastDracLoc) && HvGetRound(hv) - lastDracRound < 2) {
		int pathLength = -1;
		PlaceId *searchPath = HvGetShortestPathTo(hv, currPlayer, 
								lastDracLoc, &pathLength);
		char *placeAbbrev = (char *) placeIdToAbbrev(searchPath[0]);
		registerBestPlay(placeAbbrev, "We're coming after you");
		return;
	}

	if (currLoc == lastDracLoc) {
		int nLocs = 0;
		PlaceId *locs = HvWhereCanTheyGoByType(hv, PLAYER_DRACULA, true, false, true, &nLocs);
		int i = rand() % nLocs;
		char *placeAbbrev = (char *) placeIdToAbbrev(locs[i]);
		registerBestPlay(placeAbbrev, "We're coming after you");
		return;
	}

	// godalming patrolling Castle Dracula
	if (currPlayer == PLAYER_LORD_GODALMING) {
		PlaceId godalmingLoc = HvGetPlayerLocation(hv, 
		PLAYER_LORD_GODALMING);

		if (godalmingLoc != KLAUSENBURG) {
			headtoKlausenberg(hv, PLAYER_LORD_GODALMING);
			return;
		}
		if (godalmingLoc == KLAUSENBURG) {
			registerBestPlay("GA", "We're coming after you");
		}

		// time_t t;
		// int patrolLoc = -1;
		// srand((unsigned) time(&t));

		// // 6 because there are 6 locations connected to that place
		// int randIndex = rand() % 6;
		// PlaceId *patrol = HvWhereCanIGo(hv, &patrolLoc);

		// char *placeAbbrev = (char *) placeIdToAbbrev(patrol[randIndex]);
		// registerBestPlay(placeAbbrev, "Patrolling Klausenberg's net");
		return;
	}

	// in case Drac's location is still hidden, head to CD 
	// then make random moves from there to find Dracula
	randomMove(hv, currPlayer);
	return;
}

// head to CD
static void headtoKlausenberg(HunterView hv, Player currPlayer) {

	int pathLength = -1;
	PlaceId *pathtoKL = HvGetShortestPathTo(hv, currPlayer, KLAUSENBURG, &pathLength);

	char *placeAbbrev = (char *) placeIdToAbbrev(pathtoKL[0]);
	registerBestPlay(placeAbbrev, "Heading to Klausenberg");

	return;
}
/*
// head to desired location
static void headtoLocation(HunterView hv, Player currPlayer, PlaceId dest) {

	int pathLength = -1;
	PlaceId *pathtoDest = HvGetShortestPathTo(hv, currPlayer, dest, &pathLength);

	char *placeAbbrev = (char *) placeIdToAbbrev(pathtoDest[0]);
	registerBestPlay(placeAbbrev, "Heading to location");
	return;
}
*/

// random moves until Dracula is found
// do not revist the locations already visited
static void randomMove(HunterView hv, Player currPlayer) {

	bool canFree = false;
	int numLocations = -1;
	bool locFound = false;
	int actualLocations = -1;

	Player playerA;
	Player playerB;
	Player playerC;

	if (currPlayer == PLAYER_LORD_GODALMING) {

		playerA = PLAYER_DR_SEWARD;
		playerB = PLAYER_VAN_HELSING;
		playerC = PLAYER_MINA_HARKER;

	} else if (currPlayer == PLAYER_DR_SEWARD) {

		playerA = PLAYER_LORD_GODALMING;
		playerB = PLAYER_VAN_HELSING;
		playerC = PLAYER_MINA_HARKER;

	} else if (currPlayer == PLAYER_VAN_HELSING) {

		playerA = PLAYER_DR_SEWARD;
		playerB = PLAYER_LORD_GODALMING;
		playerC = PLAYER_MINA_HARKER;

	} else if (currPlayer == PLAYER_MINA_HARKER) {

		playerA = PLAYER_DR_SEWARD;
		playerB = PLAYER_VAN_HELSING;
		playerC = PLAYER_LORD_GODALMING;
	}

	PlaceId *currPlayerPastMov = HvGetLastMoves(hv, currPlayer, 6, 
									&actualLocations, &canFree);
	PlaceId *playerAPastMov = HvGetLastMoves(hv, playerA, 6, 
									&actualLocations, &canFree);
	PlaceId *playerBPastMov = HvGetLastMoves(hv, playerB, 6, 
									&actualLocations, &canFree);
	PlaceId *playerCPastMov = HvGetLastMoves(hv, playerC, 6, 
									&actualLocations, &canFree);

	PlaceId *possibleLocations = HvWhereCanIGo(hv, &numLocations);

	/*
	for (int i = 0; i < actualLocations; i++) {
		printf("Past Moves: %s\n", placeIdToName(playerPastMoves[i]));
	}
	printf("\n");

	for (int i = 0; i < numLocations; i++) {
		printf("Where I can go: %s\n", placeIdToName(possibleLocations[i]));
	}
	printf("\n");
	*/

	int nGoodLocs = 0;
	int goodLocs[NUM_REAL_PLACES];

	for (int i = 0; i < numLocations; i++) {

		int j;
		PlaceId newLoc = possibleLocations[i];

		for (j = 0; j < actualLocations; j++) {
			if (newLoc == currPlayerPastMov[j] || newLoc == playerAPastMov[j]  || newLoc == playerBPastMov[j]  || newLoc == playerCPastMov[j]) {
				locFound = true;
			}
		}

		if (locFound == false) {
			goodLocs[nGoodLocs] = newLoc;
			nGoodLocs++;
		}
	}
	if (nGoodLocs == 0) {
		char *placeAbbrev  = (char *) placeIdToAbbrev(possibleLocations[0]);
		registerBestPlay(placeAbbrev, "Searching for Dracula");
	} else {
		int i = rand() % nGoodLocs;
		char *placeAbbrev  = (char *) placeIdToAbbrev(goodLocs[i]);
		registerBestPlay(placeAbbrev, "Searching for Dracula Edition");
	}
	return;
}
