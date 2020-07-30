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
	{///////////////////////////////////////////////////////////////////	
		printf("\t-> Personal testing - Basic initialisation: ");

		char *trail = "";
		Message messages[] = {};
		DraculaView dv = DvNew(trail, messages);

		assert(DvGetRound(dv) == 0);
		assert(DvGetScore(dv) == GAME_START_SCORE);
		assert(DvGetHealth(dv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
		assert(DvGetHealth(dv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert(DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING) == NOWHERE);
		assert(DvGetVampireLocation(dv) == NOWHERE);

		DvFree(dv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("\t-> Personal testing - Test for basic functions, "
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
	
		printf("\t-> Personal testing - Testing vampire/trap locations: ");
		
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
	
		printf("\t-> Personal testing - Testing trap locations after one is destroyed: ");
		
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
	
		printf("\t-> Personal testing - Testing a vampire maturing: ");
		
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

		printf("\t-> Personal testing - get immvampire location when dead: ");

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

		printf("\t-> Personal testing - get immvampire location when dead 2: ");

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
	
		printf("\t-> Test for encountering Dracula: ");

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
	
		printf("\t-> Test for Dracula leaving minions 1: ");

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

	{///////////////////////////////////////////////////////////////////
	
		printf("\t-> Test for Dracula's valid moves 1: ");
		
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

		printf("\t-> Test for Dracula's valid moves 2: ");

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
		assert(numMoves == 6);
		sortPlaces(moves, numMoves);
		assert(moves[0] == BELGRADE);
		assert(moves[1] == CONSTANTA);
		assert(moves[2] == SOFIA);
		assert(moves[3] == DOUBLE_BACK_1);
		assert(moves[4] == DOUBLE_BACK_2);
		assert(moves[5] == DOUBLE_BACK_5); // TODO is this right
		free(moves);

		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
		DvFree(dv);
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("\t-> Test for DvWhereCanIGo 1: ");
		
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
	
		printf("\t-> Test for DvWhereCanIGo 2: ");
		
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
	
		printf("\t-> Test for DvWhereCanIGo 3: ");
		
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
	
		printf("\t-> Test for DvWhereCanTheyGo where rail distance is 0: \n");
		
		char *trail =
			"GMA.... SGE.... HGE.... MGE.... DKL.V.. "
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
	
		printf("\t-> Test for DvWhereCanTheyGo where rail distance is 1: ");
		
		char *trail =
			"GMA.... SMU.... HGE.... MGE.... DKL.V.. "
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
	
		printf("\t-> Test for DvWhereCanTheyGo where rail distance is 3: ");
		
		char *trail =
			"GSN.... SGE.... HGE.... MGE.... DKL.V.. "
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
	
		printf("\t-> Test for DvWhereCanTheyGoByType hunters (no rail): ");
		
		char *trail =
			"GSN.... SGE.... HGE.... MGE.... DKL.V.. "
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
	
		printf("\t-> Test for DvWhereCanTheyGoByType hunters (no road): ");
		
		char *trail =
			"GSN.... SGE.... HGE.... MGE.... DKL.V.. "
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
	
		printf("\t-> Test for DvWhereCanTheyGoByType hunters (no road and rail distance is 0): ");
		
		char *trail =
			"GMA.... SGE.... HGE.... MGE.... DKL.V.. "
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
