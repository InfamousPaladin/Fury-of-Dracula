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

// Written in the month of July 2020 for COMP2521 T2 Assignment 2.

// This is the implementation file for the GameView.h ADT. Implementation of the
// following code was completed by Gabriel Ting, Christian Ilagan, Tam Do and
// Prathamesh Jagtap.

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Game.h"
#include "GameView.h"
#include "Map.h"
#include "Places.h"

// The following ADT has adapted for use in this program from the COMP2521 labs.
// It was written by UNSW staff and for the purpose of the assignment, has been
// used to utilise the ADT. Queue.h has been adapted from COMP2521 lab05 2020T2.
#include "Queue.h"

// Defined constants
#define IMMATURE_VAMPIRE   	'V'
#define TRAP     	       	'T'
#define TRAP_EXPIRED       	'M'

#define MAX_TRAP_LOCATIONS 	18

#define TURN_CHARS			8	// chars each turn takes (include space)
#define ROUND_CHARS			40 	// chars each round takes (include space)
#define POS_ACTIONS 		6 	// player actions; 2 for location; 4 for rest


#define	START_RAIL_DIST		1
#define UNINTIALISED		-1

#define MIN_ENCOUNTER_CHAR	3
#define MAX_ENCOUNTER_CHAR	7

// Contains information and current game state of the player
typedef struct playerInfo
{
	Player name;						// name of player
	PlaceId location;					// current location of player
	int health;							// current player health
} playerInfo;

// Contains information for current state of the game
struct gameView 
{
	char *playString; 					// Stores all past plays
	Message *messages;					// stores messages
	Map map; 							// map of the board
	int nMapLocs; 						// number of locations on map
	
	Round currRound; 					// current round of game
	Player currPlayer; 					// whos turn
	int score; 							// current score of the game

	playerInfo playerID[NUM_PLAYERS];	// array that contains each player info
	PlaceId imVampireLoc; 				// location of immature vampires

	PlaceId *activeTrapLocs;			// locations of all active traps
	int nTraps;							// number of active traps
};

// Prototypes for helper functions.
static char getPlayerInitial(int player);
static PlaceId getPlaceId(GameView gv, Player player, Round round);
void setVisited(PlaceId from, int visited[], int nElements);
static void GvHunterHealthGain(GameView gv, int i, Player player);
static void GvCheckDracEncounter(GameView gv, int i, Player player);
static void GvCheckDracLocationHealth(GameView gv,char playerID, Player player);
static PlaceId convertHideDoubleBack(
	PlaceId moves[], 
	PlaceId location, 
	Round round);
static void GvHunterVampireScore(
	GameView gv, 
	int i, 
	Player player, 
	playerInfo *playerID);
static void GvHunterHealthLoss(
	GameView gv, 
	bool *playerDead, 
	int *position, 
	int i, 
	Player player);
static void GvCheckHunterTrap(
	GameView gv, 
	int i, 
	Place traps, 
	int *nTraps, 
	PlaceId tempLocations[MAX_TRAP_LOCATIONS]);
static void GvCheckDraculaPlaceTrap(
	GameView gv, 
	Round round,
	int i, 
	Place traps, 
	int *nTraps, 
	PlaceId tempLocations[MAX_TRAP_LOCATIONS]);
static void findValidRailMove(
	GameView gv,
	struct connNode reachable[],
	int visited[],
	PlaceId from,
	int *nElement,
	int railDist,
	Player player);
static void getHunterReachableType(
	GameView gv, 
	PlaceId from, 
	int visited[],
	struct connNode reachable[], 
	int *nElements, 
	int type);
static void getDraculaReachableType(
	GameView gv, 
	PlaceId from, 
	struct connNode reachable[], 
	int *nElements, 
	int type);

//**************************************************************************//
//                		   Constructor/Deconstructor 		 	            //
//**************************************************************************//

