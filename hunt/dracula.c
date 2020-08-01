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

#define DONT -1

typedef struct hunter
{
	Player name;						// name of player
	PlaceId location;					// current location of player
	int health;							// current player health
} hunterInfo;

typedef struct gameState {
	Round currRound;
	int score;
	hunterInfo hunterID[4];
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

	// starting position
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
////////////////////////////////////////////////////////////////////////////////
//							     Basic Movement							      //
////////////////////////////////////////////////////////////////////////////////

	// avoiding moving to cities where the hunters can also move
	int numLocG = 0;
	int numLocS = 0;
	int numLocH = 0;
	int numLocM = 0;
	PlaceId *God = DvWhereCanTheyGo(dv, PLAYER_LORD_GODALMING, &numLocG);
	PlaceId *Doc = DvWhereCanTheyGo(dv, PLAYER_DR_SEWARD, &numLocS);
	PlaceId *Van = DvWhereCanTheyGo(dv, PLAYER_VAN_HELSING, &numLocH);
	PlaceId *Min = DvWhereCanTheyGo(dv, PLAYER_MINA_HARKER, &numLocM);

	int totalLoc = numLocG + numLocS + numLocH + numLocM;

	// appending all individual hunters possible city moves into one array
	PlaceId *allHunterMoves = malloc(sizeof(PlaceId) * totalLoc);
	int i;
	for (i = 0; i < numLocG; i++) {
		allHunterMoves[i] = God[i];
	}
	for (int j = 0; j < numLocS; j++, i++) {
		allHunterMoves[i] = Doc[j]; 
	}
	for (int j = 0; j < numLocH; j++, i++) {
		allHunterMoves[i] = Van[j];
	}
	for (int j = 0; j < numLocM; j++, i++) {
		allHunterMoves[i] = Min[j];
	}

	// getting the locs dracula can move
	int numLocD = 0;
	PlaceId *Drac = DvWhereCanIGo(dv, &numLocD);
	int numBadLocs = 0;
	for (int i = 0; i < numLocD; i++) {
		// comparing draculas moves to moves possible by all hunters
		for (int j = 0; j < totalLoc; j++) {
			if (allHunterMoves[j] == Drac[i]) {
				Drac[i] = DONT;
				numBadLocs++;
				break;
			}
		}
	}
	// creating a new array for where dracula should go to avoid hunters
	int numGoodLocs = numLocD - numBadLocs;
	PlaceId *DracShouldGo = malloc(sizeof(PlaceId) * numGoodLocs);
	int j = 0;
	for (int i = 0; i < numLocD; i++) {
		if (Drac[i] != DONT) {
			DracShouldGo[j] = Drac[i];
			j++;
		}
	}

	// randomly going to any of the cities in DracShouldGo
	// (there is probably a better strategy, but can build off this for now)

	int locID = (rand() % (numGoodLocs - 1)) - 1;
	play = (char *) placeIdToAbbrev(DracShouldGo[locID]);
	registerBestPlay(play, "BYE BYE BUDDY!!!!");

////////////////////////////////////////////////////////////////////////////////
//							     ~ TODO ~						              //
////////////////////////////////////////////////////////////////////////////////

	// checking if a hunter has found draculas trail, run away

	// 


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

}
