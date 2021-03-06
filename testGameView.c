////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// testGameView.c: test the GameView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-01	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v1.2	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

// Written in the month of July 2020 for COMP2521 T2 Assignment 2.

// This is a testing file to determine if the implementation for GameView.h
// ADT is correct and functional. Test cases were both supplied from the given
// code and written by Gabriel Ting, Christian Ilagan, Tam Do and Prathamesh 
// Jagtap.

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Game.h"
#include "GameView.h"
#include "Places.h"
#include "testUtils.h"


int main(void)
{
	printf("********************** GameView Testing **********************\n");
	printf("Key: (Given)	-> Supplied test case intially\n");
	printf("     (Added)	-> New asserts has been added to the original test case\n");
	printf("     (New)	-> New test case created by team\n");

	printf("\n================= Game State Information =================\n");
	printf("===> Test Cases\n");
	{///////////////////////////////////////////////////////////////////	
		printf("(Added)	-> Basic initialisation: ");

		char *trail = "";
		Message messages[] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 0);
		assert(GvGetPlayer(gv) == PLAYER_LORD_GODALMING);
		assert(GvGetScore(gv) == GAME_START_SCORE);
		assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) == 
								GAME_START_HUNTER_LIFE_POINTS);
		assert(GvGetHealth(gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == NOWHERE);
		assert(GvGetVampireLocation(gv) == NOWHERE);

		{
			int nMoves = -1; bool canFree = true;
			PlaceId *moves = GvGetMoveHistory(gv, PLAYER_LORD_GODALMING, 
												&nMoves, &canFree);
			assert(nMoves == 0);
			if (canFree) free(moves);
		}
		{
			int nMoves = -1; bool canFree = true;
			PlaceId *moves = GvGetMoveHistory(gv, PLAYER_DRACULA, &nMoves, 
												&canFree);
			assert(nMoves == 0);
			if (canFree) free(moves);
		}
		{
			int lastNMoves = 10;
			int nMoves = -1; bool canFree = true;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_LORD_GODALMING, 
												lastNMoves, &nMoves, &canFree);
			assert(nMoves == 0);
			if (canFree) free(moves);
		}
		{
			int lastNMoves = 10;
			int nMoves = -1; bool canFree = true;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_DRACULA, 
												lastNMoves, &nMoves, &canFree);
			assert(nMoves == 0);
			if (canFree) free(moves);
		}
		{
			int nMoves = -1; bool canFree = true;
			PlaceId *moves = GvGetLocationHistory(gv, PLAYER_DRACULA, &nMoves, 
												&canFree);
			assert(nMoves == 0);
			if (canFree) free(moves);
		}
		{
			int lastNMoves = 10;
			int nMoves = -1; bool canFree = true;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_DRACULA, 
												lastNMoves, &nMoves, &canFree);
			assert(nMoves == 0);
			if (canFree) free(moves);
		}

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(Given)	-> After Lord Godalming's turn: ");

		char *trail =
			"GST....";
		
		Message messages[1] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 0);
		assert(GvGetPlayer(gv) == PLAYER_DR_SEWARD);
		assert(GvGetScore(gv) == GAME_START_SCORE);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(GvGetPlayerLocation(gv, PLAYER_DR_SEWARD) == NOWHERE);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("(Given)	-> After Mina Harker's turn: ");

		char *trail =
			"GST.... SAO.... HZU.... MBB....";
		
		Message messages[4] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 0);
		assert(GvGetPlayer(gv) == PLAYER_DRACULA);
		assert(GvGetScore(gv) == GAME_START_SCORE);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(GvGetPlayerLocation(gv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(GvGetPlayerLocation(gv, PLAYER_VAN_HELSING) == ZURICH);
		assert(GvGetPlayerLocation(gv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == NOWHERE);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(Given)	-> After Dracula's turn: ");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DC?.V..";
		
		Message messages[5] = {
			"Hello", "Goodbye", "Stuff", "...", "Mwahahahaha"
		};
		
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 1);
		assert(GvGetPlayer(gv) == PLAYER_LORD_GODALMING);
		assert(GvGetScore(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN);
		assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) == 
								GAME_START_HUNTER_LIFE_POINTS);
		assert(GvGetHealth(gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(GvGetPlayerLocation(gv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(GvGetPlayerLocation(gv, PLAYER_VAN_HELSING) == ZURICH);
		assert(GvGetPlayerLocation(gv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(GvGetVampireLocation(gv) == CITY_UNKNOWN);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(Given)	-> Encountering Dracula: ");

		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.V.. "
			"GGEVD..";
		
		Message messages[6] = {
			"Hello", "Goodbye", "Stuff", "...", "Mwahahahaha",
			"Aha!"
		};
		
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) ==
				GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER);
		assert(GvGetHealth(gv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS - LIFE_LOSS_HUNTER_ENCOUNTER);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == GENEVA);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == GENEVA);
		assert(GvGetVampireLocation(gv) == NOWHERE);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(Given)	-> Test for Dracula doubling back at sea, "
		       "and losing blood points (Hunter View): ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DS?.... "
			"GST.... SST.... HST.... MST.... DD1....";
		
		Message messages[10] = {
			"Party at Geneva", "Okay", "Sure", "Let's go", "Mwahahahaha",
			"", "", "", "", "Back I go"
		};
		
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 2);
		assert(GvGetPlayer(gv) == PLAYER_LORD_GODALMING);
		assert(GvGetScore(gv) == GAME_START_SCORE - 2 * SCORE_LOSS_DRACULA_TURN);
		assert(GvGetHealth(gv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS - (2 * LIFE_LOSS_SEA));
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == SEA_UNKNOWN);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}


	{///////////////////////////////////////////////////////////////////
	
		printf("(Given)	-> Test for Dracula doubling back at sea, "
			   "and losing blood points (Dracula View): ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DEC.... "
			"GST.... SST.... HST.... MST.... DD1.... "
			"GFR.... SFR.... HFR.... MFR....";
		
		Message messages[14] = {
			"Hello", "Rubbish", "Stuff", "", "Mwahahah",
			"Aha!", "", "", "", "Back I go"};
		
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 2);
		assert(GvGetPlayer(gv) == PLAYER_DRACULA);
		assert(GvGetScore(gv) == GAME_START_SCORE - 2 * SCORE_LOSS_DRACULA_TURN);
		assert(GvGetHealth(gv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS - (2 * LIFE_LOSS_SEA));
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == ENGLISH_CHANNEL);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("(Given)	-> Checking that hunters' health points are capped: ");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V.. "
			"GGE....";
	
		Message messages[6] = {};
		GameView gv = GvNew(trail, messages);
	
		assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) == 
						GAME_START_HUNTER_LIFE_POINTS);
		
		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("(Added)	-> Testing a hunter 'dying': ");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V.. "
			"GGE.... SGE.... HGE.... MGE.... DSTT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "
			"GSTTTTD";
		
		Message messages[21] = {};
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetScore(gv) == GAME_START_SCORE
		                         - 4 * SCORE_LOSS_DRACULA_TURN
		                         - SCORE_LOSS_HUNTER_HOSPITAL);
		assert(GvGetHealth(gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS
												  - LIFE_LOSS_HUNTER_ENCOUNTER);
		assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) == 0);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == HOSPITAL_PLACE);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == STRASBOURG);

		int numTraps = 0;
		PlaceId *traps = GvGetTrapLocations(gv, &numTraps);
		assert(numTraps == 0);

		free(traps);
		
		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("(Given)	-> Testing Dracula doubling back to Castle Dracula: ");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DCD.V.. "
			"GGE.... SGE.... HGE.... MGE.... DD1T...";
		
		Message messages[10] = {};
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetHealth(gv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS + (2 * LIFE_GAIN_CASTLE_DRACULA));
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CASTLE_DRACULA);
		
		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("(Given)	-> Testing vampire/trap locations: ");
		
		char *trail =
			"GVI.... SGE.... HGE.... MGE.... DCD.V.. "
			"GBD.... SGE.... HGE.... MGE.... DKLT... "
			"GSZ.... SGE.... HGE.... MGE.... DGAT... "
			"GSZ.... SGE.... HGE.... MGE....";
		
		Message messages[19] = {};
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == GALATZ);
		assert(GvGetVampireLocation(gv) == CASTLE_DRACULA);
		int numTraps = 0;
		PlaceId *traps = GvGetTrapLocations(gv, &numTraps);
		assert(numTraps == 2);
		sortPlaces(traps, numTraps);
		assert(traps[0] == GALATZ && traps[1] == KLAUSENBURG);
		free(traps);
		
		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("(Given)	-> Testing trap locations after one is destroyed: ");
		
		char *trail =
			"GVI.... SGE.... HGE.... MGE.... DBC.V.. "
			"GBD.... SGE.... HGE.... MGE.... DKLT... "
			"GSZ.... SGE.... HGE.... MGE.... DGAT... "
			"GBE.... SGE.... HGE.... MGE.... DCNT... "
			"GKLT... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) ==
				GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_TRAP_ENCOUNTER);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == KLAUSENBURG);
		assert(GvGetVampireLocation(gv) == BUCHAREST);
		int numTraps = 0;
		PlaceId *traps = GvGetTrapLocations(gv, &numTraps);
		assert(numTraps == 2);
		sortPlaces(traps, numTraps);
		assert(traps[0] == CONSTANTA && traps[1] == GALATZ);
		free(traps);
		
		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("(Given)	-> Testing a vampire maturing: ");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V.. "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T.V.";
		
		Message messages[35] = {};
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetScore(gv) == GAME_START_SCORE
		                         - 7 * SCORE_LOSS_DRACULA_TURN
		                         - SCORE_LOSS_VAMPIRE_MATURES);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(GvGetVampireLocation(gv) == NOWHERE);
		
		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////

		printf("(New)	-> Get round 1: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V.. "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T.V.";

		Message messages[35] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 7);
		assert(GvGetPlayer(gv) == PLAYER_LORD_GODALMING);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////

		printf("(New)	-> Get round 2: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V.. "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE....";

		Message messages[19] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 3);
		assert(GvGetPlayer(gv) == PLAYER_DRACULA);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////

		printf("(New)	-> Get player 1: ");

		char *trail =
			"GGE....";

		Message messages[1] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 0);
		assert(GvGetPlayer(gv) == PLAYER_DR_SEWARD);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////

		printf("(New)	-> Get player 2: ");

		char *trail =
			"GGE.... SGE....";

		Message messages[2] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 0);
		assert(GvGetPlayer(gv) == PLAYER_VAN_HELSING);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////

		printf("(New)	-> Get player 3: ");

		char *trail =
			"GGE.... SGE.... HGE....";

		Message messages[3] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 0);
		assert(GvGetPlayer(gv) == PLAYER_MINA_HARKER);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////

		printf("(New)	-> Get player 4: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE....";

		Message messages[4] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 0);
		assert(GvGetPlayer(gv) == PLAYER_DRACULA);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////

		printf("(New)	-> Get player 5: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V..";

		Message messages[5] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 1);
		assert(GvGetPlayer(gv) == PLAYER_LORD_GODALMING);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CITY_UNKNOWN);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////

		printf("(New)	-> DOUBLE_BACK_1: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V.. "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DAST... "
			"GGE.... SGE.... HGE.... MGE.... DD1..V.";

		Message messages[35] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 7);
		assert(GvGetPlayer(gv) == PLAYER_LORD_GODALMING);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == ADRIATIC_SEA);
		assert(GvGetVampireLocation(gv) == NOWHERE);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

	}

	{///////////////////////////////////////////////////////////////////

		printf("(New)	-> DOUBLE_BACK_2: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V.. "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DBBT... "
			"GGE.... SGE.... HGE.... MGE.... DAST... "
			"GGE.... SGE.... HGE.... MGE.... DD2..V.";

		Message messages[35] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 7);
		assert(GvGetPlayer(gv) == PLAYER_LORD_GODALMING);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == BAY_OF_BISCAY);
		assert(GvGetVampireLocation(gv) == NOWHERE);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

	}

	{///////////////////////////////////////////////////////////////////

		printf("(New)	-> DOUBLE_BACK_3: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V.. "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DCFT... "
			"GGE.... SGE.... HGE.... MGE.... DBBT... "
			"GGE.... SGE.... HGE.... MGE.... DAST... "
			"GGE.... SGE.... HGE.... MGE.... DD3..V.";

		Message messages[35] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 7);
		assert(GvGetPlayer(gv) == PLAYER_LORD_GODALMING);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CLERMONT_FERRAND);
		assert(GvGetVampireLocation(gv) == NOWHERE);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

	}

	{///////////////////////////////////////////////////////////////////

		printf("(New)	-> DOUBLE_BACK_4: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V.. "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DEDT... "
			"GGE.... SGE.... HGE.... MGE.... DCFT... "
			"GGE.... SGE.... HGE.... MGE.... DBBT... "
			"GGE.... SGE.... HGE.... MGE.... DAST... "
			"GGE.... SGE.... HGE.... MGE.... DD4..V.";

		Message messages[35] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 7);
		assert(GvGetPlayer(gv) == PLAYER_LORD_GODALMING);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == EDINBURGH);
		assert(GvGetVampireLocation(gv) == NOWHERE);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

	}

	{///////////////////////////////////////////////////////////////////

		printf("(New)	-> DOUBLE_BACK_5: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V.. "
			"GGE.... SGE.... HGE.... MGE.... DFRT... "
			"GGE.... SGE.... HGE.... MGE.... DEDT... "
			"GGE.... SGE.... HGE.... MGE.... DCFT... "
			"GGE.... SGE.... HGE.... MGE.... DBBT... "
			"GGE.... SGE.... HGE.... MGE.... DAST... "
			"GGE.... SGE.... HGE.... MGE.... DD5..V.";

		Message messages[35] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 7);
		assert(GvGetPlayer(gv) == PLAYER_LORD_GODALMING);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == FRANKFURT);
		assert(GvGetVampireLocation(gv) == NOWHERE);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

	}

	{///////////////////////////////////////////////////////////////////

		printf("(New)	-> DOUBLE_BACK_SPECIAL: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DGR.V.. "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DAST... "
			"GGE.... SGE.... HGE.... MGE.... DD5..V.";

		Message messages[35] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 7);
		assert(GvGetPlayer(gv) == PLAYER_LORD_GODALMING);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == GRANADA);
		assert(GvGetVampireLocation(gv) == NOWHERE);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

	}

	{///////////////////////////////////////////////////////////////////

		printf("(New)	-> Get immvampire location when dead: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DZU.V.. "
			"GGE.... SGE.... HGE.... MZU.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T...";
			
		Message messages[20] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 4);
		assert(GvGetPlayer(gv) == PLAYER_LORD_GODALMING);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(GvGetVampireLocation(gv) == NOWHERE);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

	}

	{///////////////////////////////////////////////////////////////////

		printf("(New)	-> Get immvampire location when dead 2: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DZU.V.. "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MZU.... DC?T... "
			"GGE.... SGE.... HGE.... MZU.... DC?T...";
			
		Message messages[20] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 4);
		assert(GvGetPlayer(gv) == PLAYER_LORD_GODALMING);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(GvGetVampireLocation(gv) == NOWHERE);

		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Testing everything: ");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DPA.V.. "
			"GGE.... SGE.... HGE.... MGE.... DSTT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "
			"GSTTTTD SGE.... HGE.... MGE.... DTPT... "
			"GSZ.... SGE.... HGE.... MGE.... DCDT... "
			"GSZ.... SGE.... HGE.... MGE.... DCDT.V.";
		
		Message messages[35] = {};
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetScore(gv) == GAME_START_SCORE
		                         - 7 * SCORE_LOSS_DRACULA_TURN
		                         - SCORE_LOSS_VAMPIRE_MATURES
								 - SCORE_LOSS_HUNTER_HOSPITAL);
		assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) 
						      == GAME_START_HUNTER_LIFE_POINTS);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CASTLE_DRACULA);
		int numTraps = 0;
		PlaceId *traps = GvGetTrapLocations(gv, &numTraps);
		assert(numTraps == 3);
		sortPlaces(traps, numTraps);
		assert(traps[0] == CASTLE_DRACULA && traps[1] 
						== CASTLE_DRACULA && traps[2] 
						== CASTLE_DRACULA);
		free(traps);
		assert(GvGetVampireLocation(gv) == NOWHERE);
		
		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Testing hunter is killed by one trap but there are more traps: ");
		
		char *trail =
			"GPA.... SPA.... HPA.... MPA.... DST.V.. "
			"GSTVD.. SPA.... HPA.... MPA.... DZUT... "
			"GGE.... SPA.... HPA.... MPA.... DMIT... "
			"GZUT... SPA.... HPA.... MPA.... DVET... "
			"GMIT... SPA.... HPA.... MPA.... DHIT... "
			"GVET... SPA.... HPA.... MPA.... DAS....";
		
		Message messages[30] = {};
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetRound(gv) == 6);
		assert(GvGetScore(gv) == 354);
		assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) == 0);
		assert(GvGetHealth(gv, PLAYER_DR_SEWARD) == 9);
		assert(GvGetHealth(gv, PLAYER_VAN_HELSING) == 9);
		assert(GvGetHealth(gv, PLAYER_MINA_HARKER) == 9);
		assert(GvGetHealth(gv, PLAYER_DRACULA) == 28);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == HOSPITAL_PLACE);
		assert(GvGetPlayerLocation(gv, PLAYER_DR_SEWARD) == PARIS);
		assert(GvGetPlayerLocation(gv, PLAYER_VAN_HELSING) == PARIS);
		assert(GvGetPlayerLocation(gv, PLAYER_MINA_HARKER) == PARIS);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == ADRIATIC_SEA);
		assert(GvGetVampireLocation(gv) == NOWHERE);
		int numTraps = 0;
		PlaceId *traps = GvGetTrapLocations(gv, &numTraps);
		assert(numTraps == 1);
		sortPlaces(traps, numTraps);
		assert(traps[0] == VENICE);
		free(traps);
		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Testing Dracula dying: ");
		
		char *trail =
			"GKL.... SKL.... HGA.... MGA.... DCD.V.. "
			"GCDVD.. SCDD... HCDD... MCDD... DKLT... "
			"GKLTD..";
		
		Message messages[11] = {};
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetRound(gv) == 2);
		assert(GvGetScore(gv) == 358);
		assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) == 0);
		assert(GvGetHealth(gv, PLAYER_DR_SEWARD) == 5);
		assert(GvGetHealth(gv, PLAYER_VAN_HELSING) == 5);
		assert(GvGetHealth(gv, PLAYER_MINA_HARKER) == 5);
		assert(GvGetHealth(gv, PLAYER_DRACULA) == 0);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == HOSPITAL_PLACE);
		assert(GvGetPlayerLocation(gv, PLAYER_DR_SEWARD) == CASTLE_DRACULA);
		assert(GvGetPlayerLocation(gv, PLAYER_VAN_HELSING) == CASTLE_DRACULA);
		assert(GvGetPlayerLocation(gv, PLAYER_MINA_HARKER) == CASTLE_DRACULA);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == KLAUSENBURG);
		assert(GvGetVampireLocation(gv) == NOWHERE);
		int numTraps = 0;
		PlaceId *traps = GvGetTrapLocations(gv, &numTraps);
		assert(numTraps == 0);
		free(traps);
		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Testing BIG CASE 1: ");
		
		char *trail =
			"GSW.... SLS.... HMR.... MHA.... DSJ.V.. "
			"GLO.... SAL.... HCO.... MBR.... DBET... "
			"GED.... SBO.... HLI.... MPR.... DKLT... "
			"GLV.... SNA.... HNU.... MBD.... DCDT... "
			"GIR.... SPA.... HPR.... MKLT... DHIT... "
			"GAO.... SST.... HSZ.... MCDTTD. DGAT... "
			"GMS.... SFL.... HKL.... MSZ.... DCNT.V. "
			"GTS.... SRO.... HBC.... MCNTD.. DBS..M. "
			"GIO.... SBI.... HCN.... MCN.... DIO.... "
			"GIO.... SAS.... HBS.... MCN.... DTS.... "
			"GTS.... SAS.... HIO.... MBS.... DMS.... "
			"GMS.... SIO.... HTS.... MIO.... DAO..M. "
			"GAO.... STS.... HMS.... MTS.... DNS.... "
			"GBB.... SMS.... HAO.... MMS.... DED.V.. "
			"GNA.... SAO.... HEC.... MAO.... DMNT... "
			"GBO.... SIR.... HLE.... MEC.... DD2T... "
			"GSR.... SDU.... HBU.... MPL.... DHIT... "
			"GSN.... SIR.... HAM.... MLO.... DTPT... "
			"GAL.... SAO.... HCO.... MEC.... DCDT... "
			"GMS.... SMS.... HFR.... MLE.... DKLT.V. "
			"GTS.... STS.... HBR.... MCO.... DGAT.M. "
			"GIO.... SIO.... HBD.... MLI.... DD3T.M. "
			"GBS.... SBS.... HKLT... MBR.... DHI..M. "
			"GCN.... SCN.... HCDTTTD MVI.... DTPT... "
			"GGAT... SGA.... HSZ.... MBC.... DCDT... "
			"GCDTTD. SCDD... HKL.... MGA.... DKLT... "
			"GSZ.... SKLTD.. HKLD... MKLD... DBC.V.. "
			"GBD.... SBE.... HGA.... MBCVD.. DSOT... "
			"GSZ.... SSOTD.. HBC.... MSOD...";
		
		Message messages[145] = {};
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetRound(gv) == 28);
		assert(GvGetScore(gv) == 282);
		assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) == 9);
		assert(GvGetHealth(gv, PLAYER_DR_SEWARD) == 3);
		assert(GvGetHealth(gv, PLAYER_VAN_HELSING) == 8);
		assert(GvGetHealth(gv, PLAYER_MINA_HARKER) == 0);
		assert(GvGetHealth(gv, PLAYER_DRACULA) == 0);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == SZEGED);
		assert(GvGetPlayerLocation(gv, PLAYER_DR_SEWARD) == SOFIA);
		assert(GvGetPlayerLocation(gv, PLAYER_VAN_HELSING) == BUCHAREST);
		assert(GvGetPlayerLocation(gv, PLAYER_MINA_HARKER) == HOSPITAL_PLACE);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == SOFIA);
		assert(GvGetVampireLocation(gv) == NOWHERE);
		int numTraps = 0;
		PlaceId *traps = GvGetTrapLocations(gv, &numTraps);
		assert(numTraps == 0);
		free(traps);
		GvFree(gv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}


	printf("\n================= Testing move/location history =================\n");
	{///////////////////////////////////////////////////////////////////
		char *trail =
			"GLS.... SLS.... HLS.... MGE.... DST.V.. "
			"GCA.... SAL.... HAL.... MGE.... DC?T... "
			"GGR.... SBO.... HBO.... MGE.... DC?T... "
			"GAL.... SNA.... HNA.... MGE.... DD3T... "
			"GSR.... SPA.... HPA.... MGE.... DHIT... "
			"GSN.... SST.... HST.... MGE.... DC?T... "
			"GMA.... SFLTTV.";
		
		Message messages[32] = {};
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetHealth(gv, PLAYER_DR_SEWARD) ==
				GAME_START_HUNTER_LIFE_POINTS - 2 * LIFE_LOSS_TRAP_ENCOUNTER);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(GvGetVampireLocation(gv) == NOWHERE);

		printf("===> Testing GvGetMoveHistory and GvGetLastMoves Functions\n");
		printf("==> Test Cases:\n");
		
		// Test Case: Lord Godalming (player is before current player)
		printf("(Given)	-> Lord Godalming's entire move history: ");
		{
			int numMoves = 0; bool canFree = true;
			PlaceId *moves = GvGetMoveHistory(gv, PLAYER_LORD_GODALMING,
			                                  &numMoves, &canFree);
			assert(numMoves == 7);
			assert(moves[0] == LISBON);
			assert(moves[1] == CADIZ);
			assert(moves[2] == GRANADA);
			assert(moves[3] == ALICANTE);
			assert(moves[4] == SARAGOSSA);
			assert(moves[5] == SANTANDER);
			assert(moves[6] == MADRID);
			if (canFree) free(moves);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		
		printf("(New)	-> Lord Godalming's last N move history: ");
		{
			int lastNMoves = 3;
			int numMoves = 0; bool canFree = true;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_LORD_GODALMING,
											lastNMoves, &numMoves, &canFree);
			assert(numMoves == 3);
			assert(moves[0] == SARAGOSSA);
			assert(moves[1] == SANTANDER);
			assert(moves[2] == MADRID);
			if (canFree) free(moves);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		// Test Case: Dr Seward (player is the next to current player)
		printf("(New)	-> Dr Seward's entire move history: ");
		{
			int numMoves = 0; bool canFree = true;
			PlaceId *moves = GvGetMoveHistory(gv, PLAYER_DR_SEWARD,
			                                  &numMoves, &canFree);
			assert(numMoves == 7);
			assert(moves[0] == LISBON);
			assert(moves[1] == ALICANTE);
			assert(moves[2] == BORDEAUX);
			assert(moves[3] == NANTES);
			assert(moves[4] == PARIS);
			assert(moves[5] == STRASBOURG);
			assert(moves[6] == FLORENCE);
			if (canFree) free(moves);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		
		printf("(New)	-> Dr Seward's last N move history: ");
		{
			int lastNMoves = 3;
			int numMoves = 0; bool canFree = true;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_DR_SEWARD,
											lastNMoves, &numMoves, &canFree);
			assert(numMoves == 3);
			assert(moves[0] == PARIS);
			assert(moves[1] == STRASBOURG);
			assert(moves[2] == FLORENCE);
			if (canFree) free(moves);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		// Test Case: Dr Van Helsing (player is current player)
		printf("(New)	-> Dr Van Helsing's entire move history: ");
		{
			int numMoves = 0; bool canFree = true;
			PlaceId *moves = GvGetMoveHistory(gv, PLAYER_VAN_HELSING,
			                                  &numMoves, &canFree);
			assert(numMoves == 6);
			assert(moves[0] == LISBON);
			assert(moves[1] == ALICANTE);
			assert(moves[2] == BORDEAUX);
			assert(moves[3] == NANTES);
			assert(moves[4] == PARIS);
			assert(moves[5] == STRASBOURG);
			if (canFree) free(moves);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		
		printf("(New)	-> Dr Van Helsing's last N move history: ");
		{
			int lastNMoves = 3;
			int numMoves = 0; bool canFree = true;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_VAN_HELSING,
											lastNMoves, &numMoves, &canFree);
			assert(numMoves == 3);
			assert(moves[0] == NANTES);
			assert(moves[1] == PARIS);
			assert(moves[2] == STRASBOURG);
			if (canFree) free(moves);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		// Test Case: Dracula (player after current player)
		printf("(Given)	-> Dracula's entire move history: ");
		{
			int numMoves = 0; bool canFree = true;
			PlaceId *moves = GvGetMoveHistory(gv, PLAYER_DRACULA,
			                                  &numMoves, &canFree);
			assert(numMoves == 6);
			assert(moves[0] == STRASBOURG);
			assert(moves[1] == CITY_UNKNOWN);
			assert(moves[2] == CITY_UNKNOWN);
			assert(moves[3] == DOUBLE_BACK_3);
			assert(moves[4] == HIDE);
			assert(moves[5] == CITY_UNKNOWN);
			if (canFree) free(moves);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		
		printf("(New)	-> Dracula's last N move history: ");
		{
			int lastNMoves = 3;
			int numMoves = 0; bool canFree = true;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_DRACULA,
											lastNMoves, &numMoves, &canFree);
			assert(numMoves == 3);
			assert(moves[0] == DOUBLE_BACK_3);
			assert(moves[1] == HIDE);
			assert(moves[2] == CITY_UNKNOWN);
			if (canFree) free(moves);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		
	}

	{
		char *trail =
			"GLS.... SLS.... HLS.... MGE.... DST.V.. "
			"GCA.... SAL.... HAL.... MGE.... DC?T... "
			"GGR.... SBO.... HBO.... MGE.... DTPT... "
			"GAL....";
		
		Message messages[32] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CASTLE_DRACULA);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == ALICANTE);
		assert(GvGetVampireLocation(gv) == STRASBOURG);

		printf("(New)	-> Testing overflow last N moves for Dracula: ");
		{
			int lastNMoves = 10;
			int numMoves = 0; bool canFree = true;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_DRACULA,
											lastNMoves, &numMoves, &canFree);
			assert(numMoves == 3);
			assert(moves[0] == STRASBOURG);
			assert(moves[1] == CITY_UNKNOWN);
			assert(moves[2] == TELEPORT);
			if (canFree) free(moves);

			printf("\033[1;32m");
			printf("Test passed!\n");
			printf("\033[0m");
		}

		printf("(New)	-> Testing round number last N moves for Dracula: ");
		{
			int lastNMoves = 4;
			int numMoves = 0; bool canFree = true;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_DRACULA,
											lastNMoves, &numMoves, &canFree);
			assert(numMoves == 3);
			assert(moves[0] == STRASBOURG);
			assert(moves[1] == CITY_UNKNOWN);
			assert(moves[2] == TELEPORT);
			if (canFree) free(moves);

			printf("\033[1;32m");
			printf("Test passed!\n");
			printf("\033[0m");
		}

		printf("(New)	-> Testing overflow last N moves for Lord Goldaming: ");
		{
			int lastNMoves = 10;
			int numMoves = 0; bool canFree = true;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_LORD_GODALMING,
											lastNMoves, &numMoves, &canFree);
			assert(numMoves == 4);
			assert(moves[0] == LISBON);
			assert(moves[1] == CADIZ);
			assert(moves[2] == GRANADA);
			assert(moves[3] == ALICANTE);
			if (canFree) free(moves);

			printf("\033[1;32m");
			printf("Test passed!\n");
			printf("\033[0m");
		}

		printf("(New)	-> Testing round number last N moves for Lord Goldaming: ");
		{
			int lastNMoves = 4;
			int numMoves = 0; bool canFree = true;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_LORD_GODALMING,
											lastNMoves, &numMoves, &canFree);
			assert(numMoves == 4);
			assert(moves[0] == LISBON);
			assert(moves[1] == CADIZ);
			assert(moves[2] == GRANADA);
			assert(moves[3] == ALICANTE);
			if (canFree) free(moves);

			printf("\033[1;32m");
			printf("Test passed!\n");
			printf("\033[0m");
		}

		printf("(New)	-> Testing overflow last N moves for Dr Seward: ");
		{
			int lastNMoves = 10;
			int numMoves = 0; bool canFree = true;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_DR_SEWARD,
											lastNMoves, &numMoves, &canFree);
			assert(numMoves == 3);
			assert(moves[0] == LISBON);
			assert(moves[1] == ALICANTE);
			assert(moves[2] == BORDEAUX);
			if (canFree) free(moves);

			printf("\033[1;32m");
			printf("Test passed!\n");
			printf("\033[0m");
		}

		printf("(New)	-> Testing round number last N moves for Dr Seward: ");
		{
			int lastNMoves = 4;
			int numMoves = 0; bool canFree = true;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_DR_SEWARD,
											lastNMoves, &numMoves, &canFree);
			assert(numMoves == 3);
			assert(moves[0] == LISBON);
			assert(moves[1] == ALICANTE);
			assert(moves[2] == BORDEAUX);
			if (canFree) free(moves);

			printf("\033[1;32m");
			printf("Test passed!\n");
			printf("\033[0m");
		}

		printf("(New)	-> Last 0 move history: ");
		{
			int lastNMoves = 0;
			int numMoves = 100; bool canFree = true;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_LORD_GODALMING,
											lastNMoves, &numMoves, &canFree);
			assert(numMoves == 0);
			if (canFree) free(moves);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		printf("(New)	-> Testing negative last N move history: ");
		{
			int lastNMoves = -6;
			int numMoves = 100; bool canFree = true;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_LORD_GODALMING,
											lastNMoves, &numMoves, &canFree);
			assert(numMoves == 0);
			if (canFree) free(moves);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		GvFree(gv);
	}


	printf("\n===> Testing GvGetLocationHistory and GvGetLastLocations Functions\n");
	printf("==> Test Cases:\n");
	{
		char *trail =
			"GLS.... SLS.... HLS.... MGE.... DST.V.. "
			"GCA.... SAL.... HAL.... MGE.... DC?T... "
			"GGR.... SBO.... HBO.... MGE.... DC?T... "
			"GAL.... SNA.... HNA.... MGE.... DD3T... "
			"GSR.... SPA.... HPA.... MGE.... DHIT... "
			"GSN.... SST.... HST.... MGE.... DC?T... "
			"GMA.... SFLTTV.";
		
		Message messages[32] = {};
		GameView gv = GvNew(trail, messages);

		// Test Case: Hunters
		// Test to see GvGetLocationHistory functions the same as GvGetLastMoves
		// for hunters
		printf("(New)	-> Dr Van Helsing's entire location history: ");
		{
			int numLocs = 0; bool canFree = true;
			PlaceId *locs = GvGetLocationHistory(gv, PLAYER_VAN_HELSING,
			                                     &numLocs, &canFree);
			assert(numLocs == 6);
			assert(locs[0] == LISBON);
			assert(locs[1] == ALICANTE);
			assert(locs[2] == BORDEAUX);
			assert(locs[3] == NANTES);
			assert(locs[4] == PARIS);
			assert(locs[5] == STRASBOURG);
			if (canFree) free(locs);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		// Test Case: Dracula
		printf("(Given)	-> Dracula's entire location history: ");
		{
			int numLocs = 0; bool canFree = true;
			PlaceId *locs = GvGetLocationHistory(gv, PLAYER_DRACULA,
			                                     &numLocs, &canFree);
			assert(numLocs == 6);
			assert(locs[0] == STRASBOURG);
			assert(locs[1] == CITY_UNKNOWN);
			assert(locs[2] == CITY_UNKNOWN);
			assert(locs[3] == STRASBOURG);
			assert(locs[4] == STRASBOURG);
			assert(locs[5] == CITY_UNKNOWN);
			if (canFree) free(locs);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		GvFree(gv);
	}

	{
		// Test with teleport move
		char *newTrail =
			"GLS.... SLS.... HLS.... MGE.... DST.V.. "
			"GCA.... SAL.... HAL.... MGE.... DTPT... "
			"GGR.... SBO.... HBO.... MGE.... DC?T... "
			"GAL.... SNA.... HNA.... MGE.... DD2T... "
			"GSR.... SPA.... HPA.... MGE.... DHIT... "
			"GSN.... SST.... HST.... MGE.... DC?T... "
			"GMA.... SFLTTV.";
		Message messages[32] = {};
		GameView gv = GvNew(newTrail, messages);

		// Test Case: Dracula used a teleport move in history
		printf("(New)	-> Dracula's entire move history with teleport: ");
		{
			int numMoves = 0; bool canFree = true;
			PlaceId *moves = GvGetMoveHistory(gv, PLAYER_DRACULA,
			                                  &numMoves, &canFree);
			assert(numMoves == 6);
			assert(moves[0] == STRASBOURG);
			assert(moves[1] == TELEPORT);
			assert(moves[2] == CITY_UNKNOWN);
			assert(moves[3] == DOUBLE_BACK_2);
			assert(moves[4] == HIDE);
			assert(moves[5] == CITY_UNKNOWN);
			if (canFree) free(moves);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		printf("(New)	-> Dracula's entire location history with teleport: ");
		{
			int numLocs = 0; bool canFree = true;
			PlaceId *locs = GvGetLocationHistory(gv, PLAYER_DRACULA,
			                                     &numLocs, &canFree);
			assert(numLocs == 6);
			assert(locs[0] == STRASBOURG);
			assert(locs[1] == CASTLE_DRACULA);
			assert(locs[2] == CITY_UNKNOWN);
			assert(locs[3] == CASTLE_DRACULA);
			assert(locs[4] == CASTLE_DRACULA);
			assert(locs[5] == CITY_UNKNOWN);
			if (canFree) free(locs);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		printf("(New)	-> Dracula's last N location history with teleport: ");
		{
			int lastNMoves = 3;
			int numLocs = 0; bool canFree = true;
			PlaceId *locs = GvGetLastLocations(gv, PLAYER_DRACULA,
											   lastNMoves, &numLocs, &canFree);
			assert(numLocs == 3);
			assert(locs[0] == CASTLE_DRACULA);
			assert(locs[1] == CASTLE_DRACULA);
			assert(locs[2] == CITY_UNKNOWN);
			if (canFree) free(locs);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		GvFree(gv);
	}

	{
		char *trail =
			"GLS.... SLS.... HLS.... MGE.... DST.V.. "
			"GCA.... SAL.... HAL.... MGE.... DC?T... "
			"GGR.... SBO.... HBO.... MGE.... DTPT... "
			"GAL....";
		
		Message messages[32] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CASTLE_DRACULA);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == ALICANTE);
		assert(GvGetVampireLocation(gv) == STRASBOURG);

		printf("(New)	-> Testing overflow last N location for Dracula: ");
		{
			int lastNMoves = 10;
			int numLocs = 0; bool canFree = true;
			PlaceId *locs = GvGetLastLocations(gv, PLAYER_DRACULA,
											   lastNMoves, &numLocs, &canFree);
			assert(numLocs == 3);
			assert(locs[0] == STRASBOURG);
			assert(locs[1] == CITY_UNKNOWN);
			assert(locs[2] == CASTLE_DRACULA);
			if (canFree) free(locs);

			printf("\033[1;32m");
			printf("Test passed!\n");
			printf("\033[0m");
		}

		printf("(New)	-> Testing round number last N location for Dracula: ");
		{
			int lastNMoves = 4;
			int numLocs = 0; bool canFree = true;
			PlaceId *locs = GvGetLastLocations(gv, PLAYER_DRACULA,
											   lastNMoves, &numLocs, &canFree);
			assert(numLocs == 3);
			assert(locs[0] == STRASBOURG);
			assert(locs[1] == CITY_UNKNOWN);
			assert(locs[2] == CASTLE_DRACULA);
			if (canFree) free(locs);

			printf("\033[1;32m");
			printf("Test passed!\n");
			printf("\033[0m");
		}

		printf("(New)	-> Testing overflow last N location for Lord Goldaming: ");
		{
			int lastNMoves = 10;
			int numLocs = 0; bool canFree = true;
			PlaceId *locs = GvGetLastLocations(gv, PLAYER_LORD_GODALMING,
											   lastNMoves, &numLocs, &canFree);
			assert(numLocs == 4);
			assert(locs[0] == LISBON);
			assert(locs[1] == CADIZ);
			assert(locs[2] == GRANADA);
			assert(locs[3] == ALICANTE);
			if (canFree) free(locs);

			printf("\033[1;32m");
			printf("Test passed!\n");
			printf("\033[0m");
		}

		printf("(New)	-> Testing round number last N location for Lord Goldaming: ");
		{
			int lastNMoves = 4;
			int numLocs = 0; bool canFree = true;
			PlaceId *locs = GvGetLastLocations(gv, PLAYER_LORD_GODALMING,
											   lastNMoves, &numLocs, &canFree);
			assert(numLocs == 4);
			assert(locs[0] == LISBON);
			assert(locs[1] == CADIZ);
			assert(locs[2] == GRANADA);
			assert(locs[3] == ALICANTE);
			if (canFree) free(locs);
			printf("\033[1;32m");
			printf("Test passed!\n");
			printf("\033[0m");
		}

		printf("(New)	-> Testing overflow last N location for Dr Seward: ");
		{
			int lastNMoves = 10;

			int numLocs = 0; bool canFree = true;
			PlaceId *locs = GvGetLastLocations(gv, PLAYER_DR_SEWARD,
											   lastNMoves, &numLocs, &canFree);
			assert(numLocs == 3);
			assert(locs[0] == LISBON);
			assert(locs[1] == ALICANTE);
			assert(locs[2] == BORDEAUX);
			if (canFree) free(locs);

			printf("\033[1;32m");
			printf("Test passed!\n");
			printf("\033[0m");
		}

		printf("(New)	-> Testing round number last N location for Dr Seward: ");
		{
			int lastNMoves = 4;
			int numLocs = 0; bool canFree = true;
			PlaceId *locs = GvGetLastLocations(gv, PLAYER_DR_SEWARD,
											   lastNMoves, &numLocs, &canFree);
			assert(numLocs == 3);
			assert(locs[0] == LISBON);
			assert(locs[1] == ALICANTE);
			assert(locs[2] == BORDEAUX);
			if (canFree) free(locs);

			printf("\033[1;32m");
			printf("Test passed!\n");
			printf("\033[0m");
		}

		printf("(New)	-> Last 0 location history: ");
		{
			int lastNMoves = 0;
			int numLocs = 100; bool canFree = true;
			PlaceId *locs = GvGetLastLocations(gv, PLAYER_DR_SEWARD,
											   lastNMoves, &numLocs, &canFree);
			assert(numLocs == 0);
			if (canFree) free(locs);

			printf("\033[1;32m");
			printf("Test passed!\n");
			printf("\033[0m");
		}

		printf("(New)	-> Testing negative last N location history: ");
		{
			int lastNMoves = -6;
			int numLocs = 100; bool canFree = true;
			PlaceId *locs = GvGetLastLocations(gv, PLAYER_DR_SEWARD,
											   lastNMoves, &numLocs, &canFree);
			assert(numLocs == 0);
			if (canFree) free(locs);

			printf("\033[1;32m");
			printf("Test passed!\n");
			printf("\033[0m");
		}

		GvFree(gv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("\n================= Testing connections =================\n");
		
		char *trail = "";
		Message messages[] = {};
		GameView gv = GvNew(trail, messages);

		printf("===> Testing GvGetReachable Function\n");
		printf("==> Test Cases:\n");
		{
			printf("(New)	-> Checking Draculas moveable: ");
			int numLocs = -1;
			PlaceId *locs = GvGetReachable(gv, PLAYER_DRACULA,
											1, BORDEAUX, &numLocs);

			assert(numLocs == 6);
			sortPlaces(locs, numLocs);
			assert(locs[0] == BAY_OF_BISCAY);
			assert(locs[1] == BORDEAUX);
			assert(locs[2] == CLERMONT_FERRAND);
			assert(locs[3] == NANTES);
			assert(locs[4] == SARAGOSSA);
			assert(locs[5] == TOULOUSE);
			free(locs);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		{
			printf("(New)	-> Checking Hunter rail connections (3): ");
			int numLocs = -1;
			PlaceId *locs = GvGetReachable(gv, PLAYER_LORD_GODALMING,
											3, ALICANTE, &numLocs);

			assert(numLocs == 9);
			sortPlaces(locs, numLocs);
			assert(locs[0] == ALICANTE);
			assert(locs[1] == BARCELONA);
			assert(locs[2] == BORDEAUX);
			assert(locs[3] == GRANADA);
			assert(locs[4] == LISBON);
			assert(locs[5] == MADRID);
			assert(locs[6] == MEDITERRANEAN_SEA);
			assert(locs[7] == SANTANDER);
			assert(locs[8] == SARAGOSSA);
			free(locs);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		{
			printf("(New)	-> Checking Hunter rail connections (2): ");
			int numLocs = -1;
			PlaceId *locs = GvGetReachable(gv, PLAYER_DR_SEWARD,
											1, FLORENCE, &numLocs);

			assert(numLocs == 8);
			sortPlaces(locs, numLocs);
			assert(locs[0] == FLORENCE);
			assert(locs[1] == GENEVA);
			assert(locs[2] == GENOA);
			assert(locs[3] == MILAN);
			assert(locs[4] == NAPLES);
			assert(locs[5] == ROME);
			assert(locs[6] == VENICE);
			assert(locs[7] == ZURICH);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		{
			printf("(New)	-> Checking Hunter rail connections (1): ");
			int numLocs = -1;
			PlaceId *locs = GvGetReachable(gv, PLAYER_LORD_GODALMING,
											1, FLORENCE, &numLocs);

			assert(numLocs == 5);
			sortPlaces(locs, numLocs);
			assert(locs[0] == FLORENCE);
			assert(locs[1] == GENOA);
			assert(locs[2] == MILAN);
			assert(locs[3] == ROME);
			assert(locs[4] == VENICE);
			free(locs);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		{
			printf("(New)	-> Checking Hunter rail connections (0 rail distance): ");
			int numLocs = -1;
			PlaceId *locs = GvGetReachable(gv, PLAYER_LORD_GODALMING,
											4, PARIS, &numLocs);
			sortPlaces(locs, numLocs);
			assert(numLocs == 7);
			assert(locs[0] == BRUSSELS);
			assert(locs[1] == CLERMONT_FERRAND);
			assert(locs[2] == GENEVA);
			assert(locs[3] == LE_HAVRE);
			assert(locs[4] == NANTES);
			assert(locs[5] == PARIS);
			assert(locs[6] == STRASBOURG);
			free(locs);
		}

		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		
		{
			printf("(New)	-> Checking Hunter rail connections (0): ");
			int numLocs = -1;
			PlaceId *locs = GvGetReachable(gv, PLAYER_LORD_GODALMING,
											4, FLORENCE, &numLocs);

			assert(numLocs == 4);
			sortPlaces(locs, numLocs);
			assert(locs[0] == FLORENCE);
			assert(locs[1] == GENOA);
			assert(locs[2] == ROME);
			assert(locs[3] == VENICE);
		}

		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		{
			printf("(New)	-> Checking Boat connections: ");
			int numLocs = -1;
			PlaceId *locs = GvGetReachable(gv, PLAYER_LORD_GODALMING,
											1, MEDITERRANEAN_SEA, &numLocs);

			assert(numLocs == 7);
			sortPlaces(locs, numLocs);
			assert(locs[0] == ALICANTE);
			assert(locs[1] == ATLANTIC_OCEAN);
			assert(locs[2] == BARCELONA);
			assert(locs[3] == CAGLIARI);
			assert(locs[4] == MARSEILLES);
			assert(locs[5] == MEDITERRANEAN_SEA);
			assert(locs[6] == TYRRHENIAN_SEA);
			free(locs);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		{
			printf("(New)	-> Checking Dracula can't reach the hospital: ");
			int numLocs = -1;
			PlaceId *locs = GvGetReachable(gv, PLAYER_DRACULA,
										1, BELGRADE, &numLocs);
			for (int i = 0; i < numLocs; i++) {
				assert(locs[i] != HOSPITAL_PLACE);
			}
			free(locs);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		printf("\n===> Testing GvGetReachableByType Function\n");
		printf("==> Test Cases:\n");

		{
			printf("(New)	-> Checking simultaneous connection types Hunters"
					" (Road and Boat): ");
			int numLocs = -1;
			PlaceId *locs = GvGetReachableByType(gv, PLAYER_DRACULA,
												1, BORDEAUX, true, false,
												true, &numLocs);

			assert(numLocs == 6);
			sortPlaces(locs, numLocs);
			assert(locs[0] == BAY_OF_BISCAY);
			assert(locs[1] == BORDEAUX);
			assert(locs[2] == CLERMONT_FERRAND);
			assert(locs[3] == NANTES);
			assert(locs[4] == SARAGOSSA);
			assert(locs[5] == TOULOUSE);
			free(locs);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		{
			printf("(New)	-> Checking simultaneous connection types Hunters"
					" (Boat and Rail): ");
			int numLocs = -1;
			PlaceId *locs = GvGetReachableByType(gv, PLAYER_LORD_GODALMING,
												1, LE_HAVRE, false, true,
												true, &numLocs);

			assert(numLocs == 3);
			sortPlaces(locs, numLocs);
			assert(locs[0] == ENGLISH_CHANNEL);
			assert(locs[1] == LE_HAVRE);
			assert(locs[2] == PARIS);
			free(locs);
		}

		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		{
			printf("(New)	-> Checking simultaneous connection types Hunters"
					" (Road and Rail): ");
			int numLocs = -1;
			PlaceId *locs = GvGetReachableByType(gv, PLAYER_LORD_GODALMING,
												2, PARIS, true, true,
												false, &numLocs);

			assert(numLocs == 11);
			sortPlaces(locs, numLocs);
			assert(locs[0] == BORDEAUX);
			assert(locs[1] == BRUSSELS);
			assert(locs[2] == CLERMONT_FERRAND);
			assert(locs[3] == COLOGNE);
			assert(locs[4] == GENEVA);
			assert(locs[5] == LE_HAVRE);
			assert(locs[6] == MARSEILLES);
			assert(locs[7] == NANTES);
			assert(locs[8] == PARIS);
			assert(locs[9] == SARAGOSSA);
			assert(locs[10] == STRASBOURG);
			free(locs);
		}

		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		{
			printf("(New)	-> Checking Draculas Rail connections (none): ");
			int numLocs = -1;
			PlaceId *locs = GvGetReachableByType(gv, PLAYER_DRACULA,
											1, BORDEAUX, false, true,
											false, &numLocs);
			assert(numLocs == 1);
			assert(locs[0] == BORDEAUX);
			free(locs);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		{
			printf("(New)	-> Checking rounds where hunters cant use rail "
					" Lord Godalming, Round 4: ");
			int numLocs = -1;
			PlaceId *locs = GvGetReachableByType(gv, PLAYER_LORD_GODALMING,
												4, PARIS, false, true,
												false, &numLocs);
			assert(numLocs == 1);
			assert(locs[0] == PARIS);
			free(locs);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		{
			printf("(Given)	-> Checking Galatz road connections "
			       "(Lord Godalming, Round 1): ");
			int numLocs = -1;
			PlaceId *locs = GvGetReachableByType(gv, PLAYER_LORD_GODALMING,
			                                     1, GALATZ, true, false,
			                                     false, &numLocs);

			assert(numLocs == 5);
			sortPlaces(locs, numLocs);
			assert(locs[0] == BUCHAREST);
			assert(locs[1] == CASTLE_DRACULA);
			assert(locs[2] == CONSTANTA);
			assert(locs[3] == GALATZ);
			assert(locs[4] == KLAUSENBURG);
			free(locs);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		{
			printf("(Given)	-> Checking Ionian Sea boat connections "
			       "(Lord Godalming, Round 1): ");
			
			int numLocs = -1;
			PlaceId *locs = GvGetReachableByType(gv, PLAYER_LORD_GODALMING,
			                                     1, IONIAN_SEA, false, false,
			                                     true, &numLocs);
			
			assert(numLocs == 7);
			sortPlaces(locs, numLocs);
			assert(locs[0] == ADRIATIC_SEA);
			assert(locs[1] == ATHENS);
			assert(locs[2] == BLACK_SEA);
			assert(locs[3] == IONIAN_SEA);
			assert(locs[4] == SALONICA);
			assert(locs[5] == TYRRHENIAN_SEA);
			assert(locs[6] == VALONA);
			free(locs);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		{
			printf("(Given)	-> Checking Paris rail connections "
			       "(Lord Godalming, Round 2): ");
			int numLocs = -1;
			PlaceId *locs = GvGetReachableByType(gv, PLAYER_LORD_GODALMING,
			                                     2, PARIS, false, true,
			                                     false, &numLocs);
			
			assert(numLocs == 7);
			sortPlaces(locs, numLocs);
			assert(locs[0] == BORDEAUX);
			assert(locs[1] == BRUSSELS);
			assert(locs[2] == COLOGNE);
			assert(locs[3] == LE_HAVRE);
			assert(locs[4] == MARSEILLES);
			assert(locs[5] == PARIS);
			assert(locs[6] == SARAGOSSA);
			free(locs);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		
		{
			printf("(Given)	-> Checking Athens rail connections (none): ");
			int numLocs = -1;
			PlaceId *locs = GvGetReachableByType(gv, PLAYER_LORD_GODALMING,
			                                     1, ATHENS, false, true,
			                                     false, &numLocs);
			
			assert(numLocs == 1);
			assert(locs[0] == ATHENS);
			free(locs);
		}
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

		GvFree(gv);
	}

	return EXIT_SUCCESS;
}
