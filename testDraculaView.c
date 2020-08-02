////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// testDraculaView.c: test the DraculaView ADT
//
// As supplied, these are very simple tests.  You should write more!
// Don't forget to be rigorous and thorough while writing tests.
//
// 2014-07-01	v1.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2017-12-02	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v2.0	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v3.0	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

// Written in the month of July 2020 for COMP2521 T2 Assignment 2.

// This is a testing file to determine if the implementation for DraculaView.h
// ADT is correct and functional. Test cases were both supplied from the given
// code and written by Gabriel Ting and Christian Ilagan.

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DraculaView.h"
#include "Game.h"
#include "Places.h"
#include "testUtils.h"

int main(void)
{
	printf("********************** DraculaView Testing **********************\n");
	printf("Key: (Given)	-> Supplied test case intially\n");
	printf("     (Added)	-> New asserts has been added to the original test case\n");
	printf("     (New)	-> New test case created by team\n");

	printf("\n================= Game State Information =================\n");
	{///////////////////////////////////////////////////////////////////	
		printf("(New)	-> Basic initialisation: ");

		char *trail = "";
		Message messages[] = {};
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 0);
		assert(DvGetScore(dv) == GAME_START_SCORE);
		assert(DvGetHealth(dv, PLAYER_LORD_GODALMING) == 
								GAME_START_HUNTER_LIFE_POINTS);
		assert(DvGetHealth(dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert(DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING) == NOWHERE);
		assert(DvGetVampireLocation(dv) == NOWHERE);

		DvFree(dv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(Given)	-> Test for basic functions, "
			   "just before Dracula's first move: ");

		char *trail =
			"GST.... SAO.... HZU.... MBB....";
		
		Message messages[4] = {
			"Hello", "Goodbye", "Stuff", "..."
		};
		
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 0);
		assert(DvGetScore(dv) == GAME_START_SCORE);
		assert(DvGetHealth(dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert(DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(DvGetPlayerLocation(dv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(DvGetPlayerLocation(dv, PLAYER_VAN_HELSING) == ZURICH);
		assert(DvGetPlayerLocation(dv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(DvGetPlayerLocation(dv, PLAYER_DRACULA) == NOWHERE);
		assert(DvGetVampireLocation(dv) == NOWHERE);
		int numTraps = -1;
		PlaceId *traps = DvGetTrapLocations(dv, &numTraps);
		assert(numTraps == 0);
		free(traps);

		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Testing vampire/trap locations: ");
		
		char *trail =
			"GVI.... SGE.... HGE.... MGE.... DCD.V.. "
			"GBD.... SGE.... HGE.... MGE.... DKLT... "
			"GSZ.... SGE.... HGE.... MGE.... DGAT... "
			"GSZ.... SGE.... HGE.... MGE....";
		
		Message messages[19] = {};
		DraculaView dv = DvNew(trail, messages);
		
		assert(DvGetPlayerLocation(dv, PLAYER_DRACULA) == GALATZ);
		assert(DvGetVampireLocation(dv) == CASTLE_DRACULA);
		int numTraps = 0;
		PlaceId *traps = DvGetTrapLocations(dv, &numTraps);
		assert(numTraps == 2);
		sortPlaces(traps, numTraps);
		assert(traps[0] == GALATZ && traps[1] == KLAUSENBURG);
		free(traps);
		
		DvFree(dv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Testing trap locations after one is destroyed: ");
		
		char *trail =
			"GVI.... SGE.... HGE.... MGE.... DBC.V.. "
			"GBD.... SGE.... HGE.... MGE.... DKLT... "
			"GSZ.... SGE.... HGE.... MGE.... DGAT... "
			"GBE.... SGE.... HGE.... MGE.... DCNT... "
			"GKLT... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		assert(DvGetHealth(dv, PLAYER_LORD_GODALMING) ==
				GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_TRAP_ENCOUNTER);
		assert(DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING) == KLAUSENBURG);
		assert(DvGetVampireLocation(dv) == BUCHAREST);
		int numTraps = 0;
		PlaceId *traps = DvGetTrapLocations(dv, &numTraps);
		assert(numTraps == 2);
		sortPlaces(traps, numTraps);
		assert(traps[0] == CONSTANTA && traps[1] == GALATZ);
		free(traps);
		
		DvFree(dv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Testing a vampire maturing: ");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V.. "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T.V.";
		
		Message messages[35] = {};
		DraculaView dv = DvNew(trail, messages);
		
		assert(DvGetScore(dv) == GAME_START_SCORE
		                         - 7 * SCORE_LOSS_DRACULA_TURN
		                         - SCORE_LOSS_VAMPIRE_MATURES);
		assert(DvGetPlayerLocation(dv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(DvGetVampireLocation(dv) == NOWHERE);
		
		DvFree(dv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////

		printf("(New)	-> get immvampire location when dead: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DZU.V.. "
			"GGE.... SGE.... HGE.... MZU.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T...";
			
		Message messages[20] = {};
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 4);
		assert(DvGetPlayerLocation(dv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(DvGetVampireLocation(dv) == NOWHERE);

		DvFree(dv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

	}

	{///////////////////////////////////////////////////////////////////

		printf("(New)	-> get immvampire location when dead 2: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DZU.V.. "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MZU.... DC?T... "
			"GGE.... SGE.... HGE.... MZU.... DC?T...";
			
		Message messages[20] = {};
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 4);
		assert(DvGetPlayerLocation(dv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(DvGetVampireLocation(dv) == NOWHERE);

		DvFree(dv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(Given)	-> Test for encountering Dracula: ");

		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.V.. "
			"GGEVD.. SAO.... HCD.... MAO....";
		
		Message messages[9] = {
			"Hello", "Goodbye", "Stuff", "...", "Mwahahah",
			"Aha!", "", "", ""
		};
		
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 1);
		assert(DvGetScore(dv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN);
		assert(DvGetHealth(dv, PLAYER_LORD_GODALMING) == 5);
		assert(DvGetHealth(dv, PLAYER_DRACULA) == 30);
		assert(DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING) == GENEVA);
		assert(DvGetPlayerLocation(dv, PLAYER_DRACULA) == GENEVA);
		assert(DvGetVampireLocation(dv) == NOWHERE);

		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(Given)	-> Test for Dracula leaving minions 1: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DED.V.. "
			"GST.... SST.... HST.... MST.... DMNT... "
			"GST.... SST.... HST.... MST.... DLOT... "
			"GST.... SST.... HST.... MST.... DHIT... "
			"GST.... SST.... HST.... MST....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 4);
		assert(DvGetVampireLocation(dv) == EDINBURGH);
		int numTraps = -1;
		PlaceId *traps = DvGetTrapLocations(dv, &numTraps);
		assert(numTraps == 3);
		sortPlaces(traps, numTraps);
		assert(traps[0] == LONDON);
		assert(traps[1] == LONDON);
		assert(traps[2] == MANCHESTER);
		free(traps);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	printf("\n================= Making a Move =================\n");
	printf("===> DvGetValidMoves\n");
	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Dracula's valid moves 1: ");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DCD.V.. "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[9] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
		assert(numMoves == 4);
		sortPlaces(moves, numMoves);
		assert(moves[0] == GALATZ);
		assert(moves[1] == KLAUSENBURG);
		assert(moves[2] == HIDE);
		assert(moves[3] == DOUBLE_BACK_1);
		free(moves);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////

		printf("(New)	-> Dracula's valid moves 2: ");

		char *trail =
			"GMN.... SPL.... HAM.... MPA.... DGA.V.. "
			"GLV.... SLO.... HNS.... MST.... DHIT... "
			"GIR.... SPL.... HAO.... MZU.... DCDT... "
			"GSW.... SLO.... HNS.... MFR.... DKLT... "
			"GLV.... SPL.... HAO.... MZU.... DBCT... "
			"GSW.... SLO.... HNS.... MMR....";


		Message messages[29] = {};
		DraculaView dv = DvNew(trail, messages);

		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
		assert(numMoves == 7);
		sortPlaces(moves, numMoves);
		assert(moves[0] == BELGRADE);
		assert(moves[1] == CONSTANTA);
		assert(moves[2] == SOFIA);
		assert(moves[3] == DOUBLE_BACK_1);
		assert(moves[4] == DOUBLE_BACK_2);
		assert(moves[5] == DOUBLE_BACK_4);
		assert(moves[6] == DOUBLE_BACK_5);
		free(moves);

		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> HIDE as latest move 1: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DCA.... "
			"GGE.... SGE.... HGE.... MGE.... DLS.... "
			"GGE.... SGE.... HGE.... MGE.... DSN.... "
			"GST.... SST.... HST.... MST.... DSR.... "
			"GGE.... SGE.... HGE.... MGE.... DMA.... "
			"GST.... SST.... HST.... MST.... DGR.... "
			"GST.... SST.... HST.... MST.... DHI.... "
			"GST.... SST.... HST.... MST....";
		
		Message messages[34] = {};
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 7);
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
		assert(numMoves == 5);
		sortPlaces(moves, numMoves);
		assert(moves[0] == ALICANTE);
		assert(moves[1] == CADIZ);
		assert(moves[2] == DOUBLE_BACK_1);
		assert(moves[3] == DOUBLE_BACK_2);
		assert(moves[4] == DOUBLE_BACK_3);
		free(moves);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> HIDE as latest move 2: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DRO.... "
			"GGE.... SGE.... HGE.... MGE.... DFL.... "
			"GGE.... SGE.... HGE.... MGE.... DHI.... "
			"GST.... SST.... HST.... MST....";		
		Message messages[4] = {};
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 3);
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
		sortPlaces(moves, numMoves);
		assert(numMoves == 5);
		assert(moves[0] == GENOA);
		assert(moves[1] == VENICE);
		assert(moves[2] == DOUBLE_BACK_1);
		assert(moves[3] == DOUBLE_BACK_2);
		assert(moves[4] == DOUBLE_BACK_3);
		free(moves);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Dracula goes back to last deleted location on trail 1: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DCA.... "
			"GGE.... SGE.... HGE.... MGE.... DLS.... "
			"GGE.... SGE.... HGE.... MGE.... DSN.... "
			"GST.... SST.... HST.... MST.... DSR.... "
			"GGE.... SGE.... HGE.... MGE.... DMA.... "
			"GST.... SST.... HST.... MST.... DGR.... "
			"GST.... SST.... HST.... MST....";
		
		Message messages[34] = {};
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 6);
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
		assert(numMoves == 5);
		sortPlaces(moves, numMoves);
		assert(moves[0] == ALICANTE);
		assert(moves[1] == CADIZ);
		assert(moves[2] == HIDE);
		assert(moves[3] == DOUBLE_BACK_1);
		assert(moves[4] == DOUBLE_BACK_2);
		free(moves);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Dracula goes back to last deleted location on trail 2: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DSR.... "
			"GGE.... SGE.... HGE.... MGE.... DMA.... "
			"GGE.... SGE.... HGE.... MGE.... DGR.... "
			"GST.... SST.... HST.... MST.... DAL.... "
			"GGE.... SGE.... HGE.... MGE.... DBA.... "
			"GGE.... SGE.... HGE.... MGE.... DTO.... "
			"GST.... SST.... HST.... MST....";
		
		Message messages[34] = {};
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 6);
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
		assert(numMoves == 7);
		sortPlaces(moves, numMoves);
		assert(moves[0] == BORDEAUX);
		assert(moves[1] == CLERMONT_FERRAND);
		assert(moves[2] == MARSEILLES);
		assert(moves[3] == SARAGOSSA);
		assert(moves[4] == HIDE);
		assert(moves[5] == DOUBLE_BACK_1);
		assert(moves[6] == DOUBLE_BACK_2);
		free(moves);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Dracula has teleported to Castle Dracula: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DVA.... "
			"GGE.... SGE.... HGE.... MGE.... DHI.... "
			"GGE.... SGE.... HGE.... MGE.... DD1.... "
			"GST.... SST.... HST.... MST.... DIO.... "
			"GGE.... SGE.... HGE.... MGE.... DAT.... "
			"GGE.... SGE.... HGE.... MGE.... DTP.... "
			"GST.... SST.... HST.... MST.... DGA.... "
			"GST.... SST.... HST.... MST....";
		
		Message messages[34] = {};
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 7);
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
		assert(numMoves == 5);
		sortPlaces(moves, numMoves);
		assert(moves[0] == BUCHAREST);
		assert(moves[1] == CASTLE_DRACULA);
		assert(moves[2] == CONSTANTA);
		assert(moves[3] == KLAUSENBURG);
		assert(moves[4] == HIDE);
		free(moves);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Cannot Hide at sea: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DBS.... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[4] = {};
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 1);
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
		assert(numMoves == 4);
		sortPlaces(moves, numMoves);
		assert(moves[0] == CONSTANTA);
		assert(moves[1] == IONIAN_SEA);
		assert(moves[2] == VARNA);
		assert(moves[3] == DOUBLE_BACK_1);
		free(moves);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}


	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> No valid moves 1: ");

		char *trail =
			"GST.... SST.... HST.... MST....";
		
		Message messages[4] = {};
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 0);
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
		assert(numMoves == 0);
		assert(moves == NULL);
		free(moves);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> No valid moves 2: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DVA.... "
			"GGE.... SGE.... HGE.... MGE.... DHI.... "
			"GGE.... SGE.... HGE.... MGE.... DD1.... "
			"GST.... SST.... HST.... MST.... DIO.... "
			"GGE.... SGE.... HGE.... MGE.... DAT.... "
			"GST.... SST.... HST.... MST....";		
		Message messages[4] = {};
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 5);
		int numMoves = -1;
		PlaceId *moves = DvGetValidMoves(dv, &numMoves);
		assert(numMoves == 0);
		assert(moves == NULL);
		free(moves);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}
	printf("\n");
	printf("===> DvWhereCanIGo and DvWhereCanIGoByType\n");
	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Cannot Hide at sea: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DBS.... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[4] = {};
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 1);
		int numMoves = -1;
		PlaceId *moves = DvWhereCanIGo(dv, &numMoves);
		assert(numMoves == 4);
		sortPlaces(moves, numMoves);
		assert(moves[0] == BLACK_SEA);
		assert(moves[1] == CONSTANTA);
		assert(moves[2] == IONIAN_SEA);
		assert(moves[3] == VARNA);
		free(moves);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}
	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Dracula has teleported to Castle Dracula: ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DVA.... "
			"GGE.... SGE.... HGE.... MGE.... DHI.... "
			"GGE.... SGE.... HGE.... MGE.... DD1.... "
			"GST.... SST.... HST.... MST.... DIO.... "
			"GGE.... SGE.... HGE.... MGE.... DAT.... "
			"GGE.... SGE.... HGE.... MGE.... DTP.... "
			"GST.... SST.... HST.... MST.... DGA.... "
			"GST.... SST.... HST.... MST....";
		
		Message messages[34] = {};
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 7);
		int numMoves = -1;
		PlaceId *moves = DvWhereCanIGo(dv, &numMoves);
		assert(numMoves == 5);
		sortPlaces(moves, numMoves);
		assert(moves[0] == BUCHAREST);
		assert(moves[1] == CASTLE_DRACULA);
		assert(moves[2] == CONSTANTA);
		assert(moves[3] == GALATZ);
		assert(moves[4] == KLAUSENBURG);
		free(moves);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////

		printf("(New)	-> Test for DvWhereCanIGo 0: ");

		char *trail =
			"GMN.... SPL.... HAM.... MPA.... DGA.V.. "
			"GLV.... SLO.... HNS.... MST.... DHIT... "
			"GIR.... SPL.... HAO.... MZU.... DCDT... "
			"GSW.... SLO.... HNS.... MFR.... DKLT... "
			"GLV.... SPL.... HAO.... MZU.... DBCT... "
			"GSW.... SLO.... HNS.... MMR....";


		Message messages[29] = {};
		DraculaView dv = DvNew(trail, messages);

		int numMoves = -1;
		PlaceId *moves = DvWhereCanIGo(dv, &numMoves);
		assert(numMoves == 6);
		sortPlaces(moves, numMoves);
		assert(moves[0] == BELGRADE);
		assert(moves[1] == BUCHAREST);
		assert(moves[2] == CONSTANTA);
		assert(moves[3] == GALATZ);
		assert(moves[4] == KLAUSENBURG);
		assert(moves[5] == SOFIA);
		free(moves);

		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(Given)	-> Test for DvWhereCanIGo 1: ");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DKL.V.. "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "
			"GGE.... SGE.... HGE.... MGE.... DBCT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);
		assert(numLocs == 4);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BELGRADE);
		assert(locs[1] == CONSTANTA);
		assert(locs[2] == GALATZ);
		assert(locs[3] == SOFIA);
		free(locs);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Test for DvWhereCanIGo 2 (Item leaves the trail): ");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DGR.V.. "
			"GGE.... SGE.... HGE.... MGE.... DMAT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE.... DLST... "
			"GGE.... SGE.... HGE.... MGE.... DCAT... "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);
		assert(numLocs == 2);
		sortPlaces(locs, numLocs);
		assert(locs[0] == ATLANTIC_OCEAN);
		assert(locs[1] == GRANADA);
		free(locs);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Test for DvWhereCanIGo 3 (Hide & DB available): ");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DTS.V.. "
			"GGE.... SGE.... HGE.... MGE.... DROT... "
			"GGE.... SGE.... HGE.... MGE.... DNPT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);
		assert(numLocs == 4);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BARI);
		assert(locs[1] == NAPLES);
		assert(locs[2] == ROME);
		assert(locs[3] == TYRRHENIAN_SEA);
		free(locs);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Test for DvWhereCanIGo 4 (Testing middle DOUBLE_BACKS): ");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DLS.V.. "
			"GGE.... SGE.... HGE.... MGE.... DSNT... "
			"GGE.... SGE.... HGE.... MGE.... DSRT... "
			"GGE.... SGE.... HGE.... MGE.... DMAT... "
			"GGE.... SGE.... HGE.... MGE.... DGRT... "
			"GGE.... SGE.... HGE.... MGE.... DCAT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);
		assert(numLocs == 5);
		sortPlaces(locs, numLocs);
		assert(locs[0] == ATLANTIC_OCEAN);
		assert(locs[1] == CADIZ);
		assert(locs[2] == GRANADA);
		assert(locs[3] == LISBON);
		assert(locs[4] == MADRID);
		free(locs);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Test for DvWhereCanIGo 5 (Testing DOUBLE_BACK_5): ");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DST.V.. "
			"GGE.... SGE.... HGE.... MGE.... DNUT... "
			"GGE.... SGE.... HGE.... MGE.... DMUT... "
			"GGE.... SGE.... HGE.... MGE.... DZUT... "
			"GGE.... SGE.... HGE.... MGE.... DGET... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGo(dv, &numLocs);
		assert(numLocs == 6);
		sortPlaces(locs, numLocs);
		assert(locs[0] == CLERMONT_FERRAND);
		assert(locs[1] == GENEVA);
		assert(locs[2] == MARSEILLES);
		assert(locs[3] == PARIS);
		assert(locs[4] == STRASBOURG);
		assert(locs[5] == ZURICH);
		free(locs);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Test for DvWhereCanIGoByType 1 (boat is restricted): ");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DCF.V.. "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE.... DTOT... "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "
			"GGE.... SGE.... HGE.... MGE.... DBOT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGoByType(dv, true, false, &numLocs);
		assert(numLocs == 2);
		sortPlaces(locs, numLocs);
		assert(locs[0] == NANTES);
		assert(locs[1] == SARAGOSSA);
		free(locs);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}
	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Test for DvWhereCanIGoByType 2 (road is restricted): ");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DLS.V.. "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGoByType(dv, false, true, &numLocs);
		assert(numLocs == 1);
		sortPlaces(locs, numLocs);
		assert(locs[0] == ATLANTIC_OCEAN);
		free(locs);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}
	{///////////////////////////////////////////////////////////////////
		// this test should behave just like DvWhereCanIGo
		printf("(New)	-> Test for DvWhereCanIGoByType 3 (nothing restricted): ");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DLS.V.. "
			"GGE.... SGE.... HGE.... MGE.... DSNT... "
			"GGE.... SGE.... HGE.... MGE.... DSRT... "
			"GGE.... SGE.... HGE.... MGE.... DMAT... "
			"GGE.... SGE.... HGE.... MGE.... DGRT... "
			"GGE.... SGE.... HGE.... MGE.... DCAT... "
			"GGE.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGoByType(dv, true, true, &numLocs);
		assert(numLocs == 5);
		sortPlaces(locs, numLocs);
		assert(locs[0] == ATLANTIC_OCEAN);
		assert(locs[1] == CADIZ);
		assert(locs[2] == GRANADA);
		assert(locs[3] == LISBON);
		assert(locs[4] == MADRID);
		free(locs);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}
	{///////////////////////////////////////////////////////////////////
		
		printf("(New)	-> Test for DvWhereCanIGoByType 4 (Item Leaves trail"
				" - boat restricted): ");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DGR.V.. "
			"GGE.... SGE.... HGE.... MGE.... DMAT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE.... DLST... "
			"GGE.... SGE.... HGE.... MGE.... DCAT... "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "
			"GGE.... SGE.... HGE.... MGE....";
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGoByType(dv, true, false, &numLocs);
		assert(numLocs == 1);
		sortPlaces(locs, numLocs);
		assert(locs[0] == GRANADA);
		free(locs);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
		
		printf("(New)	-> Test for DvWhereCanIGoByType 5 (Item Leaves trail"
				" - road restricted): ");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DGR.V.. "
			"GGE.... SGE.... HGE.... MGE.... DMAT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE.... DLST... "
			"GGE.... SGE.... HGE.... MGE.... DCAT... "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "
			"GGE.... SGE.... HGE.... MGE....";
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGoByType(dv, false, true, &numLocs);
		assert(numLocs == 1);
		assert(locs[0] == ATLANTIC_OCEAN);
		free(locs);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
		
		printf("(New)	-> Test for DvWhereCanIGoByType 6 (Both Restricted): ");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DGR.V.. "
			"GGE.... SGE.... HGE.... MGE.... DMAT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE.... DLST... "
			"GGE.... SGE.... HGE.... MGE.... DCAT... "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "
			"GGE.... SGE.... HGE.... MGE....";
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanIGoByType(dv, false, false, &numLocs);
		assert(numLocs == 0);
		sortPlaces(locs, numLocs);
		assert(locs == NULL);
		free(locs);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	printf("\n===> DvWhereCanTheyGo and DvWhereCanTheyGoByType\n");
	
	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Test for DvWhereCanTheyGo where rail distance is 0: ");
		
		char *trail =
			// "GMA.... SGE.... HGE.... MGE.... DKL.V.. "
			"GSN.... SGE.... HGE.... MGE.... DD1T... "
			"GBB.... SGE.... HGE.... MGE.... DBCT... "
			"GNA.... SGE.... HGE.... MGE.... DHIT... "
			"GPA.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanTheyGo(dv, PLAYER_LORD_GODALMING, &numLocs);
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
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Test for DvWhereCanTheyGo where rail distance is 1: ");
		
		char *trail =
			// "GMA.... SMU.... HGE.... MGE.... DKL.V.. "
			"GSN.... SVE.... HGE.... MGE.... DD1T... "
			"GBB.... SVE.... HGE.... MGE.... DBCT... "
			"GNA.... SGO.... HGE.... MGE.... DHIT... "
			"GPA.... SMI.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanTheyGo(dv, PLAYER_DR_SEWARD, &numLocs);
		sortPlaces(locs, numLocs);
		assert(numLocs == 8);
		assert(locs[0] == FLORENCE);
		assert(locs[1] == GENEVA);
		assert(locs[2] == GENOA);
		assert(locs[3] == MARSEILLES);
		assert(locs[4] == MILAN);
		assert(locs[5] == MUNICH);
		assert(locs[6] == VENICE);
		assert(locs[7] == ZURICH);
		
		free(locs);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Test for DvWhereCanTheyGo where rail distance is 3: ");
		
		char *trail =
			// "GSN.... SGE.... HGE.... MGE.... DKL.V.. "
			"GBB.... SGE.... HGE.... MGE.... DD1T... "
			"GNA.... SGE.... HGE.... MGE.... DBCT... "
			"GPA.... SGE.... HGE.... MGE....";
		
		Message messages[19] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = DvWhereCanTheyGo(dv, PLAYER_LORD_GODALMING, &numLocs);
		sortPlaces(locs, numLocs);
		assert(numLocs == 14);
		assert(locs[0] == BARCELONA);
		assert(locs[1] == BORDEAUX);
		assert(locs[2] == BRUSSELS);
		assert(locs[3] == CLERMONT_FERRAND);
		assert(locs[4] == COLOGNE);
		assert(locs[5] == FRANKFURT);
		assert(locs[6] == GENEVA);
		assert(locs[7] == LE_HAVRE);
		assert(locs[8] == MADRID);
		assert(locs[9] == MARSEILLES);
		assert(locs[10] == NANTES);
		assert(locs[11] == PARIS);
		assert(locs[12] == SARAGOSSA);
		assert(locs[13] == STRASBOURG);

		free(locs);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Test for DvWhereCanTheyGoByType hunters (no rail): ");
		
		char *trail =
			// "GSN.... SGE.... HGE.... MGE.... DKL.V.. "
			"GBB.... SGE.... HGE.... MGE.... DD1T... "
			"GNA.... SGE.... HGE.... MGE.... DBCT... "
			"GPA.... SGE.... HGE.... MGE....";
		
		Message messages[19] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		bool road = true;
		bool rail = false;
		bool boat = true;
		PlaceId *locs = DvWhereCanTheyGoByType(dv, PLAYER_LORD_GODALMING,
												road, rail, boat, &numLocs);
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
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Test for DvWhereCanTheyGoByType hunters (no road): ");
		
		char *trail =
			// "GSN.... SGE.... HGE.... MGE.... DKL.V.. "
			"GBB.... SGE.... HGE.... MGE.... DD1T... "
			"GNA.... SGE.... HGE.... MGE.... DBCT... "
			"GPA.... SGE.... HGE.... MGE....";
		
		Message messages[19] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		bool road = false;
		bool rail = true;
		bool boat = true;
		PlaceId *locs = DvWhereCanTheyGoByType(dv, PLAYER_LORD_GODALMING,
												road, rail, boat, &numLocs);
		sortPlaces(locs, numLocs);
		assert(numLocs == 10);
		assert(locs[0] == BARCELONA);
		assert(locs[1] == BORDEAUX);
		assert(locs[2] == BRUSSELS);
		assert(locs[3] == COLOGNE);
		assert(locs[4] == FRANKFURT);
		assert(locs[5] == LE_HAVRE);
		assert(locs[6] == MADRID);
		assert(locs[7] == MARSEILLES);
		assert(locs[8] == PARIS);
		assert(locs[9] == SARAGOSSA);

		free(locs);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}
		
	{///////////////////////////////////////////////////////////////////
	
		printf("(New)	-> Test for DvWhereCanTheyGoByType hunters (no road and rail distance is 0): ");
		
		char *trail =
			// "GMA.... SGE.... HGE.... MGE.... DKL.V.. "
			"GSN.... SGE.... HGE.... MGE.... DD1T... "
			"GBB.... SGE.... HGE.... MGE.... DBCT... "
			"GNA.... SGE.... HGE.... MGE.... DHIT... "
			"GPA.... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		DraculaView dv = DvNew(trail, messages);
		
		int numLocs = -1;
		bool road = false;
		bool rail = true;
		bool boat = true;
		PlaceId *locs = DvWhereCanTheyGoByType(dv, PLAYER_LORD_GODALMING,
												road, rail, boat, &numLocs);
		sortPlaces(locs, numLocs);
		assert(numLocs == 1);
		assert(locs[0] == PARIS);

		free(locs);
		
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}

	return EXIT_SUCCESS;
}
