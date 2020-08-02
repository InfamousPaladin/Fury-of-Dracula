////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// dracula.c: your "Fury of Dracula" Dracula AI
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include "dracula.h"
#include "DraculaView.h"
#include "Game.h"
#include <stdlib.h>
#include <time.h> 

#define DONT -1000

typedef struct hunter
{
	PlaceId currLoc;					// current location of player
	int health;							// current player health
	PlaceId *reachable;
	int nReach;
} hunterInfo;

typedef struct gameState {
	Round currRound;
	int score;

	hunterInfo hunterID[4];
	PlaceId *allHunterLocs;
	int totalLocs;

	PlaceId dracLoc;

	PlaceId *dracLocs;
	int nLocs;

	PlaceId *dracMoves;
	int nMoves;

	PlaceId dracHealth;

} GameState;


void decideDraculaMove(DraculaView dv)
{
	char *play = "CD";
	registerBestPlay("BI", "Default");
	GameState gameInfo;

	time_t t;
	srand((unsigned) time(&t));

	// Get players current location
	gameInfo.hunterID[0].currLoc = DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING);
	gameInfo.hunterID[1].currLoc = DvGetPlayerLocation(dv, PLAYER_DR_SEWARD);
	gameInfo.hunterID[2].currLoc = DvGetPlayerLocation(dv, PLAYER_VAN_HELSING);
	gameInfo.hunterID[3].currLoc = DvGetPlayerLocation(dv, PLAYER_MINA_HARKER);
	gameInfo.dracLoc = DvGetPlayerLocation(dv, PLAYER_DRACULA);

	// Get players next reachable location
	gameInfo.hunterID[0].reachable = DvWhereCanTheyGo(dv, PLAYER_LORD_GODALMING, &gameInfo.hunterID[0].nReach); 
	gameInfo.hunterID[1].reachable = DvWhereCanTheyGo(dv, PLAYER_DR_SEWARD, &gameInfo.hunterID[1].nReach); 
	gameInfo.hunterID[2].reachable = DvWhereCanTheyGo(dv, PLAYER_VAN_HELSING, &gameInfo.hunterID[2].nReach); 
	gameInfo.hunterID[3].reachable = DvWhereCanTheyGo(dv, PLAYER_MINA_HARKER, &gameInfo.hunterID[3].nReach);
	gameInfo.dracLocs = DvWhereCanIGo(dv, &gameInfo.nLocs);
	gameInfo.dracMoves = DvGetValidMoves(dv, &gameInfo.nMoves);

	// Get players health
	gameInfo.hunterID[0].health = DvGetHealth(dv, PLAYER_LORD_GODALMING);
	gameInfo.hunterID[1].health = DvGetHealth(dv, PLAYER_DR_SEWARD);
	gameInfo.hunterID[2].health = DvGetHealth(dv, PLAYER_VAN_HELSING);
	gameInfo.hunterID[3].health = DvGetHealth(dv, PLAYER_MINA_HARKER);
	gameInfo.dracHealth = DvGetHealth(dv, PLAYER_DRACULA);

	gameInfo.currRound = DvGetRound(dv);
	gameInfo.score = DvGetScore(dv);

	// avoiding moving to cities where the hunters can also move
	gameInfo.totalLocs = 0;
	for (int i = PLAYER_LORD_GODALMING; i < PLAYER_DRACULA; i++) {
		gameInfo.totalLocs += gameInfo.hunterID[i].nReach;
	}

	// appending all individual hunters possible city moves into one array
	// TODO: Consider if hunters can reach the same city (uniqueness)
	gameInfo.allHunterLocs = malloc(sizeof(PlaceId) * gameInfo.totalLocs);
	int i;
	for (i = 0; i < gameInfo.hunterID[0].nReach; i++) {
		gameInfo.allHunterLocs[i] = gameInfo.hunterID[0].reachable[i];
	}
	for (int j = 0; j < gameInfo.hunterID[1].nReach; j++, i++) {
		gameInfo.allHunterLocs[i] = gameInfo.hunterID[1].reachable[j]; 
	}
	for (int j = 0; j < gameInfo.hunterID[2].nReach; j++, i++) {
		gameInfo.allHunterLocs[i] = gameInfo.hunterID[2].reachable[j];
	}
	for (int j = 0; j < gameInfo.hunterID[3].nReach; j++, i++) {
		gameInfo.allHunterLocs[i] = gameInfo.hunterID[3].reachable[j];
	}

	// Round 0
	if (gameInfo.currRound == 0) {
		// Start near the coast if possible
		bool englandCoast = false;

		for (int i = 0; i < gameInfo.totalLocs; i++) {
			if (gameInfo.allHunterLocs[i] == GALWAY ||
				gameInfo.allHunterLocs[i] == DUBLIN) {
				englandCoast = true;
				break;
			}
		}

		bool westCoast = false;
		for (int i = 0; i < gameInfo.totalLocs; i++) {
			if (gameInfo.allHunterLocs[i] == CONSTANTA ||
				gameInfo.allHunterLocs[i] == VARNA) {
				westCoast = true;
				break;
			}
		}

		bool southCoast = false;
		for (int i = 0; i < gameInfo.totalLocs; i++) {
			if (gameInfo.allHunterLocs[i] == ROME ||
				gameInfo.allHunterLocs[i] == NAPLES ||
				gameInfo.allHunterLocs[i] == GENOA ||
				gameInfo.allHunterLocs[i] == CAGLIARI ||
				gameInfo.allHunterLocs[i] == TYRRHENIAN_SEA ||
				gameInfo.allHunterLocs[i] == IONIAN_SEA ||
				gameInfo.allHunterLocs[i] == MEDITERRANEAN_SEA) {
				southCoast = true;
				break;
			}
		}

		if (!englandCoast) {
			registerBestPlay("GW", "comp1511 here i come");
		} else if (!westCoast) {
			registerBestPlay("VR", "be a hunter");
		} else if(!southCoast) {
			registerBestPlay("TS", "infinity");
		} else {
			bool goodMove = false;
			PlaceId locID;
			while (!goodMove) {
				locID = rand() % NUM_REAL_PLACES;
				goodMove = true;
				for (int i = 0; i < gameInfo.totalLocs; i++) {
					if (gameInfo.allHunterLocs[i] == locID) {
						goodMove = false;
						break;
					}
				}
			}
			play = (char *) placeIdToAbbrev(locID);
			registerBestPlay(play, "being a complete idiot here");
		}
		free(gameInfo.allHunterLocs);
	} else if (gameInfo.nMoves == 0) {
		// Case where no more valid moves
		registerBestPlay("TP", "Gotcha Fool");
		free(gameInfo.allHunterLocs);
	} else {
		// TODO: Just in case, just register a random move.
		bool notGoodMove = true;
		while (notGoodMove) {
			PlaceId locID = rand() % gameInfo.nMoves;
			play = (char *) placeIdToAbbrev(locID);
			registerBestPlay(play, "BYE BYE BUDDY!!!!");
			for (int i = 0; i < gameInfo.totalLocs; i++) {
				if (gameInfo.allHunterLocs[i] == locID) {
					notGoodMove = false;
					break;
				}
			}
		}


		{

		// TODO: need to account for both moves and locations. This is currently
		// only considering moves. TODO: Just locs
		int numBadLocs = 0;
		// comparing draculas locations to locations possible by all hunters
		for (int i = 0; i < gameInfo.nLocs; i++) {
			for (int j = 0; j < gameInfo.totalLocs; j++) {
				if (gameInfo.allHunterLocs[j] == gameInfo.dracLocs[i]) {
					gameInfo.dracLocs[i] = DONT;
					numBadLocs++;
					break;
				}
			}
		}

		// creating a new array for where dracula should go to avoid hunters
		int numGoodLocs = gameInfo.nLocs - numBadLocs;
		PlaceId *DracShouldGoLoc = malloc(sizeof(PlaceId) * numGoodLocs);
		int j = 0;
		for (int i = 0; i < gameInfo.nLocs; i++) {
			if (gameInfo.dracLocs[i] != DONT) {
				DracShouldGoLoc[j] = gameInfo.dracLocs[i];
				j++;
			}
		}

		// randomly going to any of the cities in DracShouldGo
		// (there is probably a better strategy, but can build off this for now)

		// if dracula can safely dodge the hunters
		if (numGoodLocs > 0) {
			int locID = rand() % numGoodLocs;
			play = (char *) placeIdToAbbrev(DracShouldGoLoc[locID]);
			registerBestPlay(play, "BYE BYE BUDDY!!!!");
		} else {
			notGoodMove = true;
			while (notGoodMove) {
				PlaceId locID = rand() % gameInfo.nLocs;
				play = (char *) placeIdToAbbrev(locID);
				registerBestPlay(play, "BYE BYE BUDDY!!!!");
				for (int i = 0; i < gameInfo.totalLocs; i++) {
					if (gameInfo.allHunterLocs[i] == locID) {
						notGoodMove = false;
						break;
					}
				}
			}
		}
		free(DracShouldGoLoc);
		}


		// TODO: need to account for both moves and locations. This is currently
		// only considering moves.
		int numBadLocs = 0;
		// comparing draculas locations to locations possible by all hunters
		for (int i = 0; i < gameInfo.nMoves; i++) {
			for (int j = 0; j < gameInfo.totalLocs; j++) {
				if (gameInfo.allHunterLocs[j] == gameInfo.dracMoves[i]) {
					gameInfo.dracMoves[i] = DONT;
					numBadLocs++;
					break;
				}
			}
		}

		// creating a new array for where dracula should go to avoid hunters
		int numGoodLocs = gameInfo.nMoves - numBadLocs;
		PlaceId *DracShouldGo = malloc(sizeof(PlaceId) * numGoodLocs);
		int j = 0;
		for (int i = 0; i < gameInfo.nMoves; i++) {
			if (gameInfo.dracMoves[i] != DONT) {
				DracShouldGo[j] = gameInfo.dracMoves[i];
				j++;
			}
		}

		// randomly going to any of the cities in DracShouldGo
		// (there is probably a better strategy, but can build off this for now)

		// if dracula can safely dodge the hunters
		if (numGoodLocs > 0) {
			int locID = rand() % numGoodLocs;
			play = (char *) placeIdToAbbrev(DracShouldGo[locID]);
			registerBestPlay(play, "BYE BYE BUDDY!!!!");
		} else {
			notGoodMove = true;
			while (notGoodMove) {
				PlaceId locID = rand() % gameInfo.nMoves;
				play = (char *) placeIdToAbbrev(locID);
				registerBestPlay(play, "BYE BYE BUDDY!!!!");
				for (int i = 0; i < gameInfo.totalLocs; i++) {
					if (gameInfo.allHunterLocs[i] == locID) {
						notGoodMove = false;
						break;
					}
				}
			}
			// // meaning dracula cant move anywhere without encountering a hunter
			// // go to the city containing ONLY one trap

			// // remember in DvGetTrapLocations, if there are multiple traps in one
			// // city, it appears multiple times.

			// // TODO: There might be problems with this code
			// Drac = DvWhereCanIGo(dv, &numLocD);
			// int numTraps = 0;
			// PlaceId *TrapLocs = DvGetTrapLocations(dv, &numTraps);
			// PlaceId TrapsReachable[100];
			// int trapIndex = 0;
			// // TODO: consider uniqueness of cities
			// for (int i = 0; i < numLocD; i++) {
			// 	for (int j = 0; j < numTraps; j++) {
			// 		if (Drac[i] == TrapLocs[j]) {
			// 			TrapsReachable[trapIndex] = Drac[i];
			// 			trapIndex++;
			// 		}
			// 	}
			// }
			// PlaceId *TrapCities = malloc(sizeof(PlaceId) * trapIndex);
			// for (int i = 0; i < trapIndex; i++) {
			// 	TrapCities[i] = TrapsReachable[i];
			// }
			// // randomly going to any city containing a trap
			// int locID = (rand() % (trapIndex - 1)) - 1;
			// play = (char *) placeIdToAbbrev(TrapCities[locID]);
			// registerBestPlay(play, "Trap and kill :)))");

		}

		free(gameInfo.allHunterLocs);
		free(DracShouldGo);
		DvFree(dv);
	}

	free(gameInfo.hunterID[0].reachable);
	free(gameInfo.hunterID[1].reachable);
	free(gameInfo.hunterID[2].reachable);
	free(gameInfo.hunterID[3].reachable);
	free(gameInfo.dracLocs);
	free(gameInfo.dracMoves);

