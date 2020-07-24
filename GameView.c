////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// GameView.c: GameView ADT implementation
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
#include <string.h>

#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"

// add your own #includes here
#include "Queue.h"

#define TURN_CHARS	8	// chars each turn takes in play string (w space)
#define ROUND_CHARS	40 	// chars each round takes in play string (w space)
#define POS_ACTIONS 6 	// player actions; 2 for location; 4 for rest
#define PLRACT_STRING 7 // each player string length
#define	START_RAIL_DIST	1
#define UNDECLARED	-1	// path not found yet


// TODO: ADD YOUR OWN STRUCTS HERE
// each player is given player information
// this information is mapped to playerInfo
typedef struct playerInfo {
	int health;
	Player name;
	Place location;
} playerInfo;


struct gameView 
{
	Round round; // keeps track of the round
	Map map; // map of the board
	Player currPlayer; // whos turn
	int score; // current score of the game
	playerInfo playerID[NUM_PLAYERS];
	char *playString; // Stores all past plays (i.e. game log)
	int nPlaces; // number of places/cities in map
	Place imvampireLocation; // keeps track of vampires
	Place trapLocations[]; // an array of traps and their locations
};

// Helper Function Prototypes
static Place getPlaceId(GameView gv, Player player, int round);
static int findValidRailMove(
	GameView gv,
	struct connNode reachable[],
	int visited[],
	PlaceId from,
	int nElement);
static PlaceId GvDraculaDoubleBack(
	GameView gv,
	Place playerLoc,
	int roundBack);

////////////////////////////////////////////////////////////////////////
// Constructor/Destructor

GameView GvNew(char *pastPlays, Message messages[]) {
	// // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	// Basically summarises the current state of the game
	// pastPlays variable = gamelog
	// messages array holds each play (same number of elements as pastPlays)
	// first play will be at index 0.
	assert(pastPlays != NULL);
	assert(messages != NULL);

	GameView new = malloc(sizeof(struct gameView));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}
	// assigning a playstring
	new->playString = pastPlays;
	// initialising a new map
	new->map = MapNew();
	// getting the current round of the game
	new->round = GvGetRound(new);
	// Updating the information of the players
	new->playerID[0].location.id =  GvGetPlayerLocation(new, PLAYER_LORD_GODALMING);
	new->playerID[1].location.id = GvGetPlayerLocation(new, PLAYER_DR_SEWARD);
	new->playerID[2].location.id = GvGetPlayerLocation(new, PLAYER_VAN_HELSING);
	new->playerID[3].location.id = GvGetPlayerLocation(new, PLAYER_MINA_HARKER);
	new->playerID[4].location.id = GvGetPlayerLocation(new, PLAYER_DRACULA);
	new->playerID[0].health = GvGetHealth(new, PLAYER_LORD_GODALMING);
	new->playerID[1].health = GvGetHealth(new, PLAYER_DR_SEWARD);
	new->playerID[2].health = GvGetHealth(new, PLAYER_VAN_HELSING);
	new->playerID[3].health = GvGetHealth(new, PLAYER_MINA_HARKER);
	new->playerID[4].health = GvGetHealth(new, PLAYER_DRACULA);
	// getting the current score of the game
	new->score = GvGetScore(new);
	// getting the current player
	new->currPlayer = GvGetPlayer(new);
	// getting traps on the map and storing it in gv struct
	PlaceId *TrapLocs = GvGetTrapLocations(new, 0);
	int i = 0;
	while (TrapLocs[i] != '\0') {
		new->trapLocations[i].id = TrapLocs[i];
		i++;
	}
	// getting vampire locations
	new->imvampireLocation.id = GvGetVampireLocation(new);
	return new;
}

void GvFree(GameView gv) {
	// // TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	// free(gv->players);
	// MapFree(gv->map);
	// free(gv);
}

////////////////////////////////////////////////////////////////////////
// Game State Information

