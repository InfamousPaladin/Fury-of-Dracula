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
			registerBestPlay("SW", "LETS FUCKIN GO BOYS ESHAYS");
		} else if (currPlayer == PLAYER_DR_SEWARD) {
			registerBestPlay("LS", "LETS FUCKIN GO BOYS ESHAYS");
		} else if (currPlayer == PLAYER_VAN_HELSING) {
			registerBestPlay("MR", "LETS FUCKIN GO BOYS ESHAYS");
		} else if (currPlayer == PLAYER_MINA_HARKER) {
			registerBestPlay("HA", "LETS FUCKIN GO BOYS ESHAYS");
		}
	}

	// finds Dracula's location and finds the shortest path to that location
	int pathLength = -1;
	PlaceId *pathtoDracula = HvGetShortestPathTo(hv, currPlayer, dracLocation, &pathLength);

	Place placeAbbrev;
	placeAbbrev.abbrev = (char *) placeIdToAbbrev(pathtoDracula[0]);
	registerBestPlay(placeAbbrev.abbrev, "LETS FUCKIN GO BOYS ESHAYS");



}
