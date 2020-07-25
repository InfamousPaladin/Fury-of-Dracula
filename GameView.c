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

// The following ADT has adapted for use in this program from the COMP2521 labs.
// It was written by UNSW staff and for the purpose of the assignment, has been
// used to utilise the ADT. Queue.h has been adapted from COMP2521 lab05 2020T2.
#include "Queue.h"

#define TURN_CHARS	8	// chars each turn takes in play string (w space)
#define ROUND_CHARS	40 	// chars each round takes in play string (w space)
#define POS_ACTIONS 6 	// player actions; 2 for location; 4 for rest
#define PLRACT_STRING 7 // each player string length
#define	START_RAIL_DIST	1
#define UNINTIALISED	-1	// path not found yet


#define IMMATURE_VAMPIRE   'V'
#define TRAP     	       'T'
#define TRAP_EXPIRED       'M'
#define MAX_TRAP_LOCATIONS 18

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
};

// Helper Function Prototypes
static PlaceId GvDraculaDoubleBack(GameView gv, Place playerLoc, int roundBack);
static Place getPlaceId(GameView gv, Player player, int round);
static int findValidRailMove(
	GameView gv,
	struct connNode reachable[],
	int visited[],
	PlaceId from,
	int nElement,
	Round round,
	Player player);
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
	new->nPlaces = MapNumPlaces(new->map);
	// getting the current round of the game
	new->round = GvGetRound(new);
	// Updating the information of the players
	new->playerID[0].name = PLAYER_LORD_GODALMING;
	new->playerID[1].name = PLAYER_DR_SEWARD;
	new->playerID[2].name = PLAYER_VAN_HELSING;
	new->playerID[3].name = PLAYER_MINA_HARKER;
	new->playerID[4].name = PLAYER_DRACULA;
	new->playerID[0].location.id = GvGetPlayerLocation(new, PLAYER_LORD_GODALMING);
	new->playerID[1].location.id = GvGetPlayerLocation(new, PLAYER_DR_SEWARD);
	new->playerID[2].location.id = GvGetPlayerLocation(new, PLAYER_VAN_HELSING);
	new->playerID[3].location.id = GvGetPlayerLocation(new, PLAYER_MINA_HARKER);
	new->playerID[4].location.id = GvGetPlayerLocation(new, PLAYER_DRACULA);

	/*
	new->playerID[0].health = GvGetHealth(new, PLAYER_LORD_GODALMING);
	new->playerID[1].health = GvGetHealth(new, PLAYER_DR_SEWARD);
	new->playerID[2].health = GvGetHealth(new, PLAYER_VAN_HELSING);
	new->playerID[3].health = GvGetHealth(new, PLAYER_MINA_HARKER);
	new->playerID[4].health = GvGetHealth(new, PLAYER_DRACULA);
	*/

	new->playerID[0].health = GAME_START_HUNTER_LIFE_POINTS;
	new->playerID[1].health = GAME_START_HUNTER_LIFE_POINTS;
	new->playerID[2].health = GAME_START_HUNTER_LIFE_POINTS;
	new->playerID[3].health = GAME_START_HUNTER_LIFE_POINTS;
	new->playerID[4].health = GAME_START_BLOOD_POINTS;

	// getting the current score of the game
	/*
	new->score = GvGetScore(new);
	*/
	new->score = GAME_START_SCORE;
	// getting the current player
	new->currPlayer = GvGetPlayer(new);
	// getting traps on the map and storing it in gv struct
	// PlaceId *TrapLocs = GvGetTrapLocations(new, 0);
	// int i = 0;
	// while (TrapLocs[i] != '\0') {
	// 	new->trapLocations[i].id = TrapLocs[i];
	// 	i++;
	// }
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