GameView GvNew(char *pastPlays, Message messages[]) {
	assert(pastPlays != NULL);
	assert(messages != NULL);

	GameView new = malloc(sizeof(struct gameView));
	if (new == NULL) {
		fprintf(stderr, "Couldn't allocate GameView!\n");
		exit(EXIT_FAILURE);
	}
	
	new->playString = pastPlays;
	new->messages = messages;
	
	// initialising a new map and number of places on map
	new->map = MapNew();
	new->nMapLocs = MapNumPlaces(new->map);

	// set up current state of game
	new->currRound = GvGetRound(new);
	new->currPlayer = GvGetPlayer(new);

	// set up current information of the players
	new->playerID[0].name = PLAYER_LORD_GODALMING;
	new->playerID[1].name = PLAYER_DR_SEWARD;
	new->playerID[2].name = PLAYER_VAN_HELSING;
	new->playerID[3].name = PLAYER_MINA_HARKER;
	new->playerID[4].name = PLAYER_DRACULA;
	new->playerID[0].location = GvGetPlayerLocation(new, PLAYER_LORD_GODALMING);
	new->playerID[1].location = GvGetPlayerLocation(new, PLAYER_DR_SEWARD);
	new->playerID[2].location = GvGetPlayerLocation(new, PLAYER_VAN_HELSING);
	new->playerID[3].location = GvGetPlayerLocation(new, PLAYER_MINA_HARKER);
	new->playerID[4].location = GvGetPlayerLocation(new, PLAYER_DRACULA);
	new->playerID[0].health = GvGetHealth(new, PLAYER_LORD_GODALMING);
	new->playerID[1].health = GvGetHealth(new, PLAYER_DR_SEWARD);
	new->playerID[2].health = GvGetHealth(new, PLAYER_VAN_HELSING);
	new->playerID[3].health = GvGetHealth(new, PLAYER_MINA_HARKER);
	new->playerID[4].health = GvGetHealth(new, PLAYER_DRACULA);
	
	// get the current score of the game
	new->score = GvGetScore(new);
	
	// get traps on the map
	new->nTraps = 0;
	new->activeTrapLocs = GvGetTrapLocations(new, &new->nTraps);

	// getting vampire locations
	new->imVampireLoc = GvGetVampireLocation(new);

	return new;
}

void GvFree(GameView gv) {
	MapFree(gv->map);
	free(gv->activeTrapLocs);
	free(gv);
}

//**************************************************************************//
//                		      Game State Information 		 	            //
//**************************************************************************//

Round GvGetRound(GameView gv)
{
	int numMoves = 1;
	for (int i = 0; gv->playString[i] != '\0'; i++) {
		if (gv->playString[i] == ' ') numMoves++;
	}

	Round currRound = numMoves / NUM_PLAYERS;
	return currRound;
}

Player GvGetPlayer(GameView gv)
{
	int playerNum = 0;
	for (int i = 0; gv->playString[i] != '\0'; i++) {
		
		// calculates which player is next in turn
		if (gv->playString[i] == 'G' || gv->playString[i] == 'S' ||
            gv->playString[i] == 'H' || gv->playString[i] == 'M' ||
            gv->playString[i] == 'D') {
			i += POS_ACTIONS;
			playerNum++;
		}

		// returns back to playerID 0 after playerID 4
		if (playerNum == NUM_PLAYERS) playerNum = 0;
	}

	Player currPlayer = playerNum;
	return currPlayer;
}

int GvGetScore(GameView gv)
{
	gv->score = GAME_START_SCORE;
	playerInfo playerID[NUM_PLAYERS];

	// Each round that goes by that Dracula is alive
	gv->score -= gv->currRound * SCORE_LOSS_DRACULA_TURN;

	// Set initial health points
	playerID[0].health = GAME_START_HUNTER_LIFE_POINTS;
	playerID[1].health = GAME_START_HUNTER_LIFE_POINTS;
	playerID[2].health = GAME_START_HUNTER_LIFE_POINTS;
	playerID[3].health = GAME_START_HUNTER_LIFE_POINTS;

	// Consider how score is affected based on player health
	for (int i = 0; gv->playString[i] != '\0'; i += TURN_CHARS) {
		Player player;
		if (gv->playString[i] == 'G')		player = PLAYER_LORD_GODALMING;
		else if (gv->playString[i] == 'S')	player = PLAYER_DR_SEWARD;
		else if (gv->playString[i] == 'H') 	player = PLAYER_VAN_HELSING;
		else if (gv->playString[i] == 'M') 	player = PLAYER_MINA_HARKER;
		else if (gv->playString[i] == 'D') 	player = PLAYER_DRACULA;
		GvHunterVampireScore(gv, i, player, playerID);
		if (gv->playString[i + TURN_CHARS - 1] == '\0') break;
	}
	return gv->score;
}