Round GvGetRound(GameView gv) {

	int numMoves = 1;
	for (int i = 0; gv->playString[i] != '\0'; i++) {
		if (gv->playString[i] == ' ') numMoves++;
	}

	gv->round = numMoves / NUM_PLAYERS;
	return gv->round;
}

Player GvGetPlayer(GameView gv) {

	int playerNum = 1;
	for (int i = 0; gv->playString[i] != '\0'; i++) {
		
		// calculates which player is next in turn
		if (gv->playString[i] == 'G' || gv->playString[i] == 'S' ||
            gv->playString[i] == 'H' || gv->playString[i] == 'M' ||
            gv->playString[i] == 'D') {
			i += POS_ACTIONS;
			playerNum++;
		}

		// returns back to playerID 1 after playerID 5
		if (playerNum == NUM_PLAYERS + 1) playerNum = 1;
	}

	if (playerNum == 1) {
		gv->currPlayer = PLAYER_LORD_GODALMING;
	} else if (playerNum == 2) {
		gv->currPlayer = PLAYER_DR_SEWARD;
	} else if (playerNum == 3) {
		gv->currPlayer = PLAYER_VAN_HELSING;
	} else if (playerNum == 4) {
		gv->currPlayer = PLAYER_MINA_HARKER;
	} else if (playerNum == 5) {
		gv->currPlayer = PLAYER_DRACULA;
	}

	return gv->currPlayer;
}

int GvGetScore(GameView gv) {
	return gv->score;
}

int GvGetHealth(GameView gv, Player player) {
	return gv->playerID[player].health;
}