int GvGetScore(GameView gv)
{
	gv->score = GAME_START_SCORE;
	// Check all ways scores can be reduced
	// Each round that goes by that Dracula is alive
	gv->score -= gv->round * SCORE_LOSS_DRACULA_TURN;
	// Hunters dying
	for (int j = 0; j < NUM_PLAYERS - 1; j++) {
		if (GvGetHealth(gv, gv->playerID[j].name) <= 0) gv->score -= SCORE_LOSS_HUNTER_HOSPITAL;
	} 
	// A vampire matures
	for (int i = 0; gv->playString[i] != '\0'; i += TURN_CHARS) {
		// Check for Dracula move
		if (gv->playString[i] == 'D') {
			// Check for the 'V' character
			if (gv->playString[i + 5] == IMMATURE_VAMPIRE) gv->score -= SCORE_LOSS_VAMPIRE_MATURES; 
		}
		if (gv->playString[i + TURN_CHARS - 1] == '\0') break;
	}
	return gv->score;
}

int GvGetHealth(GameView gv, Player player)
{
	// Set all the hunters health points
	for (int i = 0; i < NUM_PLAYERS; i++) {
		if (i != PLAYER_DRACULA) {
			gv->playerID[i].health = GAME_START_HUNTER_LIFE_POINTS;
		} else {
			gv->playerID[i].health = GAME_START_BLOOD_POINTS;
		}
	}
	// Return the health of the given player
	// Extract intial of player
	int playerID = player;
	if (playerID == 0) playerID = 'G';
	if (playerID == 1) playerID = 'S';
	if (playerID == 2) playerID = 'H';
	if (playerID == 3) playerID = 'M';
	if (playerID == 4) playerID = 'D';
	bool playerDead = false;
	// Traverse through playString round to find 'G', 'S', 'H' or 'M'
	for (int i = 0; gv->playString[i] != '\0'; i += TURN_CHARS) {
		// Check the hunter's POV
		if (gv->playString[i] == playerID && playerID != 'D') {
			// Check the last four characters to see if anything has happened
			for (int j = 3; j < 7; j++) {
				if (gv->playString[i + j] == TRAP) {
					gv->playerID[player].health -= LIFE_LOSS_TRAP_ENCOUNTER;
				} else if (gv->playString[i + j] == 'D') {
					gv->playerID[player].health -= LIFE_LOSS_DRACULA_ENCOUNTER;
				}
				if (gv->playerID[player].health <= 0) {
					gv->playerID[player].health = 0;
					// The player is dead and is teleported to the hospital
					gv->playerID[player].location.id = HOSPITAL_PLACE;
					playerDead = true;
					break;
				}
			}
			if (playerDead == true) break;
			// Check if the hunter rested
			if (gv->round != 0) {
				if (gv->playString[i + 1] == gv->playString[i + 1 - ROUND_CHARS]) {
					if (gv->playString[i + 2] == gv->playString[i + 2 - ROUND_CHARS]) {
						// This means the hunter stayed in the same location
						gv->playerID[player].health += LIFE_GAIN_REST;
					}
				}
				// Cap the hunter's health at 9 if they get above it
				if (gv->playerID[player].health > GAME_START_HUNTER_LIFE_POINTS) gv->playerID[player].health = GAME_START_HUNTER_LIFE_POINTS;
			}
		} 
		// Check Dracula's POV
		if (gv->playString[i] == playerID && playerID == 'D') {
			// Check if Dracula encountered a hunter
			for (int j = 0; j < 30; j++) {
				if (gv->playString[i - 30 + j] == 'D') {
					gv->playerID[player].health -= LIFE_LOSS_HUNTER_ENCOUNTER;
				}
			}
			// Check if Dracula TP to his castle
			gv->playerID[player].location.id = GvGetPlayerLocation(gv, player);
			if (gv->playerID[player].location.id == TELEPORT || gv->playerID[player].location.id == CASTLE_DRACULA) {
				gv->playerID[player].health += LIFE_GAIN_CASTLE_DRACULA;
			// Check if Dracula is at sea
			} else if (placeIsSea(gv->playerID[player].location.id) == true) {
				gv->playerID[player].health -= LIFE_LOSS_SEA;
			}
			// Check if Dracula is dead
			if (gv->playerID[player].health <= 0) {
				// Dracula is dead (game is over)
				return gv->playerID[player].health;
			}
		}
		if (gv->playString[i + TURN_CHARS - 1] == '\0') break;
	}
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

	// return Hospital if dead
	if (player != PLAYER_DRACULA) {
		if (player == PLAYER_DR_SEWARD && 
			GvGetHealth(gv, PLAYER_DR_SEWARD) == 0) {
			return HOSPITAL_PLACE;
		} else if (player == PLAYER_VAN_HELSING && 
			GvGetHealth(gv, PLAYER_VAN_HELSING) == 0) {
			return HOSPITAL_PLACE;
		} else if (player == PLAYER_MINA_HARKER && 
			GvGetHealth(gv, PLAYER_MINA_HARKER) == 0) {
			return HOSPITAL_PLACE;
		} else if (player == PLAYER_LORD_GODALMING && 
			GvGetHealth(gv, PLAYER_LORD_GODALMING) == 0) {
			return HOSPITAL_PLACE;
		}
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

	Place playerLoc;
	char playerPlace[3];
	char placeAbbrev[3];
	Place immvampireLoc;
	bool foundLocation = false;
	playerLoc.abbrev = playerPlace;
	immvampireLoc.abbrev = placeAbbrev;

	// finding immvampires location
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

	// immvampire not found
	if (foundLocation == false) return NOWHERE;

	// checks if hunter has been in the same place as the
	// immvampire for the last 6 rounds and kills the vampire if yes
	// therefore, location of immvampire is NOWHERE as he is dead
	for (int i = 0; gv->playString[i] != '\0'; i++) {
		
		// need to fix this dual-condition thing
		if (gv->playString[i] == 'G' || gv->playString[i] == 'S' ||
            gv->playString[i] == 'H' || gv->playString[i] == 'M') {

			// obtain two initials of place
			playerLoc.abbrev[0] = gv->playString[i + 1];
			playerLoc.abbrev[1] = gv->playString[i + 2];
			playerLoc.abbrev[2] = '\0';

			// get placeID
			playerLoc.id = placeAbbrevToId(playerLoc.abbrev);

			// immvampire encountered and killed instantly
			if (playerLoc.id == immvampireLoc.id) {
				if (GvGetRound(gv) < 6) return NOWHERE;
			}
			//playerNum++;
			i += POS_ACTIONS;
		}
	}

	gv->imvampireLocation.id = immvampireLoc.id;
	return gv->imvampireLocation.id;
}

PlaceId *GvGetTrapLocations(GameView gv, int *numTraps)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION
	PlaceId *trapLocations = malloc(sizeof(PlaceId) * (MAX_TRAP_LOCATIONS));
	// Number of traps in playStrings ('T')
	*numTraps = 0;
	// String to hold location initials
	Place traps;
	char placeAbbrev[3];
	traps.abbrev = placeAbbrev;
	// Counter for location storage
	int counterLocation = 0;
	// Transverses through the stirng at the position 'D'
	for (int i = 0; gv->playString[i] != '\0'; i += TURN_CHARS) {
		// Check every D for a 'T'
		if (gv->playString[i] == 'D') {
			if (gv->playString[i + 3] == TRAP) {
				// Increase the number of traps
				++*numTraps;
				// Extract the location name
				traps.abbrev[0] = gv->playString[i + 1];
				traps.abbrev[1] = gv->playString[i + 2];
				traps.abbrev[2] = '\0';
				// Convert the abbrev to name
				traps.id = placeAbbrevToId(traps.abbrev);
				// Store the location in the array
				trapLocations[counterLocation] = traps.id;
				counterLocation++;
			}
			// Decrease the numTraps if the trap has expired
			if (gv->playString[i + 5] == TRAP_EXPIRED) {
				--*numTraps;
				// Remove the oldest location (first element)
				for (int j = 1; j < counterLocation; j++) {
					trapLocations[j - 1] = trapLocations[j];
				}
			}
		// Means it is a hunter and check if they stepped on a trap
		} else {
			// They stepped on a trap
			if (gv->playString[i + 3] == TRAP) {
				// Remove it
				--*numTraps;
				// Extract the location name
				traps.abbrev[0] = gv->playString[i + 1];
				traps.abbrev[1] = gv->playString[i + 2];
				traps.abbrev[2] = '\0';
				// Convert the abbrev to name
				traps.id = placeAbbrevToId(traps.abbrev);
				// Find what position the trap is in and remove it
				int j;
				for (j = 0; j < *numTraps; j++) {
					if (trapLocations[j] == traps.id) break;
				}
				// If at the start of array
				if (j == 0) {
					for (int k = 1; k < counterLocation; k++) trapLocations[k - 1] = trapLocations[k];
				// It is in the middle or end
				} else {
					for (int k = 0; k < *numTraps; k++) trapLocations[k] = trapLocations[k+ 1];
				}
			}
		}
		if (gv->playString[i + TURN_CHARS - 1] == '\0') break;
	}
	// Variable to store last known location of the trap
	return trapLocations;
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
	int nMoves = gv->round;
	if (player < gv->currPlayer) nMoves++; 
	PlaceId *moves = malloc(sizeof(PlaceId) * (nMoves));

	int i;
	for (i = 0; i < nMoves; i++) {
		Place curr = getPlaceId(gv, player, i);
		moves[i] = curr.id;
	}
	*numReturnedMoves = i;

	return moves;
}

