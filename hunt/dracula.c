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
#include <stdio.h>

typedef struct hunter
{
	PlaceId location;					// current location of player
	int health;							// current player health
} hunterInfo;

typedef struct gameState {
	Round currRound;
	int score;
	hunterInfo hunterID[4];

	int dracHealth;
	PlaceId dracLoc;
} GameState;


void decideDraculaMove(DraculaView dv)
{
	char *play = "HI";
	Round currRound = DvGetRound(dv);
	// int score = DvGetScore(dv);

	GameState gameInfo;

	gameInfo.currRound = DvGetRound(dv);
	gameInfo.score = DvGetScore(dv);
	

	gameInfo.hunterID[0].location = DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING);
	gameInfo.hunterID[1].location = DvGetPlayerLocation(dv, PLAYER_DR_SEWARD);
	gameInfo.hunterID[2].location = DvGetPlayerLocation(dv, PLAYER_VAN_HELSING);
	gameInfo.hunterID[3].location = DvGetPlayerLocation(dv, PLAYER_MINA_HARKER);

	gameInfo.hunterID[0].health = DvGetHealth(dv, PLAYER_LORD_GODALMING);
	gameInfo.hunterID[1].health = DvGetHealth(dv, PLAYER_DR_SEWARD);
	gameInfo.hunterID[2].health = DvGetHealth(dv, PLAYER_VAN_HELSING);
	gameInfo.hunterID[3].health = DvGetHealth(dv, PLAYER_MINA_HARKER);

	gameInfo.dracHealth = DvGetHealth(dv, PLAYER_DRACULA);
	gameInfo.dracLoc = DvGetPlayerLocation(dv, PLAYER_DRACULA);

	if (gameInfo.dracLoc == NOWHERE) {
		registerBestPlay("CD", "Mwahahahaha");
	} else{
		if (currRound == 0) {
		play = "TP";
	}

	for (int i = 0; i < 4; i++) {
		printf("Hunter %d:\n", i + 1);
		printf("\t Location: %d\n", gameInfo.hunterID[i].location);
		printf("\t Health:   %d\n", gameInfo.hunterID[i].health);
	}
	printf("Round: %d\n", gameInfo.currRound);
	printf("Score: %d\n", gameInfo.score);

	registerBestPlay(play, "Mwahahahaha");

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

}