// helper function for GvGetPlayerLocation()
static PlaceId GvDraculaDoubleBack(GameView gv, Place playerLoc, int roundBack) {

	int numMoves = 0;
	for (int i = 0; gv->playString[i] != '\0'; i++) {

		// calculates number of moves in a given
		// range of rounds
		if (gv->playString[i] == ' ') numMoves++;
		int countRound = numMoves / 5;
		if (countRound == roundBack) break;
	}

	// calculating number of individual chars in a given range of rounds
	// finding Dracula's position after his use of DOUBLE_BACK_N
	for (int i = 0; i <= (numMoves + PLRACT_STRING * NUM_PLAYERS * roundBack); i++) {

		if (gv->playString[i] == 'D' && gv->playString[i + 1] != '.' && 
			gv->playString[i + 2] != '.') {

			// obtain two initials of place
			playerLoc.abbrev[0] = gv->playString[i + 1];
			playerLoc.abbrev[1] = gv->playString[i + 2];
			playerLoc.abbrev[2] = '\0';

			// get placeID
			playerLoc.id = placeAbbrevToId(playerLoc.abbrev);
		}
	}

	return playerLoc.id;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player) {

	Place playerLoc;
	int roundBack = 0;
	char nameInitials;
	char placeAbbrev[3];
	bool foundLocation = false;
	playerLoc.abbrev = placeAbbrev;

	// mapping to initials
	if (player == PLAYER_LORD_GODALMING) {
		nameInitials = 'G';
	} else if (player == PLAYER_DR_SEWARD) {
		nameInitials = 'S';
	} else if (player == PLAYER_VAN_HELSING) {
		nameInitials = 'H';
	} else if (player == PLAYER_MINA_HARKER) {
		nameInitials = 'M';
	} else if (player == PLAYER_DRACULA) {
		nameInitials = 'D';
	}

	for (int i = 0; gv->playString[i] != '\0'; i++) {
		
		// need to fix this dual-condition thing
		if (gv->playString[i] == nameInitials && i == 0) {

			// obtain two initials of place
			playerLoc.abbrev[0] = gv->playString[i + 1];
			playerLoc.abbrev[1] = gv->playString[i + 2];
			playerLoc.abbrev[2] = '\0';

			// get placeID
			playerLoc.id = placeAbbrevToId(playerLoc.abbrev);

			//playerNum++;
			i += POS_ACTIONS;
			foundLocation = true;			

		} else if (gv->playString[i] == nameInitials && 
				   gv->playString[i - 1] == ' '      && i > 0) {

			// obtain two initials of place
			playerLoc.abbrev[0] = gv->playString[i + 1];
			playerLoc.abbrev[1] = gv->playString[i + 2];
			playerLoc.abbrev[2] = '\0';

			// get placeID
			playerLoc.id = placeAbbrevToId(playerLoc.abbrev);

			//playerNum++;
			i += POS_ACTIONS;
			foundLocation = true;
		}
	}

	// various conditions obtained from rules
	if (foundLocation == false) {

		return NOWHERE;

	} else if (player == PLAYER_DRACULA) {

		// Dracula gets teleported to CASTLE_DRACULA
		if (playerLoc.id == TELEPORT) {

			gv->playerID[PLAYER_DRACULA].location.id = CASTLE_DRACULA;
			return gv->playerID[PLAYER_DRACULA].location.id;

		// Not really sure about his, under work
		} else if (playerLoc.id == HIDE) {

			if (gv->playerID[PLAYER_DRACULA].location.id != 0) {
				return gv->playerID[PLAYER_DRACULA].location.id;
			}

		// getting location back if Dracula uses DOUBLE_BACK_N
		} else if (playerLoc.id >= DOUBLE_BACK_1 && playerLoc.id <= DOUBLE_BACK_5) {

			int movebackBy = 0;
			if (playerLoc.id == DOUBLE_BACK_1) {
				movebackBy = 1;
			} else if (playerLoc.id == DOUBLE_BACK_2) {
				movebackBy = 2;
			} else if (playerLoc.id == DOUBLE_BACK_3) {
				movebackBy = 3;
			} else if (playerLoc.id == DOUBLE_BACK_4) {
				movebackBy = 4;
			} else if (playerLoc.id == DOUBLE_BACK_5) {
				movebackBy = 5;
			}

			roundBack = GvGetRound(gv) - movebackBy;
			while (playerLoc.id > SEA_UNKNOWN) {
				playerLoc.id = GvDraculaDoubleBack(gv, playerLoc, roundBack);
				roundBack--;
			}
		}
	}

	gv->playerID[player].location.id = playerLoc.id;
	return gv->playerID[player].location.id;
}

