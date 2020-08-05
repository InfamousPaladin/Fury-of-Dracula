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
#include "Map.h"
#include <stdlib.h>
#include <time.h> 
// #include <stdio.h>

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
	PlaceId allHunterLocs[NUM_REAL_PLACES * 4];
	int totalHunterLocs;

	PlaceId dangerHunterLocs[NUM_REAL_PLACES * 4];

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

	DraculaView dv;

} GameState;

Player findLowestHealth(GameState gameInfo);
int playerNearCastle(int GpathLen, int SpathLen, int VpathLen, int MpathLen, int DracPathLen);
void startRoundLoc(GameState gameInfo);
bool castleArea(GameState gameInfo);
bool teleportArea(GameState gameInfo);
bool eastCoastArea(GameState gameInfo);
bool westCoastArea(GameState gameInfo);
void dodgeHunters(DraculaView dv, GameState gameInfo);
void goSeaMove(DraculaView dv, PlaceId seaDracLocs[], int nSeaLocs);

void decideDraculaMove(DraculaView dv)
{
	char *play = "CD";
	registerBestPlay("BI", "Default");
	GameState gameInfo;

	time_t t;
	srand((unsigned) time(&t));

	gameInfo.dv = dv;

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
	// appending all individual hunters possible city moves into one array
	int i = 0;
	for (Player player = PLAYER_LORD_GODALMING; player < PLAYER_DRACULA; player++) {
		for (int j = 0; j < gameInfo.hunterID[player].nReach; j++) {
			if (placeIsLand(gameInfo.hunterID[player].reachable[j])) {
				gameInfo.allHunterLocs[i] = gameInfo.hunterID[player].reachable[j];
				i++;
			}
		}
	}
	gameInfo.totalHunterLocs = i;

	// Round 0
	if (gameInfo.currRound == 0) {
		startRoundLoc(gameInfo);
		return;
	} else if (gameInfo.nMoves == 0) {
		// Case where no more valid moves
		registerBestPlay("TP", "Gotcha Fool");
		return;
	} else {
		// comparing draculas locations to locations possible by all hunters
		int numBadLocs = 0;
		for (int i = 0; i < gameInfo.nLocs; i++) {
			for (int j = 0; j < gameInfo.totalHunterLocs; j++) {
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
		free(gameInfo.dracLocs);
		gameInfo.dracLocs = DvWhereCanIGo(dv, &gameInfo.nLocs);

		// Immediately go to Castle if health is critical
		if (gameInfo.dracHealth <= 20) {
			int pathLen;
			PlaceId *pathToCastle = DvGetShortestPathTo(dv, CASTLE_DRACULA, &pathLen);
			if (gameInfo.nShouldLocs > 0) {
				for (int i = 0; i < gameInfo.nShouldLocs; i++) {
					if (gameInfo.DracShouldGoLocs[i] == pathToCastle[0]) {
						PlaceId loc = pathToCastle[0];
						PlaceId move = DvConvertLocToMove(dv, loc);
						play = (char *) placeIdToAbbrev(move);
						registerBestPlay(play, "recovered");
						free(pathToCastle);
						return;
					}
				}
				int loc = rand() % gameInfo.nShouldLocs;
				PlaceId move = DvConvertLocToMove(dv, loc);
				play = (char *) placeIdToAbbrev(move);
				registerBestPlay(play, "recovered extra");
				free(pathToCastle);
				return;
			} else {
				for (int i = 0; i < gameInfo.nLocs; i++) {
					if (gameInfo.dracLocs[i] == pathToCastle[0]) {
						PlaceId loc = pathToCastle[0];
						PlaceId move = DvConvertLocToMove(dv, loc);
						play = (char *) placeIdToAbbrev(move);
						registerBestPlay(play, "recovered barely");
						free(pathToCastle);
						return;
					}
				}
				int loc = rand() % gameInfo.nLocs;
				PlaceId move = DvConvertLocToMove(dv, loc);
				play = (char *) placeIdToAbbrev(move);
				registerBestPlay(play, "recovered unknown");
				free(pathToCastle);
				return;
			}
		}

		// Strategy One: Go to LS CA GR AL MA SN then repeat
		bool doSouthLoop = false;
		int nPlayers = 0;

		for (int i = PLAYER_LORD_GODALMING; i < PLAYER_DRACULA; i++) {
			for (int j = 0; j < gameInfo.hunterID[i].nReach; j++) {
				if (gameInfo.hunterID[i].reachable[j] == LISBON ||
				gameInfo.hunterID[i].reachable[j] == CADIZ ||
				gameInfo.hunterID[i].reachable[j] == GRANADA ||
				gameInfo.hunterID[i].reachable[j] == ALICANTE ||
				gameInfo.hunterID[i].reachable[j] == MADRID ||
				gameInfo.hunterID[i].reachable[j] == SANTANDER) {
					nPlayers++;
					break;
				}
			}
		}
		if (nPlayers < 2 && gameInfo.dracHealth > 20) doSouthLoop = true;

		// Try to execute the loop
		if (doSouthLoop) {
			int pathLen = 0;
			PlaceId *pathToMadrid = DvGetShortestPathTo(dv, MADRID, &pathLen);
			// Check if the path to MADRID exists and try to go there
			if (pathLen > 1) {
				for (int i = 0; i < gameInfo.nShouldLocs; i++) {
					if (gameInfo.dracLocs[i] == pathToMadrid[0]) {
						PlaceId loc = pathToMadrid[0];
						PlaceId move = DvConvertLocToMove(dv, loc);
						play = (char *) placeIdToAbbrev(move);
						registerBestPlay(play, "Eazy?!!!!");
						free(pathToMadrid);
						return;
					}
				}
			}
			// Signifies that Dracula is on the loop
			if (gameInfo.currDracLoc == LISBON) {
				PlaceId move = DvConvertLocToMove(dv, CADIZ);
				play = (char *) placeIdToAbbrev(move);
				registerBestPlay(play, "Eazy?");
				free(pathToMadrid);
				return;
			} else if (gameInfo.currDracLoc == CADIZ) {
				PlaceId move = DvConvertLocToMove(dv, GRANADA);
				play = (char *) placeIdToAbbrev(move);
				registerBestPlay(play, "Eazy?!");
				free(pathToMadrid);
				return;
			} else if (gameInfo.currDracLoc == GRANADA) {
				PlaceId move = DvConvertLocToMove(dv, ALICANTE);
				play = (char *) placeIdToAbbrev(move);
				registerBestPlay(play, "Eazy?!!");
				free(pathToMadrid);
				return;
			} else if (gameInfo.currDracLoc == ALICANTE) {
				PlaceId move = DvConvertLocToMove(dv, MADRID);
				play = (char *) placeIdToAbbrev(move);
				registerBestPlay(play, "Eazy?!!!");
				free(pathToMadrid);
				return;
			} else if (gameInfo.currDracLoc == MADRID) {
				PlaceId move = DvConvertLocToMove(dv, SANTANDER);
				play = (char *) placeIdToAbbrev(move);
				registerBestPlay(play, "Eazy?!!!!");
				free(pathToMadrid);
				return;
			} else if (gameInfo.currDracLoc == SANTANDER) {
				PlaceId move = DvConvertLocToMove(dv, LISBON);
				play = (char *) placeIdToAbbrev(move);
				registerBestPlay(play, "Eazy?!!!!!");
				free(pathToMadrid);
				return;
			}

			// Just go to a random loc until a path to Madrid has been found
			if (gameInfo.nShouldLocs > 0) {
				int locID = rand() % gameInfo.nShouldLocs;
				PlaceId move = DvConvertLocToMove(dv, locID);
				play = (char *) placeIdToAbbrev(move);
				registerBestPlay(play, "Last 1");
				free(pathToMadrid);
				return;
			} 
			// TODO: try to go to a safer location
			else {
				int locID = rand() % gameInfo.nLocs;
				PlaceId move = DvConvertLocToMove(dv, locID);
				play = (char *) placeIdToAbbrev(move);
				registerBestPlay(play, "Last 2");
				free(pathToMadrid);
				return;
			}
			
		}

		// Strategy Two: Go to random safe places and evade hunters
		else if (gameInfo.nShouldLocs > 0) {
			int DracPathLen = 0;
			PlaceId *pathToCastle = DvGetShortestPathTo(dv, CASTLE_DRACULA, &DracPathLen);
			// Go to the Castle
			if (!castleArea(gameInfo)) {
				// Check if the path to CASTLE_DRACULA exists and no players surround the area
				if (DracPathLen > 0) {
					for (int i = 0; i < gameInfo.nShouldLocs; i++) {
						if (gameInfo.DracShouldGoLocs[i] == pathToCastle[0]) {
							PlaceId loc = pathToCastle[0];
							PlaceId move = DvConvertLocToMove(dv, loc);
							play = (char *) placeIdToAbbrev(move);
							registerBestPlay(play, "Mario?!!!!");
							free(pathToCastle);
							break;
						}
					}
				}
			}
			dodgeHunters(dv, gameInfo);
			return;
		}
		// Case where he can only go to cities that their a potential hunter 
		// could go
		else {
			// Register a random move in case
			bool goodMove = false;
			int minEncounter = 5;
			int repeat = 0;
			while (!goodMove) {
				int encounter = 0;
				PlaceId locID = rand() % gameInfo.nLocs;
				PlaceId move = DvConvertLocToMove(dv, gameInfo.dracLocs[locID]);
				char *play = (char *) placeIdToAbbrev(move);
				registerBestPlay(play, "yolo v");
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
				if (encounter < minEncounter) {
					minEncounter = encounter;
				}
				if (encounter > 1) {
					goodMove = false;
					repeat++;
				}
				else break;
			}
			if (repeat > LIMIT) {
				PlaceId loc = gameInfo.dracLocs[minEncounter];
				PlaceId move = DvConvertLocToMove(dv, loc);
				play = (char *) placeIdToAbbrev(move);
				registerBestPlay(play, "Come here V2!");
			}

			// First consider if the reachable locations are on SEA
			PlaceId seaDracLocs[MAX_REAL_PLACE];
			int nSeaLocs = 0;
			for (int i = 0; i < gameInfo.nLocs; i++) {
				if (placeIsSea(gameInfo.dracLocs[i])) {
					seaDracLocs[nSeaLocs] = gameInfo.dracLocs[i];
					nSeaLocs++;
				}
			}

			if (nSeaLocs > 0 && gameInfo.currDracLoc > 30) {
				goSeaMove(dv, seaDracLocs, nSeaLocs);
				return;
			}

			// Try not to go to hunter's current location
			int nDangerGo = 0;
			for (int i = 0; i < gameInfo.nLocs; i++) {
				if (gameInfo.dracLocs[i] == gameInfo.hunterID[0].currLoc ||
					gameInfo.dracLocs[i] == gameInfo.hunterID[1].currLoc ||
					gameInfo.dracLocs[i] == gameInfo.hunterID[2].currLoc ||
					gameInfo.dracLocs[i] == gameInfo.hunterID[3].currLoc) {
					
					gameInfo.dangerHunterLocs[nDangerGo] = gameInfo.dracLocs[i];
					nDangerGo++;
				}
			}


			// Try to go to castle
			if (gameInfo.dracHealth <= 30 && nDangerGo > 0) {
				int DracPathLen = 0;
				PlaceId *pathToCastle = DvGetShortestPathTo(dv, CASTLE_DRACULA, &DracPathLen);
				// Go to the Castle
					
				bool pathExist = false;
				// Check if the path to CASTLE_DRACULA exists
				if (DracPathLen > 0) {
					for (int i = 0; i < nDangerGo; i++) {
						if (gameInfo.dangerHunterLocs[i] == pathToCastle[0]) {
							pathExist = true;
							break;
						}
					}
				}
				// Try to go back to CASTLE_DRACULA
				if (pathExist) {
					PlaceId loc = pathToCastle[0];
					PlaceId move = DvConvertLocToMove(dv, loc);
					play = (char *) placeIdToAbbrev(move);
					registerBestPlay(play, "Mario?!!!!");
					free(pathToCastle);
					return;
				}
			} 

			// Try to evade as much as possible
			// Try to encounter a player with lowest health
			Player lowPlayer = findLowestHealth(gameInfo);

			// Consider which city have traps
			// TODO: Consider the uniqness of trap locations
			int nTrapsReach = 0;
			PlaceId TrapsReachable[MAX_REAL_PLACE];
			for (int i = 0; i < nDangerGo; i++) {
				if (gameInfo.dangerHunterLocs[i] == gameInfo.activeTrapLocs[i] &&
					gameInfo.activeTrapLocs[i] != gameInfo.hunterID[0].currLoc) {
					TrapsReachable[nTrapsReach] = gameInfo.activeTrapLocs[i];
					nTrapsReach++;
				}
			}

			// Try to go offensive set and aim for an encounter near a trap city
			// Also consider possible number of hunters
			if (lowPlayer != PLAYER_DRACULA && gameInfo.dracHealth >= 30 &&
				nTrapsReach != 0) {

				bool goodMove = false;
				int repeat = 0;
				int minEncounter = 5;
				while (!goodMove) {
					int encounter = 0;
					int locID = rand() % nTrapsReach;
					PlaceId loc = TrapsReachable[locID];
					PlaceId move = DvConvertLocToMove(dv, loc);
					play = (char *) placeIdToAbbrev(move);
					registerBestPlay(play, "Come here!");
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
					if (encounter < minEncounter) {
						minEncounter = encounter;
					}
					if (encounter > 1) {
						goodMove = false;
						repeat++;
					}
					else break;
				}
				if (repeat > LIMIT) {
					PlaceId loc = TrapsReachable[minEncounter];
					PlaceId move = DvConvertLocToMove(dv, loc);
					play = (char *) placeIdToAbbrev(move);
					registerBestPlay(play, "Come here V2!");
				}
				return;
			} 
			
		}
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

bool castleArea(GameState gameInfo)
{
	bool nearCastle = false;
	for (int i = 0; i < gameInfo.totalHunterLocs; i++) {
		if (gameInfo.allHunterLocs[i] == CASTLE_DRACULA ||
			gameInfo.allHunterLocs[i] == KLAUSENBURG	||
			gameInfo.allHunterLocs[i] == GALATZ 		|| 
			gameInfo.allHunterLocs[i] == BUCHAREST 		||
			gameInfo.allHunterLocs[i] == SZEGED 		||
			gameInfo.allHunterLocs[i] == CONSTANTA) 
		{
			nearCastle = true;
			break;
		}
	}
	if (gameInfo.currRound > 4 && !nearCastle) {
		for (int i = 0; i < PLAYER_DRACULA; i++) {
			int nMoves = 0;	bool canFree = true;
			PlaceId *moves = DvGetLastMoves(gameInfo.dv, i, TRAIL_SIZE, &nMoves, &canFree);
			for (int i = 0; i < nMoves - 1; i++) {
				if (moves[i] == moves[i + 1]) {
					nearCastle = false;
					break;
				}
				else nearCastle = true;
			}
			if (nearCastle) break;
			free(moves);
		}
	}

	return nearCastle;
}

bool teleportArea(GameState gameInfo) 
{
	for (int i = 0; i < gameInfo.totalHunterLocs; i++) {
		if (gameInfo.allHunterLocs[i] == CAGLIARI) {
			return true;
		}
	}
	return false;
}

bool eastCoastArea(GameState gameInfo)
{
	for (int i = 0; i < gameInfo.totalHunterLocs; i++) {
		if (gameInfo.allHunterLocs[i] == GALWAY ||
			gameInfo.allHunterLocs[i] == DUBLIN) {
			return true;
		}
	}
	return false;
}

bool westCoastArea(GameState gameInfo) 
{
	for (int i = 0; i < gameInfo.totalHunterLocs; i++) {
		if (gameInfo.allHunterLocs[i] == CONSTANTA ||
			gameInfo.allHunterLocs[i] == VARNA) {
			return true;
		}
	}
	return false;
}

void startRoundLoc(GameState gameInfo) 
{
	// TODO: Strategy:
	//	1) Start at CASTLE_DRACULA to gain health
	// 	2) Force a TELEPORT to gain health
	//	3) Start near the coasts


	bool castleDanger = castleArea(gameInfo);
	bool teleportDanger = teleportArea(gameInfo);
	bool englandCoastDanger = eastCoastArea(gameInfo);
	bool westCoastDanger = westCoastArea(gameInfo);

	if (!castleDanger) {
		registerBestPlay("CD", "DC");
	} else if (!teleportDanger) {
		registerBestPlay("TS", "CD");
	} else if (!englandCoastDanger) {
		registerBestPlay("GW", "comp1511 here i come");
	} else if (!westCoastDanger) {
		registerBestPlay("VR", "be a hunter");
	} else {
		bool goodMove = false;
		while (!goodMove) {
			PlaceId locID = rand() % NUM_REAL_PLACES;
			char *play = (char *) placeIdToAbbrev(locID);
			registerBestPlay(play, "yolo");
			goodMove = true;
			for (int i = 0; i < gameInfo.totalHunterLocs; i++) {
				if (gameInfo.allHunterLocs[i] == locID || locID == HOSPITAL_PLACE) {
					goodMove = false;
					break;
				}
			}
		}
	}
}

void dodgeHunters(DraculaView dv, GameState gameInfo)
{
	int locID = rand() % gameInfo.nShouldLocs;
	PlaceId loc = gameInfo.DracShouldGoLocs[locID];
	PlaceId move = DvConvertLocToMove(dv, loc);
	char *play = (char *) placeIdToAbbrev(move);
	registerBestPlay(play, "Cant catch me");
}

void goSeaMove(DraculaView dv, PlaceId seaDracLocs[], int nSeaLocs)
{
	int locID = rand() % nSeaLocs;
	PlaceId loc = seaDracLocs[locID];
	PlaceId move = DvConvertLocToMove(dv, loc);
	char *play = (char *) placeIdToAbbrev(move);
	registerBestPlay(play, "Aquaman");
}