PlaceId *GvGetLastMoves(GameView gv, Player player, int numMoves,
                        int *numReturnedMoves, bool *canFree)
{
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
	int nMoves = gv->round;
	// Case where the player is a hunter, this function should behave exactly
	// the same as GvGetMoveHistory.
	if (player != PLAYER_DRACULA) {
		return GvGetMoveHistory(gv, player, numReturnedLocs, canFree);
	} else {
		if (player < gv->currPlayer) nMoves++; 
		PlaceId *moves = malloc(sizeof(PlaceId) * (nMoves));

		int i;
		for (i = 0; i < nMoves; i++) {
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
							 int visited[], PlaceId from, int nElement,
							 Round round, Player player) {

	Queue railLocs = newQueue();

	// Add all rail transport from the starting point to the queue
	for (int i = 0; i < nElement; i++) {
		visited[reachable[i].p] = from;
		QueueJoin(railLocs, reachable[i].p);
	}

	int railDist = (round + player) % 4;
	if (railDist == 0) return 0;

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
				if (nextCity->type == RAIL
				&& visited[nextCity->p] == UNINTIALISED) {
					// Visit cities that can be access by rail and add to queue.
					reachable[nElement].p = nextCity->p;
					visited[nextCity->p] = currCity;
					QueueJoin(railLocs, nextCity->p);
					nElement++;
				}
			}
		}
	}
	return nElement;
}