int GvGetHealth(GameView gv, Player player)
{

	// Set initial health points
	if (player != PLAYER_DRACULA) {
		gv->playerID[player].health = GAME_START_HUNTER_LIFE_POINTS;
	} else {
		gv->playerID[player].health = GAME_START_BLOOD_POINTS;
	}	

	// Extract intial of player
	char playerID = getPlayerInitial(player);
	bool playerDead = false;
	// Variable to hold position on the string when the hunter died
	int position = 0;

	// Check the hunter's POV
	if (player != PLAYER_DRACULA) {
		// Traverse through playString round to find 'G', 'S', 'H' or 'M'
		for (int i = 0; gv->playString[i] != '\0'; i += TURN_CHARS) {
			// Check if the hunter is dead and revive them if they are
			if (playerDead && position == i - ROUND_CHARS) {
				gv->playerID[player].health = GAME_START_HUNTER_LIFE_POINTS;
				playerDead = false;
			}
			if (gv->playString[i] == playerID) {
				GvHunterHealthLoss(gv, &playerDead, &position, i, player);
				// Check if the hunter rested
				GvHunterHealthGain(gv, i, player);
			}
			if (gv->playString[i + TURN_CHARS - 1] == '\0') break;
		}
	// Check Dracula's POV
	} else {
		// Check if Dracula encountered a hunter
		for (int i = 0; gv->playString[i] != '\0'; i += TURN_CHARS) {
			GvCheckDracEncounter(gv, i, player);
			if (gv->playString[i + TURN_CHARS - 1] == '\0') break;
		}
		GvCheckDracLocationHealth(gv, playerID, player);
	}

	return gv->playerID[player].health;
}

PlaceId GvGetPlayerLocation(GameView gv, Player player)
{
	// return Hospital if dead
	if (player != PLAYER_DRACULA && GvGetHealth(gv, player) == 0) {
		return HOSPITAL_PLACE;
	}

	int nLocs = -1;	bool canFree = true;
	PlaceId *playerLocs = GvGetLocationHistory(gv, player, &nLocs, &canFree);
	PlaceId currLoc = playerLocs[nLocs - 1];
	if (canFree) free(playerLocs);

	// Player has not made a turn yet
	if (nLocs == 0) {
		return NOWHERE;
	} else {
		return currLoc;
	}
}