PlaceId GvGetVampireLocation(GameView gv) {

	char placeAbbrev[3];
	Place immvampireLoc;
	bool foundLocation = false;
	immvampireLoc.abbrev = placeAbbrev;

	for (int i = 0; gv->playString[i] != '\0'; i++) {

		if (gv->playString[i] == 'D' && gv->playString[i + 4] == 'V') {

			// obtain two initials of place
			immvampireLoc.abbrev[0] = gv->playString[i + 1];
			immvampireLoc.abbrev[1] = gv->playString[i + 2];
			immvampireLoc.abbrev[2] = '\0';

			// get placeID
			foundLocation = true;
			immvampireLoc.id = placeAbbrevToId(immvampireLoc.abbrev);

		} else if (gv->playString[i] == 'D' && gv->playString[i + 5] == 'V') {

			// vampire has hatched
			gv->imvampireLocation.id = NOWHERE;
			return gv->imvampireLocation.id;
		}
	}

	// cases where vampire is not present
	// or hunter killed it before it matured
	if (foundLocation == false) {
		return NOWHERE;
	} else if (GvGetPlayerLocation(gv, PLAYER_DR_SEWARD) == immvampireLoc.id) {
		if (GvGetRound(gv) < 6) return NOWHERE;
	} else if (GvGetPlayerLocation(gv, PLAYER_VAN_HELSING) == immvampireLoc.id) {
		if (GvGetRound(gv) < 6) return NOWHERE;
	} else if (GvGetPlayerLocation(gv, PLAYER_MINA_HARKER) == immvampireLoc.id) {
		if (GvGetRound(gv) < 6) return NOWHERE;
	} else if (GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == immvampireLoc.id) {
		if (GvGetRound(gv) < 6) return NOWHERE;
	}

	gv->imvampireLocation.id = immvampireLoc.id;
	return gv->imvampireLocation.id;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	*numTraps = 0;
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Game History
// Returns the placeId (location) of a player for a given round
static Place getPlaceId (GameView gv, Player player, int round)
{
	Place location;
	char placeAbbrev[2];
	location.abbrev = placeAbbrev;

	// Formula to calculate index of the player location in a given round
	int playerTurn = TURN_CHARS * player + ROUND_CHARS * round;
	location.abbrev[0] = gv->playString[playerTurn + 1]; 
	location.abbrev[1] = gv->playString[playerTurn + 2];
	
	location.id = placeAbbrevToId(location.abbrev);


	return location;
}

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	// TODO: Test - placeholder data
	gv->playString =
			"GLS.... SLS.... HLS.... MGE.... DST.V.. "
			"GCA.... SAL.... HAL.... MGE.... DC?T... "
			"GGR.... SBO.... HBO.... MGE.... DC?T... "
			"GAL.... SNA.... HNA.... MGE.... DD3T... "
			"GSR.... SPA.... HPA.... MGE.... DHIT... "
			"GSN.... SST.... HST.... MGE.... DC?T... "
			"GMA.... SFLTTV.";
	gv->round = 6;
	gv->currPlayer = PLAYER_VAN_HELSING;

	if (player < gv->currPlayer) gv->round++; 
	PlaceId *moves = malloc(sizeof(PlaceId) * (gv->round));

	int i;
	for (i = 0; i < gv->round; i++) {
		Place curr = getPlaceId(gv, player, i);
		moves[i] = curr.id;
	}
	*numReturnedMoves = i;

	return moves;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	// TODO: Test - placeholder data
	gv->playString =
			"GLS.... SLS.... HLS.... MGE.... DST.V.. "
			"GCA.... SAL.... HAL.... MGE.... DC?T... "
			"GGR.... SBO.... HBO.... MGE.... DC?T... "
			"GAL.... SNA.... HNA.... MGE.... DD3T... "
			"GSR.... SPA.... HPA.... MGE.... DHIT... "
			"GSN.... SST.... HST.... MGE.... DC?T... "
			"GMA.... SFLTTV.";
	gv->round = 6;
	gv->currPlayer = PLAYER_VAN_HELSING;

	// Formula to find the last accessible move in pastPlay string 
	int startIndex = gv->round - numMoves;
	if (player < gv->currPlayer) startIndex++;

	// Error checks and bounds them
	if (startIndex < 0) startIndex = 0;
	if (numMoves > gv->round) numMoves = gv->round;

	PlaceId *moves = malloc(sizeof(PlaceId) * numMoves);

	int i;
	for (i = 0; i < numMoves; i++, startIndex++) {
		Place curr = getPlaceId(gv, player, startIndex);
		moves[i] = curr.id;
	}
	*numReturnedMoves = i;

	return moves;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	// TODO: Test - placeholder data
	gv->playString =
			"GLS.... SLS.... HLS.... MGE.... DST.V.. "
			"GCA.... SAL.... HAL.... MGE.... DC?T... "
			"GGR.... SBO.... HBO.... MGE.... DC?T... "
			"GAL.... SNA.... HNA.... MGE.... DD3T... "
			"GSR.... SPA.... HPA.... MGE.... DHIT... "
			"GSN.... SST.... HST.... MGE.... DC?T... "
			"GMA.... SFLTTV.";
	gv->round = 6;
	gv->currPlayer = PLAYER_VAN_HELSING;

	// Case where the player is a hunter, this function should behave exactly
	// the same as GvGetMoveHistory.
	if (player != PLAYER_DRACULA) {
		return GvGetMoveHistory(gv, player, numReturnedLocs, canFree);
	} else {
		if (player < gv->currPlayer) gv->round++; 
		PlaceId *moves = malloc(sizeof(PlaceId) * (gv->round));

		int i;
		for (i = 0; i < gv->round; i++) {
			Place curr = getPlaceId(gv, player, i);
			
			// Finds dracula location when performing his special moves
			if (curr.id == HIDE || curr.id == DOUBLE_BACK_1)
				curr.id = moves[i - 1];
			else if (curr.id == DOUBLE_BACK_2)
				curr.id = moves[i - 2];
			else if (curr.id == DOUBLE_BACK_3)
				curr.id = moves[i - 3];
			else if (curr.id == DOUBLE_BACK_4)
				curr.id = moves[i - 4];
			else if (curr.id == DOUBLE_BACK_5)
				curr.id = moves[i - 5];
			else if (curr.id == TELEPORT)
				curr.id = CASTLE_DRACULA;

			moves[i] = curr.id;
		}
		*numReturnedLocs = i;
		return moves;
	}
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	// TODO: Test - placeholder data
	gv->playString =
			"GLS.... SLS.... HLS.... MGE.... DST.V.. "
			"GCA.... SAL.... HAL.... MGE.... DC?T... "
			"GGR.... SBO.... HBO.... MGE.... DC?T... "
			"GAL.... SNA.... HNA.... MGE.... DD3T... "
			"GSR.... SPA.... HPA.... MGE.... DHIT... "
			"GSN.... SST.... HST.... MGE.... DC?T... "
			"GMA.... SFLTTV.";
	gv->round = 6;
	gv->currPlayer = PLAYER_VAN_HELSING;

	// Case where the player is a hunter, this function should behave exactly
	// the same as GvGetLastMoves.
	if (player != PLAYER_DRACULA) {
		return GvGetLastMoves(gv, player, numLocs, numReturnedLocs, canFree);
	} else {
		bool canFreeAllMoves = true;
		PlaceId *allMoves = GvGetLocationHistory(gv, player, numReturnedLocs,
												 &canFreeAllMoves);
		
		// Formula to find the last accessible move in pastPlay string 
		int startIndex = gv->round - numLocs;
		if (player < gv->currPlayer) startIndex++;
		// Error checks and bounds
		if (startIndex < 0) startIndex = 0;
		PlaceId *lastNMoves = malloc(sizeof(PlaceId) * numLocs);

		int i;

		// Copy values last N moves from allMoves array to lastNMoves array
		for (i = 0; i < numLocs; i++, startIndex++) {
			lastNMoves[i] = allMoves[startIndex];
		}
		*numReturnedLocs = i;

		if (canFreeAllMoves) free(allMoves);
		return lastNMoves;
	}
}