PlaceId *GvGetReachable(GameView gv, Player player, Round round,
                        PlaceId from, int *numReturnedLocs)
{
	// get availiable connections
	ConnList startReached = MapGetConnections(gv->map, from);
	struct connNode reachable[gv->nPlaces];

	// Initialise visited array
	int visited[gv->nPlaces];
	for (int i = 0; i < gv->nPlaces; i++) {
		visited[i] = UNINTIALISED;
	}
	visited[from] = from;	

	int nElements = 0;
	// If player is a hunter, consider rail moves
	if (player != PLAYER_DRACULA) {
		// Go through startReached list and store/visit cities that can be
		// visited with rail moves.
		for (ConnList curr = startReached; curr != NULL; curr = curr->next) {
			if (visited[curr->p] == UNINTIALISED && curr->type == RAIL) {
				reachable[nElements].p = curr->p;
				visited[curr->p] = from;
				nElements++;
			}
		}
		nElements = findValidRailMove(gv, reachable, visited, from,
										nElements, round, player);
		// Add all reachable locations from starting point (`from`) that has
		// not been visited already.
		for (ConnList curr = startReached; curr != NULL; curr = curr->next) {
			if (visited[curr->p] == UNINTIALISED) {
				reachable[nElements].p = curr->p;
				visited[curr->p] = from;
				nElements++;
			}
		}
	}
	// Otherwise, moves of dracula must not be `HOSPITAL_PLACE` and rail moves.
	else {
		for (ConnList curr = startReached; curr != NULL; curr = curr->next) {
			if (curr->p != HOSPITAL_PLACE && curr->type != RAIL
			&& visited[curr->p] == UNINTIALISED) {
				reachable[nElements].p = curr->p;
				visited[curr->p] = from;
				nElements++;
			}
		}
	}

	// Include starting location as a valid move
	reachable[nElements].p = from;
	nElements++;
	*numReturnedLocs = nElements;

	// Copies placeID from reachable and store it in a new array
	PlaceId *reachableID = malloc(sizeof(PlaceId) * nElements);
	for (int j = 0; j < nElements; j++) {
		reachableID[j] = reachable[j].p;
	}

	return reachableID;
}