PlaceId GvGetVampireLocation(GameView gv)
{
	// booleans
	bool foundLocation = false;

	// arrays
	char playerPlace[4];
	char placeAbbrev[4];

	// int variables
	int playerRound = 0;
	int imVampireRound = 0;
	int vampRoundIncrement = 0;
	int playerRoundIncrement = 0;

	// storing location variables
	Place playerLoc;
	Place immvampireLoc;
	playerLoc.abbrev = playerPlace;
	immvampireLoc.abbrev = placeAbbrev;

	// finding immvampires location
	for (int i = 0; gv->playString[i] != '\0'; i++) {
		
		// keeps record of which round the vampire was planted
		if (gv->playString[i] == ' ') vampRoundIncrement++;

		// immvampire found is found
		if (gv->playString[i] == 'D' && gv->playString[i + 4] == 'V') {

			// obtain two initials of place
			immvampireLoc.abbrev[0] = gv->playString[i + 1];
			immvampireLoc.abbrev[1] = gv->playString[i + 2];
			immvampireLoc.abbrev[2] = '\0';
		
			// round of immVampire & placeID 
			foundLocation = true;
			imVampireRound = vampRoundIncrement / NUM_PLAYERS;
			immvampireLoc.id = placeAbbrevToId(immvampireLoc.abbrev);

		// immvampire has matured
		} else if (gv->playString[i] == 'D' && gv->playString[i + 5] == 'V') {
			// vampire has hatched
			immvampireLoc.id = NOWHERE;
		}
	}

	// immvampire not found or immvampire has matured
	if (!foundLocation || immvampireLoc.id == NOWHERE) return NOWHERE;

	// checks if hunter has been in the same location as
	// immvampire for the last 6 rounds (relative to hunter) and 
	// kills the vampire if yes so immvampire location is NOWHERE
	for (int i = 0; gv->playString[i] != '\0'; i++) {

		// keeps record of which round the player is in
		if (gv->playString[i] == ' ') playerRoundIncrement++;
		
		// gets player location
		if ((gv->playString[i] == 'G'     || gv->playString[i] 	   == 'S' ||
             gv->playString[i] == 'H' 	  || gv->playString[i]     == 'M') && 
			(gv->playString[i + 6] == '.' || gv->playString[i + 6] == 'D')) {

			// obtain two initials of place
			playerLoc.abbrev[0] = gv->playString[i + 1];
			playerLoc.abbrev[1] = gv->playString[i + 2];
			playerLoc.abbrev[2] = '\0';

			// get player round
			playerRound = playerRoundIncrement / NUM_PLAYERS;
			playerLoc.id = placeAbbrevToId(playerLoc.abbrev);

			// immvampire encountered and killed
			if (playerLoc.id == immvampireLoc.id &&
				playerRound >= imVampireRound) {
				immvampireLoc.id = NOWHERE;
				return immvampireLoc.id;
			}
		}
	}

	return immvampireLoc.id;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	// String to hold location initials
	Place traps;
	char placeAbbrev[3];
	traps.abbrev = placeAbbrev;

	int nTraps = 0;
	Round round = 0;

	// Make a temp array to store the trap locations
	PlaceId tempLocations[MAX_TRAP_LOCATIONS];

	// Transverses through the stirng at the position 'D'
	for (int i = 0; gv->playString[i] != '\0'; i += TURN_CHARS) {
		// Check every D for a 'T'
		if (gv->playString[i] == 'D') {
			GvCheckDraculaPlaceTrap(gv, round,i, traps, &nTraps, tempLocations);
			round++;
		// Case where it is a hunter and check if they stepped on a trap
		} else {
			GvCheckHunterTrap(gv, i, traps, &nTraps, tempLocations);
		}
		if (gv->playString[i + TURN_CHARS - 1] == '\0') break;
	}
	// Place the locations of traps into array
	PlaceId *trapLocations = malloc(sizeof(PlaceId) * (nTraps));
	for (int i = 0; i < nTraps; i++) {
		trapLocations[i] = tempLocations[i];
	}
	
	*numTraps = nTraps;
	return trapLocations;
}

//**************************************************************************//
//                		      	 Game History 		 	           		    //
//**************************************************************************//

PlaceId *GvGetMoveHistory(GameView gv, Player player,
                          int *numReturnedMoves, bool *canFree)
{
	int nMoves = gv->currRound;
	if (player < gv->currPlayer) nMoves++; 

	PlaceId *moves = malloc(sizeof(PlaceId) * (nMoves));
	int i;
	for (i = 0; i < nMoves; i++) {
		PlaceId currId = getPlaceId(gv, player, i);
		moves[i] = currId;
	}
	*numReturnedMoves = i;

	return moves;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
	// Clamp overflow for numMoves
	if (numMoves > gv->currRound) numMoves = gv->currRound;

	if (player < gv->currPlayer && numMoves == gv->currRound) {
		numMoves++;
	}

	// Formula to find the last accessible move in pastPlay string 
	int startIndex = gv->currRound - numMoves;
	if (player < gv->currPlayer) {
		startIndex++;
	}

	// Error checks and bounds
	if (startIndex < 0) startIndex = 0;

	PlaceId *moves = malloc(sizeof(PlaceId) * numMoves);

	int i;
	for (i = 0; i < numMoves; i++, startIndex++) {
		PlaceId currId = getPlaceId(gv, player, startIndex);
		moves[i] = currId;
	}
	*numReturnedMoves = i;

	return moves;
}