////////////////////////////////////////////////////////////////////////
// Making a Move

// Find all valid rail moves using a breadth first search and return the
// total number of elements in reachable array.
static int findValidRailMove(GameView gv, struct connNode reachable[],
				  int visited[], PlaceId from, int nElement) {

	Queue railLocs = newQueue();

	// Add all rail transport from the starting point to the queue
	for (int curr = 0; curr < nElement; curr++) {
		if (reachable[curr].type == RAIL) {
			visited[reachable[curr].p] = from;
			QueueJoin(railLocs, reachable[curr].p);
		}
	}

	int railDist = (gv->round + gv->currPlayer) % 4;

	while (!QueueIsEmpty(railLocs)) {
		Item currCity = QueueLeave(railLocs);

		int currRailDist = START_RAIL_DIST;
		int pathIndex = currCity;
		// Check rail distance from currCity to `from` (starting point)
		while (visited[pathIndex] != from) {
			pathIndex = visited[pathIndex];
			currRailDist++;
		}
		
		if (currRailDist < railDist) {
			ConnList reachFromRail = MapGetConnections(gv->map, currCity);
			ConnList nextCity = reachFromRail;
			while (nextCity != NULL) {
				// Filter out cities that dont have rail connections from
				// currCity.
				if (nextCity->type == RAIL && visited[nextCity->p] == UNDECLARED) {
					reachable[nElement].p = nextCity->p;
					reachable[nElement].type = nextCity->type;
					reachable[nElement].next = NULL;

					visited[nextCity->p] = currCity;
					QueueJoin(railLocs, nextCity->p);
					nElement++;
				}
				nextCity = nextCity->next;
			}
		}
	}
	return nElement;
}

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{

	// TODO - test data
	gv->currPlayer = player;
	gv->round = round;
	gv->map = MapNew();
	gv->nPlaces = MapNumPlaces(gv->map);

	// get availiable connections
	ConnList startReached = MapGetConnections(gv->map, from);
	struct connNode reachable[gv->nPlaces];

	// Initialise visited array
	int visited[gv->nPlaces];
	for (int i = 0; i < gv->nPlaces; i++) {
		visited[i] = UNDECLARED;
	}
	visited[from] = from;	

	int i = 0;
	// If player is a hunter, consider rail moves
	if (player != PLAYER_DRACULA) {
		ConnList curr = startReached;
		// Goes through startReached list and store values in array only for
		// rail moves.
		while (curr != NULL) {
			if (visited[curr->p] == -1 && curr->type == RAIL) {
				reachable[i].p = curr->p;
				reachable[i].type = curr->type;
				reachable[i].next = NULL;
				visited[curr->p] = from;
				i++;
			}
			curr = curr->next;
		}
		i = findValidRailMove(gv, reachable, visited, from, i);
		// Now add all reachable locations
		curr = startReached;
		while (curr != NULL) {
			if (visited[curr->p] == -1) {
				reachable[i].p = curr->p;
				reachable[i].type = curr->type;
				i++;
			}
			curr = curr->next;
		}
	}
	// Otherwise, moves of dracula must not be `HOSPITAL_PLACE` and rail moves
	else {
		ConnList curr = startReached;
		while (curr != NULL) {
			if (curr->p != HOSPITAL_PLACE && curr->type != RAIL
			&& visited[curr->p] == UNDECLARED) {
				reachable[i].p = curr->p;
				reachable[i].type = curr->type;
				reachable[i].next = NULL;
				visited[curr->p] = from;
				i++;
			}
			curr = curr->next;
		}
	}

	PlaceId *allReachable = malloc(sizeof(PlaceId) * i + 1);
	int j;
	for (j = 0; j < i; j++) {
		allReachable[j] = reachable[j].p;
	}

	// Append starting location to array
	i++;
	allReachable[j] = from;
	*numReturnedLocs = i;
	return allReachable;
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION

	// Basically this function considers the connection type based on the 
	// bools inputted into the function. e.g. if road & rail = true, then
	// only include the moves available by road & rail.

	// Steps:
	// 1. Check all available moves (road, rail, boat) using the bool.
	// 2. Consider the graph of the map, and adjacent verticies (cities).
	// 3. Create a dynamically allocated array.
	// 4. Determine the number of appropiate adjacent cities.
	// 5. Consider if player is Dracula or Hunter.


	
	if (road && boat && rail) {
		PlaceId *reached = GvGetReachable(gv, player, round, from, numReturnedLocs);
		return reached;
	} 
	// get availiable connections
	/*ConnList startReached = MapGetConnections(gv->map, from);
	struct connNode reachable[gv->nPlaces];

	// Initialise visited array
	int visited[gv->nPlaces];
	for (int i = 0; i < gv->nPlaces; i++) {
		visited[i] = UNDECLARED;
	}
	if (player == PLAYER_DRACULA) {
		ConnList curr = startReached;
		while (curr != NULL) {
			if (visited[curr->p] == UNDECLARED) {
				
			}
			curr = curr->next;
		}
	}*/
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
