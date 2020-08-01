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

void decideHunterMove(HunterView hv)
{
	Round roundNum = HvGetRound(hv);
	Player currPlayer = HvGetPlayer(hv);

	if (currPlayer == PLAYER_LORD_GODALMING && roundNum == 0) {
		registerBestPlay("SW", "LETS GO BOYS");
	} else if (currPlayer == PLAYER_DR_SEWARD && roundNum == 0) {
		registerBestPlay("LS", "LETS GO BOYS");
	} else if (currPlayer == PLAYER_VAN_HELSING && roundNum == 0) {
		registerBestPlay("MR", "LETS GO BOYS");
	} else if (currPlayer == PLAYER_MINA_HARKER && roundNum == 0) {
		registerBestPlay("HA", "LETS GO BOYS");
	}

	PlaceId dracLocation = HvGetPlayerLocation(hv, PLAYER_DRACULA);

	// immediately reveal Dracula's location
	if (dracLocation == NOWHERE && roundNum == 1) {
		
		if (currPlayer == PLAYER_LORD_GODALMING) {
			registerBestPlay("SW", "LETS GO BOYS");
		} else if (currPlayer == PLAYER_DR_SEWARD) {
			registerBestPlay("LS", "LETS GO BOYS");
		} else if (currPlayer == PLAYER_VAN_HELSING) {
			registerBestPlay("MR", "LETS GO BOYS");
		} else if (currPlayer == PLAYER_MINA_HARKER) {
			registerBestPlay("HA", "LETS GO BOYS");
		}

	} else if (dracLocation == CITY_UNKNOWN) {

		int numLocations = -1;
		PlaceId *possibleLocations = HvWhereCanIGoByType(hv, true, true, false, 
																&numLocations);
		int locID = (rand() % (numLocations - 1)) - 1;

		Place placeAbbrev;
		placeAbbrev.abbrev = (char *) placeIdToAbbrev(possibleLocations[locID]);

		registerBestPlay(placeAbbrev.abbrev, "LETS GO BOYS");

	} else if (dracLocation == SEA_UNKNOWN) {

		int numLocations = -1;
		PlaceId *possibleLocations = HvWhereCanIGoByType(hv, true, true, true, 
																&numLocations);
		int locID = (rand() % (numLocations - 1)) - 1;

		Place placeAbbrev;
		placeAbbrev.abbrev = (char *) placeIdToAbbrev(possibleLocations[locID]);

		registerBestPlay(placeAbbrev.abbrev, "LETS GO BOYS");

	}  


	// if vampire's location is visible, set a course to the location and kill it

	// finds Dracula's location and finds the shortest path to that location
	if (dracLocation < CITY_UNKNOWN && dracLocation != NOWHERE &&
		dracLocation != UNKNOWN_PLACE) {

		int pathLength = -1;
		PlaceId *pathtoDracula = HvGetShortestPathTo(hv, currPlayer, dracLocation, &pathLength);

		Place placeAbbrev;
		placeAbbrev.abbrev = (char *) placeIdToAbbrev(pathtoDracula[0]);
		registerBestPlay(placeAbbrev.abbrev, "LETS GO BOYS");
	}

}