PlaceId *GvGetReachableByType(GameView gv, Player player, Round round,
                              PlaceId from, bool road, bool rail,
                              bool boat, int *numReturnedLocs)
{
	// TODO: REPLACE THIS WITH YOUR OWN IMPLEMENTATION

	// Basically this function considers the connection type based on the 
	// bools inputted into the function. e.g. if road & rail = true, then
	// only include the moves available by road & rail.	
	if (road && boat && rail) {
		PlaceId *reached = GvGetReachable(gv, player, round, from, numReturnedLocs);
		return reached;
	} 
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
		visited[i] = UNINTIALISED;
	}
	visited[from] = from;	

	int i = 0;
	// If player is a hunter, consider rail moves
	if (player != PLAYER_DRACULA) {
		ConnList curr = startReached;
		// Getting all the rail connections
		if (rail) {
			while (curr != NULL) {
				if (visited[curr->p] == UNINTIALISED && curr->type == RAIL) {
					reachable[i].p = curr->p;
					visited[curr->p] = from;
					i++;
				}
				curr = curr->next;
			}
			i = findValidRailMove(gv, reachable, visited, from,
										i, round, player);
		}
		// getting all the road connections
		curr = startReached;
		if (road) {
			while (curr != NULL) {
				if (visited[curr->p] == -1 && curr->type == ROAD) {
					reachable[i].p = curr->p;
					reachable[i].type = curr->type;
					i++;
				}
				curr = curr->next;
			}
		}
		// getting all the sea connections
		curr = startReached;
		if (boat) {
			while (curr != NULL) {
				if (visited[curr->p] == -1 && curr->type == BOAT) {
					reachable[i].p = curr->p;
					reachable[i].type = curr->type;
					i++;
				}
				curr = curr->next;
			}
		}
	} else {
		// Otherwise, dracula (cannot move to hospital, or via rail)
		if (road) {
			ConnList curr = startReached;
			while (curr != NULL) {
				if (curr->p != HOSPITAL_PLACE && curr->type == ROAD
				&& visited[curr->p] == UNINTIALISED) {
					reachable[i].p = curr->p;
					visited[curr->p] = from;
					i++;
				}
				curr = curr->next;
			}
		}
		// getting boat connections
		
		if (boat) {
			ConnList curr = startReached;
			while (curr != NULL) {
				if (curr->p != HOSPITAL_PLACE && curr->type == BOAT
				&& visited[curr->p] == UNINTIALISED) {
					reachable[i].p = curr->p;
					visited[curr->p] = from;
					i++;
				}
				curr = curr->next;
			}
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
	return 0;
}

////////////////////////////////////////////////////////////////////////
// Your own interface functions

// TODO