////////////////////////////////////////////////////////////////////////////////
//							     ~ TODO ~						              //
////////////////////////////////////////////////////////////////////////////////
	// The strategy:
	// Ideas:
	// ------------------------------------------------------------
	// using DvWhereCanIgo, and DvWhereCanTheyGo, if the cities they can reach
	// are similar to the cities that you can reach, try going to a different
	// city to avoid them, 
	// ------------------------------------------------------------
	// if all the cities you can reach are reachable by
	// the hunters as well, go to the cities containing the most traps 
	// ------------------------------------------------------------
	// if dracula
	// is getting cornered try to force a TP move by making moves that would make
	// it such that there are no more valid moves. (maximising health gain)
	// ------------------------------------------------------------
	// at the start of the game, try to place as much traps as possible by visiting
	// the cities
	// ------------------------------------------------------------
	// Utilise Hide && Double_back_1 to place multiple traps, also consider
	// placing multiple traps in cities containing an immature vampire
	// *remember that max "encounters" in a city is 3
	// ------------------------------------------------------------
	// if Dracula is <= 20hp make way to Castle Dracula
	// ------------------------------------------------------------
	// Dracula starts at castle dracula, makes a hide move gaining an extra
	// 10 health (becareful, this is a pretty obvious strategy) *probably not
	// actually, it is probably more worth it to plant an immature vampire at
	// the start of the game but perhaps you can start in a city close to castle
	// Dracula and in the next turn go to Castle Dracula
	// ------------------------------------------------------------
	// Since dracula's only source of damage is when at sea or when he encounters
	// a hunter, then dracula can pretty safely spend 4 rounds at sea until
	// he would need to go back to castle dracula and heal up
	// ------------------------------------------------------------
	// ensure that dracula is in a city on the first round and every 13th round
	// only dont go to city if hunters are in that city
	// ------------------------------------------------------------
	// when in boat move from sea to sea to port city, maximising distance away
	// from hunters
	// ------------------------------------------------------------
	// *place traps to cities adjacent to where an immature vampire is, also
	// placing a trap on the city containing the immature vampire, to increase
	// chance of killing hunters and lowering game score.
	// ------------------------------------------------------------
	// Go to Castle Dracula when possible
	// ------------------------------------------------------------
	// *perhaps starting at the middle of the map (e.g Strasbourg) could be
	// a good idea as you would be easily able to navigate to different areas
	// of the map regardless of where the hunters start.
	// ------------------------------------------------------------
	// if the hunters have found draculas trail, dracula makes his best
	// effort to run away.
	// ------------------------------------------------------------
	// if a city is reachable by two or more hunters, and dracula must make an
	// encounter, choose the city that would minimise damage to dracula in this
	// case it would generally be one,
	// ------------------------------------------------------------
	// 

}
