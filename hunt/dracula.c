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
	}
}
