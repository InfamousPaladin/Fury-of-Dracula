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
	// This is the move command
	char *move;

	// Starting positions
	if (currPlayer == PLAYER_LORD_GODALMING && roundNum == 0) {
		registerBestPlay("SW", "LETS FUCKIN GO BOYS ESHAYS");
	} else if (currPlayer == PLAYER_DR_SEWARD && roundNum == 0) {
		registerBestPlay("LS", "LETS FUCKIN GO BOYS ESHAYS");
	} else if (currPlayer == PLAYER_VAN_HELSING && roundNum == 0) {
		registerBestPlay("MR", "LETS FUCKIN GO BOYS ESHAYS");
	} else if (currPlayer == PLAYER_MINA_HARKER && roundNum == 0) {
		registerBestPlay("HA", "LETS FUCKIN GO BOYS ESHAYS");
	}

	PlaceId dracLocation = HvGetPlayerLocation(hv, PLAYER_DRACULA);

	// immediately reveal Dracula's location
	if (dracLocation == NOWHERE && roundNum == 1) {
		
		if (currPlayer == PLAYER_LORD_GODALMING) {
			move = (char *) placeIdToAbbrev(HvGetPlayerLocation(hv, PLAYER_LORD_GODALMING));
			registerBestPlay(move, "LETS FUCKIN GO BOYS ESHAYS");
		} else if (currPlayer == PLAYER_DR_SEWARD) {
			move = (char *) placeIdToAbbrev(HvGetPlayerLocation(hv, PLAYER_DR_SEWARD));
			registerBestPlay(move, "LETS FUCKIN GO BOYS ESHAYS");
		} else if (currPlayer == PLAYER_VAN_HELSING) {
			move = (char *) placeIdToAbbrev(HvGetPlayerLocation(hv, PLAYER_VAN_HELSING));
			registerBestPlay(move, "LETS FUCKIN GO BOYS ESHAYS");
		} else if (currPlayer == PLAYER_MINA_HARKER) {
			move = (char *) placeIdToAbbrev(HvGetPlayerLocation(hv, PLAYER_MINA_HARKER));
			registerBestPlay(move, "LETS FUCKIN GO BOYS ESHAYS");
		}
	// Do random moves until a hunter finds Dracula
	} else if (dracLocation == CITY_UNKNOWN) {

		int numLocations = -1;
		PlaceId *possibleLocations = HvWhereCanIGoByType(hv, true, true, false, 
																&numLocations);
		int locID = (rand() % (numLocations - 1)) - 1;

		move = (char *) placeIdToAbbrev(possibleLocations[locID]);

		registerBestPlay(move, "LETS GO BOYS");

	} else if (dracLocation == SEA_UNKNOWN) {

		int numLocations = -1;
		PlaceId *possibleLocations = HvWhereCanIGoByType(hv, true, true, true, 
																&numLocations);
		int locID = (rand() % (numLocations - 1)) - 1;

		move = (char *) placeIdToAbbrev(possibleLocations[locID]);

		registerBestPlay(move, "LETS GO BOYS");
	// if vampire's location is visible, set a course to the location and kill it
	} else if (HvGetVampireLocation(hv) != NOWHERE && HvGetVampireLocation(hv) != CITY_UNKNOWN) {
		int pathLength = -1;
		PlaceId *vampPath = HvGetShortestPathTo(hv, currPlayer, HvGetVampireLocation(hv), &pathLength);
		move = (char *) placeIdToAbbrev(vampPath[0]);
		registerBestPlay(move, "*STAB *STAB");
	}
	// finds Dracula's location and finds the shortest path to that location
	if (dracLocation < CITY_UNKNOWN && dracLocation != NOWHERE &&
		dracLocation != UNKNOWN_PLACE) {
		// finds Dracula's location and finds the shortest path to that location
		dracLocation = HvGetLastKnownDraculaLocation(hv, &roundNum);
		int pathLength = -1;
		PlaceId *pathtoDracula = HvGetShortestPathTo(hv, currPlayer, dracLocation, &pathLength);
		move = (char *) placeIdToAbbrev(pathtoDracula[0]);
		registerBestPlay(move, "The hunt begins");
	}
}
