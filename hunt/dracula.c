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

#define DONT 	-1000
#define LIMIT	100

typedef struct hunter
{
	PlaceId currLoc;					// current location of player
	int health;							// current player health
	PlaceId *reachable;
	int nReach;
} hunterInfo;

typedef struct gameState 
{
	Round currRound;
	int score;

	hunterInfo hunterID[4];
	PlaceId *allHunterLocs;
	int totalLocs;

	PlaceId currDracLoc;

	PlaceId *DracShouldGoLocs;
	int nShouldLocs;

	PlaceId *dracLocs;
	int nLocs;

	PlaceId *dracMoves;
	int nMoves;

	PlaceId dracHealth;

	PlaceId *activeTrapLocs;
	int nTraps;

} GameState;

Player findLowestHealth(GameState gameInfo);
int playerNearCastle(int GpathLen, int SpathLen, int VpathLen, int MpathLen, int DracPathLen);

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
	gameInfo.currDracLoc = DvGetPlayerLocation(dv, PLAYER_DRACULA);

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
	gameInfo.activeTrapLocs = DvGetTrapLocations(dv, &gameInfo.nTraps);

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
		// Start at Castle Dracula if possible
		bool castle = false;

		for (int i = 0; i < gameInfo.totalLocs; i++) {
			if (gameInfo.allHunterLocs[i] == CASTLE_DRACULA ||
				gameInfo.allHunterLocs[i] == KLAUSENBURG ||
				gameInfo.allHunterLocs[i] == GALATZ || 
				gameInfo.allHunterLocs[i] == BUCHAREST ||
				gameInfo.allHunterLocs[i] == SZEGED ||
				gameInfo.allHunterLocs[i] == CONSTANTA ||
				gameInfo.allHunterLocs[i] == BELGRADE ||
				gameInfo.allHunterLocs[i] == SOFIA) {
				castle = true;
				break;
			}
		}

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

		if (!castle) {
			registerBestPlay("CD", "leblanc");
		} else if (!englandCoast) {
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
		bool goodMove = false;
		int repeat = 0;
		while (!goodMove) {
			int locID = rand() % gameInfo.nLocs;
			PlaceId loc = gameInfo.dracLocs[locID];
			PlaceId move = DvConvertLocToMove(dv, loc);
			play = (char *) placeIdToAbbrev(move);
			goodMove = true;
			registerBestPlay(play, "BYE BYE BUDDY!!!!");
			for (int i = 0; i < gameInfo.totalLocs; i++) {
				if (gameInfo.allHunterLocs[i] == locID) {
					goodMove = false;
					break;
				}
			}
			if (repeat > LIMIT) break;
			repeat++;
		}

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
		gameInfo.nShouldLocs = gameInfo.nLocs - numBadLocs;
		gameInfo.DracShouldGoLocs = malloc(sizeof(PlaceId) * gameInfo.nShouldLocs);
		int j = 0;
		for (int i = 0; i < gameInfo.nLocs; i++) {
			if (gameInfo.dracLocs[i] != DONT) {
				gameInfo.DracShouldGoLocs[j] = gameInfo.dracLocs[i];
				j++;
			}
		}

		// if dracula can safely dodge the hunters
		if (gameInfo.nShouldLocs > 0) {
			bool goodMove = false;
			repeat = 0;
			while (!goodMove) {
				int locID = rand() % gameInfo.nShouldLocs;
				PlaceId loc = gameInfo.DracShouldGoLocs[locID];
				PlaceId move = DvConvertLocToMove(dv, loc);
				play = (char *) placeIdToAbbrev(move);
				registerBestPlay(play, "Phew!!!!");
				goodMove = true;
				for (int i = 0; i < gameInfo.totalLocs; i++) {
					if (gameInfo.allHunterLocs[i] == locID) {
						goodMove = false;
						break;
					}
				}
				if (repeat > LIMIT || gameInfo.nShouldLocs == 1) break;
				repeat++;
			}
			// Try to go to CASTLE DRACULA if possible
			int GpathLen = 0;
			int SpathLen = 0;
			int VpathLen = 0;
			int MpathLen = 0;
			HvGetShortestPathTo(dv, PLAYER_LORD_GODALMING, CASTLE_DRACULA, &GpathLen);
			HvGetShortestPathTo(dv, PLAYER_DR_SEWARD, CASTLE_DRACULA, &SpathLen);
			HvGetShortestPathTo(dv, PLAYER_VAN_HELSING, CASTLE_DRACULA, &VpathLen);
			HvGetShortestPathTo(dv, PLAYER_MINA_HARKER, CASTLE_DRACULA, &MpathLen);
			int DracPathLen = 0;
			PlaceId *pathToCastle = DvGetShortestPathTo(dv, CASTLE_DRACULA, &DracPathLen);
			// Go back to Castle Dracula
			int nearCastle = playerNearCastle(GpathLen, SpathLen, VpathLen, MpathLen, DracPathLen);
			if (gameInfo.dracHealth <= 30 && nearCastle < 2) {
				if (DracPathLen > 1) {
					PlaceId loc = pathToCastle[0];
					PlaceId move = DvConvertLocToMove(dv, loc);
					play = (char *) placeIdToAbbrev(move);
					registerBestPlay(play, "Mario?!!!!");
					free(pathToCastle);
				} else {
					for (int i = 0; i < gameInfo.nLocs; i++) {
						if (gameInfo.dracLocs[i] == CASTLE_DRACULA) {
							PlaceId move = DvConvertLocToMove(dv, gameInfo.dracLocs[i]);
							play = (char *) placeIdToAbbrev(move);
							registerBestPlay(play, "Back then");
							return;
						}
					}
				}
			}

		}
		// Case where he can only go to cities that their a potential hunter 
		// could go
		else {
			Player lowPlayer = findLowestHealth(gameInfo);

			// Consider which city have traps
			// TODO: Consider the uniqness of trap locations
			int nTrapsReach = 0;
			PlaceId TrapsReachable[MAX_REAL_PLACE];
			for (int i = 0; i < gameInfo.nTraps; i++) {
				if (gameInfo.DracShouldGoLocs[i] == gameInfo.activeTrapLocs[i]) {
					TrapsReachable[nTrapsReach] = gameInfo.activeTrapLocs[i];
					nTrapsReach++;
				}
			}

			// Try to go offensive set and aim for an encounter near a trap city
			if (lowPlayer != PLAYER_DRACULA && gameInfo.dracHealth >= 30 &&
				nTrapsReach != 0) {
				int locID = rand() % nTrapsReach;
				PlaceId loc = TrapsReachable[locID];
				PlaceId move = DvConvertLocToMove(dv, loc);
				play = (char *) placeIdToAbbrev(move);
				registerBestPlay(play, "Lets go!!!!");
			} 
			// Try to avoid much of the hunter encounters if possible and go to
			// CASTLE_DRACULA to regain health
			else {
				for (int i = 0; i < gameInfo.nLocs; i++) {
					if (gameInfo.dracLocs[i] == CASTLE_DRACULA) {
						registerBestPlay("CD", "Back then");
						return;
					}
				}
				int pathLen = 0;
				PlaceId *pathToCastle = DvGetShortestPathTo(dv, CASTLE_DRACULA, &pathLen);
				if (pathLen > 1) {
					PlaceId loc = pathToCastle[0];
					PlaceId move = DvConvertLocToMove(dv, loc);
					play = (char *) placeIdToAbbrev(move);
					registerBestPlay(play, "Mario?!!!!");
					free(pathToCastle);
				} else {
					goodMove = false;
					repeat = 0;
					while (!goodMove) {
						int encounter = 0;
						int locID = rand() % gameInfo.nLocs;
						PlaceId loc = gameInfo.dracLocs[locID];
						PlaceId move = DvConvertLocToMove(dv, loc);
						play = (char *) placeIdToAbbrev(move);
						registerBestPlay(play, "Last Resort!!!!");
						goodMove = true;
						for (int i = 0; i < gameInfo.hunterID[0].nReach; i++) {
							if (gameInfo.hunterID[0].reachable[i] == locID) {
								encounter++;
								break;
							}
						}
						for (int i = 0; i < gameInfo.hunterID[1].nReach; i++) {
							if (gameInfo.hunterID[1].reachable[i] == locID) {
								encounter++;
								break;
							}
						}
						for (int i = 0; i < gameInfo.hunterID[2].nReach; i++) {
							if (gameInfo.hunterID[2].reachable[i] == locID) {
								encounter++;
								break;
							}
						}
						for (int i = 0; i < gameInfo.hunterID[3].nReach; i++) {
							if (gameInfo.hunterID[3].reachable[i] == locID) {
								encounter++;
								break;
							}
						}
						if (repeat > LIMIT) break;
						if (encounter > 1) {
							goodMove = false;
							repeat++;
						}
						else break;
					}
				}
			}
		}
		free(gameInfo.allHunterLocs);
		free(gameInfo.DracShouldGoLocs);
	}

	free(gameInfo.hunterID[0].reachable);
	free(gameInfo.hunterID[1].reachable);
	free(gameInfo.hunterID[2].reachable);
	free(gameInfo.hunterID[3].reachable);
	free(gameInfo.dracLocs);
	free(gameInfo.dracMoves);
	free(gameInfo.activeTrapLocs);
}

Player findLowestHealth(GameState gameInfo)
{
	if (gameInfo.hunterID[0].health <= 4) return PLAYER_LORD_GODALMING;
	else if (gameInfo.hunterID[1].health <= 4) return PLAYER_DR_SEWARD;
	else if (gameInfo.hunterID[2].health <= 4) return PLAYER_VAN_HELSING;
	else if (gameInfo.hunterID[3].health <= 4) return PLAYER_MINA_HARKER;
	return PLAYER_DRACULA;
}

int playerNearCastle(int GpathLen, int SpathLen, int VpathLen, int MpathLen, int DracPathLen)
{
	int players = 0;
	if (DracPathLen >= GpathLen && GpathLen < 5) players++;
	if (DracPathLen >= SpathLen && SpathLen < 5) players++;
	if (DracPathLen >= VpathLen && VpathLen < 5) players++;
	if (DracPathLen >= MpathLen && MpathLen < 5) players++;
	return players;
}