PlaceId *GvGetLocationHistory(GameView gv, Player player,
                              int *numReturnedLocs, bool *canFree)
{
	int nMoves = gv->currRound;
	// Case where the player is a hunter, this function should behave exactly
	// the same as GvGetMoveHistory.
	if (player != PLAYER_DRACULA) {
		return GvGetMoveHistory(gv, player, numReturnedLocs, canFree);
	} else {
		if (player < gv->currPlayer) nMoves++; 
		PlaceId *moves = malloc(sizeof(PlaceId) * (nMoves));

		int i;
		for (i = 0; i < nMoves; i++) {
			PlaceId currId = getPlaceId(gv, player, i);
			currId = convertHideDoubleBack(moves, currId, i);
			moves[i] = currId;
		}
		*numReturnedLocs = i;
		return moves;
	}
}

PlaceId *GvGetLastLocations(GameView gv, Player player, int numLocs,
                            int *numReturnedLocs, bool *canFree)
{
	// Case where the player is a hunter, this function should behave exactly
	// the same as GvGetLastMoves.
	if (player != PLAYER_DRACULA) {
		return GvGetLastMoves(gv, player, numLocs, numReturnedLocs, canFree);
	} else {
		bool canFreeAllMoves = true;
		PlaceId *allMoves = GvGetLocationHistory(gv, player, numReturnedLocs,
												 &canFreeAllMoves);
		
		// Clamp overflow for numLocs
		if (numLocs > gv->currRound) numLocs = gv->currRound;

		if (player < gv->currPlayer && numLocs == gv->currRound) {
			numLocs++;
		}

		// Formula to find the last accessible move in pastPlay string 
		int startIndex = gv->currRound - numLocs;
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

//**************************************************************************//
//                		    	 Making a Move  		 	                //
//**************************************************************************//

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{
	if (from == NOWHERE) return NULL;

	// Initialise visited array
	int visited[gv->nMapLocs];
	setVisited(from, visited, gv->nMapLocs);
	struct connNode reachable[gv->nMapLocs];
	int nLocs = 0;

	// If player is a hunter, consider rail moves
	if (player != PLAYER_DRACULA) {
		int railDist = (round + player) % 4;
		if (railDist > 0) {
			getHunterReachableType(gv, from, visited, reachable, &nLocs, RAIL);
			findValidRailMove(gv, reachable, visited, from,	&nLocs, 
								railDist, player);
		}
		getHunterReachableType(gv, from, visited, reachable, &nLocs, ROAD);
		getHunterReachableType(gv, from, visited, reachable, &nLocs, BOAT);
	} else {
		// Get dracula's connections for ROAD and BOAT
		getDraculaReachableType(gv, from, reachable, &nLocs, ROAD); 
		getDraculaReachableType(gv, from, reachable, &nLocs, BOAT); 
	}

	// Include starting location as a valid move
	reachable[nLocs].p = from;
	nLocs++;
	*numReturnedLocs = nLocs;	

	// Copies placeID from reachable and store it in a new array
	PlaceId *reachableID = malloc(sizeof(PlaceId) * nLocs);
	for (int i = 0; i < nLocs; i++) {
		reachableID[i] = reachable[i].p;
	}
	return reachableID;
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	// Initialise visited array
	int visited[gv->nMapLocs];
	setVisited(from, visited, gv->nMapLocs);
	struct connNode reachable[gv->nMapLocs];
	int max = 0;

	// If player is a hunter, consider rail moves
	if (player != PLAYER_DRACULA) {
		// Getting all the rail connections if allowed
		int railDist = (round + player) % 4;
		if (rail && railDist > 0) {
			getHunterReachableType(gv, from, visited, reachable, &max, RAIL);
			findValidRailMove(gv, reachable, visited, from,	&max, 
								railDist, player);
		}
		// getting all the road connections
		if (road) {
			getHunterReachableType(gv, from, visited, reachable, &max, 
									ROAD);
		}
		// getting all the sea connections
		if (boat) {
			getHunterReachableType(gv, from, visited, reachable, &max, BOAT);
		}
	} 
	// Otherwise, dracula (cannot move to hospital, or via rail)
	else {
		if (road) {
			getDraculaReachableType(gv, from, reachable, &max, ROAD); 
		}
		// getting boat connections
		if (boat) {
			getDraculaReachableType(gv, from, reachable, &max, BOAT); 
		}
	}

	// Append starting location to array
	reachable[max].p = from;
	max++;
	*numReturnedLocs = max;	

	PlaceId *allReachable = malloc(sizeof(PlaceId) * max);
	for (int i = 0; i < max; i++) {
		allReachable[i] = reachable[i].p;
	}

	return allReachable;
}


//**************************************************************************//
//                		    	Helper Functions 		 	                //
//**************************************************************************//

// Maps the player assigned ID value with the corresponding character value.
static char getPlayerInitial(int player)
{
	if (player == PLAYER_LORD_GODALMING) 	return 'G';
	else if (player == PLAYER_DR_SEWARD) 	return 'S';
	else if (player == PLAYER_VAN_HELSING) 	return 'H';
	else if (player == PLAYER_MINA_HARKER) 	return 'M';
	else 									return 'D';
}

// Converts DOUBLE_BACK, HIDE and TELEPORT moves to the correct place ID
// depending on the move history.
static PlaceId convertHideDoubleBack(PlaceId moves[], PlaceId currId, 
										Round round) {
	// Finds dracula location when performing his special moves
	if (currId == HIDE || currId == DOUBLE_BACK_1) {
		currId = moves[round - 1];
	} else if (currId == DOUBLE_BACK_2) {
		currId = moves[round - 2];
	} else if (currId == DOUBLE_BACK_3) {
		currId = moves[round - 3];
	} else if (currId == DOUBLE_BACK_4) {
		currId = moves[round - 4];
	} else if (currId == DOUBLE_BACK_5) {
		currId = moves[round - 5];
	} else if (currId == TELEPORT) {
		currId = CASTLE_DRACULA;
	}
	return currId;
}

// Helper function for GvGetScore: Analyse the score gained from the death
// of a hunter and a vampire maturing
static void GvHunterVampireScore(GameView gv, int i, Player player, 
								 	playerInfo *playerID)
{
	// Hunter's POV
	if (player != PLAYER_DRACULA) {
		// Check the last four characters to see if anything has happened
		for (int j = MIN_ENCOUNTER_CHAR; j < MAX_ENCOUNTER_CHAR; j++) {
			if (gv->playString[i + j] == TRAP) {
				playerID[player].health -= LIFE_LOSS_TRAP_ENCOUNTER;
			} else if (gv->playString[i + j] == 'D') {
				playerID[player].health -= LIFE_LOSS_DRACULA_ENCOUNTER;
			}
		}
		if (playerID[player].health <= 0) {
			gv->score -= SCORE_LOSS_HUNTER_HOSPITAL;
			playerID[player].health = GAME_START_HUNTER_LIFE_POINTS;
		}
	// Dracula's POV
	} else {
		if (gv->playString[i + 5] == IMMATURE_VAMPIRE) {
			gv->score -= SCORE_LOSS_VAMPIRE_MATURES; 
		}
	}
}

// Helper function for GvGetHealth: Analyse the health loss of the hunter when
// encountering with traps or Dracula.
static void GvHunterHealthLoss(GameView gv, bool *playerDead, int *position, 
								int i, Player player)
{
	// Check the last four characters to see if anything has happened
	for (int j = 3; j < 7; j++) {
		if (gv->playString[i + j] == TRAP) {
			gv->playerID[player].health -= LIFE_LOSS_TRAP_ENCOUNTER;
		} else if (gv->playString[i + j] == 'D') {
			gv->playerID[player].health -= LIFE_LOSS_DRACULA_ENCOUNTER;
		}
		if (gv->playerID[player].health <= 0) {
			gv->playerID[player].health = 0;
			*playerDead = true;
			*position = i;
		}
	}
}

// Helper function for GvGetHealth: Analyse the health gain for the hunter
static void GvHunterHealthGain(GameView gv, int i, Player player)
{
	if (i >= ROUND_CHARS) {
		if (gv->playString[i + 1] == gv->playString[i + 1 - ROUND_CHARS]) {
			if (gv->playString[i + 2] == gv->playString[i + 2 - ROUND_CHARS]) {
				// This means the hunter stayed in the same location
				gv->playerID[player].health += LIFE_GAIN_REST;
			}
		}
		// Cap the hunter's health at 9 if they get above it
		if (gv->playerID[player].health > GAME_START_HUNTER_LIFE_POINTS) {
			gv->playerID[player].health = GAME_START_HUNTER_LIFE_POINTS;
		}
	}
}

// Helper function for GvGetHealth: Analyse the health of Dracula if he
// encountered a hunter.
static void GvCheckDracEncounter(GameView gv, int i, Player player)
{
	for (int j = MIN_ENCOUNTER_CHAR; j < MAX_ENCOUNTER_CHAR; j++) {
		if (gv->playString[i + j] == 'D') {
			gv->playerID[player].health -= LIFE_LOSS_HUNTER_ENCOUNTER;
		}
	}
}

// Helper function for GvGetHealth: Checks Dracula's health corresponding to
// his location.
static void GvCheckDracLocationHealth(GameView gv, char playerID, Player player)
{
	Place location;
	char placeAbbrev[3];
	location.abbrev = placeAbbrev;
	Round round = 0;
	// Get past locations of dracula
	int nMoves = 0;	bool canFree = true;
	PlaceId *moves = GvGetLocationHistory(gv, PLAYER_DRACULA,&nMoves, &canFree);

	for (int i = 0; gv->playString[i] != '\0'; i += TURN_CHARS) {
		if (gv->playString[i] == playerID) {
			// Get Dracula's location
			location.abbrev[0] = gv->playString[i + 1];
			location.abbrev[1] = gv->playString[i + 2];
			location.abbrev[2] = '\0';
			// Convert the abbrev to name
			location.id = placeAbbrevToId(location.abbrev);
			location.id = convertHideDoubleBack(moves, location.id, round);

			// Regains health if Dracula is in Castle Dracula
			if (location.id == CASTLE_DRACULA) {
				gv->playerID[player].health += LIFE_GAIN_CASTLE_DRACULA;
			}
			// Check if Dracula is at sea
			if (placeIsSea(location.id)) {
				gv->playerID[player].health -= LIFE_LOSS_SEA;
			}
			// Check if Dracula is dead
			if (gv->playerID[player].health <= 0) {
				// Dracula is dead (game is over)
				gv->playerID[player].health = 0;
				break;
			}
			round++;
		}
		if (gv->playString[i + TURN_CHARS - 1] == '\0') break;
	}
	if (canFree) free(moves);
}

// Helper function for GvGetTrapLocations: Check if Dracula places a trap 
// in a location.
static void GvCheckDraculaPlaceTrap(GameView gv, Round round, int i, Place traps, 
				int *nTraps, PlaceId tempLocations[MAX_TRAP_LOCATIONS]) {
	// Get past locations of dracula
	int nMoves = 0;	bool canFree = true;
	PlaceId *moves = GvGetLocationHistory(gv, PLAYER_DRACULA,&nMoves, &canFree);

	if (gv->playString[i + 3] == TRAP) {
		// Extract the location name
		traps.abbrev[0] = gv->playString[i + 1];
		traps.abbrev[1] = gv->playString[i + 2];
		traps.abbrev[2] = '\0';
		// Convert the abbrev to name
		traps.id = placeAbbrevToId(traps.abbrev);
		traps.id = convertHideDoubleBack(moves, traps.id, round);

		// Store the location in the array
		tempLocations[*nTraps] = traps.id;
		++*nTraps;
	}

	// Decrease the numTraps if the trap has expired
	if (gv->playString[i + 5] == TRAP_EXPIRED) {
		// Remove the oldest location (first element)
		for (int j = 1; j < *nTraps; j++) {
			tempLocations[j - 1] = tempLocations[j];
		}
		--*nTraps;
	}

	if (canFree) free(moves);
}

// Helper function for GvGetTrapLocations: Removes a trap if a hunter steps
// on one.
static void GvCheckHunterTrap(GameView gv, int i, Place traps, int *nTraps, 
								PlaceId tempLocations[MAX_TRAP_LOCATIONS]) {
	// Hunter stepped on a trap
	for (int j = MIN_ENCOUNTER_CHAR; j < MAX_ENCOUNTER_CHAR; j++) {
		if (gv->playString[i + j] == TRAP) {
			// Extract the location name
			traps.abbrev[0] = gv->playString[i + 1];
			traps.abbrev[1] = gv->playString[i + 2];
			traps.abbrev[2] = '\0';
			// Convert the abbrev to name
			traps.id = placeAbbrevToId(traps.abbrev);

			// Find what position the trap is in and remove it
			int j;
			for (j = 0; j < *nTraps; j++) {
				if (tempLocations[j] == traps.id) break;
			}
			// Remove the element
			for (int k = j; k < *nTraps; k++) {
				tempLocations[k] = tempLocations[k + 1];
			}
			--*nTraps;
		}
	}
}

// Helper function for `Game History`: Returns the placeId (location) for a
// player of a given round.
static PlaceId getPlaceId (GameView gv, Player player, Round round)
{
	Place location;
	char placeAbbrev[3];
	location.abbrev = placeAbbrev;

	// Formula to calculate index of the player location in a given round
	int playerTurn = TURN_CHARS * player + ROUND_CHARS * round;
	location.abbrev[0] = gv->playString[playerTurn + 1]; 
	location.abbrev[1] = gv->playString[playerTurn + 2];
	location.abbrev[2] = '\0';
	
	location.id = placeAbbrevToId(location.abbrev);

	return location.id;
}

// Helper function for  `Game History`: Set the visited array to be unitialised.
void setVisited(PlaceId from, int visited[], int nElements)
{
	for (int i = 0; i < nElements; i++) {
		visited[i] = UNINTIALISED;
	}
	visited[from] = from;
}

// Helper function for `Making a Move`: Find all valid rail moves using a 
// breadth first search and return the total number of elements in reachable 
// array.
static void findValidRailMove(GameView gv, struct connNode reachable[],
							 int visited[], PlaceId from, int *nElement,
							 int railDist, Player player)
{
	if (railDist <= 1) return;

	Queue railLocs = newQueue();
	// Add all rail transport from the starting point to the queue
	for (int i = 0; i < *nElement; i++) {
		visited[reachable[i].p] = from;
		QueueJoin(railLocs, reachable[i].p);
	}

	int i = *nElement;
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
			for (ConnList nextCity = reachFromRail; nextCity != NULL;
				nextCity = nextCity->next) {
				// Filter out cities that dont have rail connections from
				// currCity.
				if (nextCity->type == RAIL &&
					visited[nextCity->p] == UNINTIALISED) {
					// Visit cities that can be access by rail and add to queue.
					reachable[i].p = nextCity->p;
					visited[nextCity->p] = currCity;
					QueueJoin(railLocs, nextCity->p);
					i++;
				}
			}
		}
	}
	*nElement = i;
}

// Helper function for `Making a Move`: Returns an array of rechable locations 
// for only a specified type of transportation (ROAD, RAIL, BOAT) for hunters 
// from the given location `from`.
static void getHunterReachableType(GameView gv, PlaceId from, int visited[],
									struct connNode reachable[], 
									int *nElements, int type)
{
	// get all adjacent connections
	ConnList startReached = MapGetConnections(gv->map, from);
	int i = *nElements;
	for (ConnList curr = startReached; curr != NULL; curr = curr->next) {
		if (visited[curr->p] == UNINTIALISED && curr->type == type) {
			reachable[i].p = curr->p;
			visited[curr->p] = from;
			i++;
		}
	}
	*nElements = i;
}

// Helper function for `Making a Move`: Returns an array of rechable locations 
// for only a specified type of transportation (ROAD and BOAT) for Dracula from 
// the given location `from`. 
static void getDraculaReachableType(GameView gv, PlaceId from,
									struct connNode reachable[], 
									int *nElements, int type)
{
	// get all adjacent connections
	ConnList startReached = MapGetConnections(gv->map, from);
	int i = *nElements;
	for (ConnList curr = startReached; curr != NULL; curr = curr->next) {
		if (curr->type == type && curr->p != HOSPITAL_PLACE) {
			reachable[i].p = curr->p;
			i++;
		}
	}
	*nElements = i;
}
