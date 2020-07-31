////////////////////////////////////////////////////////////////////////
// COMP2521 20T2 ... the Fury of Dracula
// testHunterView.c: test the HunterView ADT
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

// This is a testing file to determine if the implementation for HunterView.h
// ADT is correct and functional. Test cases were both supplied from the given
// code and written by Tam Do and Prathamesh Jagtap.

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Game.h"
#include "HunterView.h"
#include "Places.h"
#include "testUtils.h"

int main(void)
{
	{///////////////////////////////////////////////////////////////////
	
		printf("Basic initialisation\n");
		
		char *trail = "";
		Message messages[] = {};
		HunterView hv = HvNew(trail, messages);

		assert(HvGetRound(hv) == 0);
		assert(HvGetPlayer(hv) == PLAYER_LORD_GODALMING);
		assert(HvGetScore(hv) == GAME_START_SCORE);
		assert(HvGetHealth(hv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
		assert(HvGetHealth(hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert(HvGetVampireLocation(hv) == NOWHERE);

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("After Lord Godalming's turn\n");

		char *trail =
			"GST....";
		
		Message messages[1] = {};
		HunterView hv = HvNew(trail, messages);

		assert(HvGetRound(hv) == 0);
		assert(HvGetPlayer(hv) == PLAYER_DR_SEWARD);
		assert(HvGetScore(hv) == GAME_START_SCORE);
		assert(HvGetPlayerLocation(hv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(HvGetPlayerLocation(hv, PLAYER_DR_SEWARD) == NOWHERE);

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("After Van Helsing's turn\n");

		char *trail =
			"GST.... SAO.... HZU....";
		
		Message messages[3] = {};
		HunterView hv = HvNew(trail, messages);

		assert(HvGetRound(hv) == 0);
		assert(HvGetPlayer(hv) == PLAYER_MINA_HARKER);
		assert(HvGetScore(hv) == GAME_START_SCORE);
		assert(HvGetPlayerLocation(hv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(HvGetPlayerLocation(hv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(HvGetPlayerLocation(hv, PLAYER_VAN_HELSING) == ZURICH);
		assert(HvGetPlayerLocation(hv, PLAYER_MINA_HARKER) == NOWHERE);
		assert(HvGetPlayerLocation(hv, PLAYER_DRACULA) == NOWHERE);
		
		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("After Dracula's turn\n");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DC?.V..";
		
		Message messages[5] = {
			"Hello", "Goodbye", "Stuff", "...", "Mwahahahaha"
		};
		
		HunterView hv = HvNew(trail, messages);

		assert(HvGetRound(hv) == 1);
		assert(HvGetPlayer(hv) == PLAYER_LORD_GODALMING);
		assert(HvGetScore(hv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN);
		assert(HvGetHealth(hv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert(HvGetPlayerLocation(hv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(HvGetPlayerLocation(hv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(HvGetPlayerLocation(hv, PLAYER_VAN_HELSING) == ZURICH);
		assert(HvGetPlayerLocation(hv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(HvGetPlayerLocation(hv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(HvGetVampireLocation(hv) == CITY_UNKNOWN);
		Round round = -1;
		assert(HvGetLastKnownDraculaLocation(hv, &round) == NOWHERE);

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Encountering Dracula\n");

		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.V.. "
			"GGEVD..";
		
		Message messages[6] = {
			"Hello", "Goodbye", "Stuff", "...", "Mwahahahaha",
			"Aha!"
		};
		
		HunterView hv = HvNew(trail, messages);
		
		assert(HvGetHealth(hv, PLAYER_LORD_GODALMING) ==
				GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER);
		assert(HvGetHealth(hv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS - LIFE_LOSS_HUNTER_ENCOUNTER);
		assert(HvGetPlayerLocation(hv, PLAYER_LORD_GODALMING) == GENEVA);
		assert(HvGetPlayerLocation(hv, PLAYER_DRACULA) == GENEVA);
		assert(HvGetVampireLocation(hv) == NOWHERE);
		Round round = -1;
		assert(HvGetLastKnownDraculaLocation(hv, &round) == GENEVA);
		assert(round == 0);

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for Dracula doubling back at sea, "
			   "and losing blood points\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DS?.... "
			"GST.... SST.... HST.... MST.... DD1....";
		
		Message messages[10] = {
			"Party at Geneva", "Okay", "Sure", "Let's go", "Mwahahahaha",
			"", "", "", "", "Back I go"
		};
		
		HunterView hv = HvNew(trail, messages);

		assert(HvGetRound(hv) == 2);
		assert(HvGetPlayer(hv) == PLAYER_LORD_GODALMING);
		assert(HvGetScore(hv) == GAME_START_SCORE - 2 * SCORE_LOSS_DRACULA_TURN);
		assert(HvGetHealth(hv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS - (2 * LIFE_LOSS_SEA));
		assert(HvGetPlayerLocation(hv, PLAYER_DRACULA) == SEA_UNKNOWN);

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Testing a hunter 'dying'\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V.. "
			"GGE.... SGE.... HGE.... MGE.... DSTT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "
			"GSTTTTD";
		
		Message messages[21] = {};
		HunterView hv = HvNew(trail, messages);
		
		assert(HvGetScore(hv) == GAME_START_SCORE
		                         - 4 * SCORE_LOSS_DRACULA_TURN
		                         - SCORE_LOSS_HUNTER_HOSPITAL);
		assert(HvGetHealth(hv, PLAYER_LORD_GODALMING) == 0);
		assert(HvGetPlayerLocation(hv, PLAYER_LORD_GODALMING) == HOSPITAL_PLACE);
		assert(HvGetPlayerLocation(hv, PLAYER_DRACULA) == STRASBOURG);
		
		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Testing Dracula doubling back to Castle Dracula\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DCD.V.. "
			"GGE.... SGE.... HGE.... MGE.... DD1T...";
		
		Message messages[10] = {};
		HunterView hv = HvNew(trail, messages);
		
		assert(HvGetHealth(hv, PLAYER_DRACULA) ==
				GAME_START_BLOOD_POINTS + (2 * LIFE_GAIN_CASTLE_DRACULA));
		assert(HvGetPlayerLocation(hv, PLAYER_DRACULA) == CASTLE_DRACULA);
		
		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Testing vampire location\n");
		
		char *trail =
			"GVI.... SGE.... HGE.... MGE.... DCD.V.. "
			"GBD.... SGE.... HGE.... MGE.... DC?T... "
			"GSZ.... SGE.... HGE.... MGE.... DC?T... "
			"GSZ.... SGE.... HGE....";
		
		Message messages[18] = {};
		HunterView hv = HvNew(trail, messages);
		
		assert(HvGetPlayerLocation(hv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(HvGetVampireLocation(hv) == CASTLE_DRACULA);
		
		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Testing a vampire maturing\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V.. "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T.V.";
		
		Message messages[35] = {};
		HunterView hv = HvNew(trail, messages);
		
		assert(HvGetScore(hv) == GAME_START_SCORE
		                         - 7 * SCORE_LOSS_DRACULA_TURN
		                         - SCORE_LOSS_VAMPIRE_MATURES);
		assert(HvGetPlayerLocation(hv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(HvGetVampireLocation(hv) == NOWHERE);
		
		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////

		printf("Personal testing: get round 1\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V.. "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T.V.";

		Message messages[35] = {};
		HunterView hv = HvNew(trail, messages);

		assert(HvGetRound(hv) == 7);
		assert(HvGetPlayer(hv) == PLAYER_LORD_GODALMING);

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////

		printf("Personal testing: get round 2\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V.. "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE....";

		Message messages[19] = {};
		HunterView hv = HvNew(trail, messages);

		assert(HvGetRound(hv) == 3);
		assert(HvGetPlayer(hv) == PLAYER_DRACULA);

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////

		printf("Personal testing: get player 1\n");

		char *trail =
			"GGE....";

		Message messages[1] = {};
		HunterView hv = HvNew(trail, messages);

		assert(HvGetRound(hv) == 0);
		assert(HvGetPlayer(hv) == PLAYER_DR_SEWARD);

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////

		printf("Personal testing: get player 2\n");

		char *trail =
			"GGE.... SGE....";

		Message messages[2] = {};
		HunterView hv = HvNew(trail, messages);

		assert(HvGetRound(hv) == 0);
		assert(HvGetPlayer(hv) == PLAYER_VAN_HELSING);

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////

		printf("Personal testing: get player 3\n");

		char *trail =
			"GGE.... SGE.... HGE....";

		Message messages[3] = {};
		HunterView hv = HvNew(trail, messages);

		assert(HvGetRound(hv) == 0);
		assert(HvGetPlayer(hv) == PLAYER_MINA_HARKER);

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////

		printf("Personal testing: get player 4\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE....";

		Message messages[4] = {};
		HunterView hv = HvNew(trail, messages);

		assert(HvGetRound(hv) == 0);
		assert(HvGetPlayer(hv) == PLAYER_DRACULA);

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////

		printf("Personal testing: get player 5\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V..";

		Message messages[5] = {};
		HunterView hv = HvNew(trail, messages);

		assert(HvGetRound(hv) == 1);
		assert(HvGetPlayer(hv) == PLAYER_LORD_GODALMING);
		assert(HvGetPlayerLocation(hv, PLAYER_DRACULA) == CITY_UNKNOWN);

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////

		printf("Personal testing: get immvampire location when dead\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DZU.V.. "
			"GGE.... SGE.... HGE.... MZU.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MGE.... DC?T...";
			
		Message messages[20] = {};
		HunterView hv = HvNew(trail, messages);

		assert(HvGetRound(hv) == 4);
		assert(HvGetPlayer(hv) == PLAYER_LORD_GODALMING);
		assert(HvGetPlayerLocation(hv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(HvGetVampireLocation(hv) == NOWHERE);

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

	}

	{///////////////////////////////////////////////////////////////////

		printf("Personal testing: get immvampire location when dead 2\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DZU.V.. "
			"GGE.... SGE.... HGE.... MGE.... DC?T... "
			"GGE.... SGE.... HGE.... MZU.... DC?T... "
			"GGE.... SGE.... HGE.... MZU.... DC?T...";
			
		Message messages[20] = {};
		HunterView hv = HvNew(trail, messages);

		assert(HvGetRound(hv) == 4);
		assert(HvGetPlayer(hv) == PLAYER_LORD_GODALMING);
		assert(HvGetPlayerLocation(hv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(HvGetVampireLocation(hv) == NOWHERE);

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Testing Dracula's last known location 1\n");
		
		char *trail =
			"GGE.... SGE.... HVI.... MGE.... DCD.V.. "
			"GGE.... SGE.... HBD.... MGE.... DKLT... "
			"GGE.... SGE.... HSZ.... MGE.... DC?T... "
			"GGE.... SGE.... HKLT... MGE.... DC?T... "
			"GGE.... SGE.... HCDV... MGE.... DD1T...";
	
		Message messages[25] = {};
		HunterView hv = HvNew(trail, messages);
		
		assert(HvGetPlayerLocation(hv, PLAYER_DRACULA) == CITY_UNKNOWN);
		Round round = -1;
		assert(HvGetLastKnownDraculaLocation(hv, &round) == KLAUSENBURG);
		assert(round == 1);
		
		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("-> Testing everything\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DPA.V.. "
			"GGE.... SGE.... HGE.... MGE.... DSTT... "
			"GGE.... SGE.... HGE.... MGE.... DHIT... "
			"GGE.... SGE.... HGE.... MGE.... DD1T... "
			"GSTTTTD SGE.... HGE.... MGE.... DTPT... "
			"GSZ.... SGE.... HGE.... MGE.... DCDT... "
			"GSZ.... SGE.... HGE.... MGE.... DCDT.V.";
		
		Message messages[35] = {};
		HunterView hv = HvNew(trail, messages);
		
		assert(HvGetScore(hv) == GAME_START_SCORE
		                         - 7 * SCORE_LOSS_DRACULA_TURN
		                         - SCORE_LOSS_VAMPIRE_MATURES
								 - SCORE_LOSS_HUNTER_HOSPITAL);
		assert(HvGetHealth(hv, PLAYER_LORD_GODALMING) 
						      == GAME_START_HUNTER_LIFE_POINTS);
		assert(HvGetPlayerLocation(hv, PLAYER_DRACULA) == CASTLE_DRACULA);
		assert(HvGetVampireLocation(hv) == NOWHERE);
		
		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////

		printf("\t-> Testing various cases with a large pastPlay string\n");
		
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
			"GAO.... STS.... HMS.... MTS.... DHI.... "
			"GBB.... SMS.... HAO.... MMS.... DHI.V.. "
			"GNA.... SAO.... HEC.... MAO.... DMNT... "
			"GBO.... SIR.... HLE.... MEC.... DD2T... "
			"GSR.... SDU.... HBU.... MPL.... DHIT... "
			"GSN.... SIR.... HAM.... MLO.... DTPT... "
			"GAL.... SAO.... HCO.... MEC.... DD3T... "
			"GMS.... SMS.... HFR.... MLE.... DKLT.V. "
			"GTS.... STS.... HBR.... MCO.... DGAT.M. "
			"GIO.... SIO.... HBD.... MLI.... DD3T.M. "
			"GBS.... SBS.... HKLT... MBR.... DHI..M. "
			"GCN.... SCN.... HCDTTTD MVI.... DD5T... "
			"GGAT... SGA.... HSZ.... MBC.... DCDT... "
			"GCDTTD. SCDD... HKL.... MGA.... DKLT... "
			"GSZ.... SKLTD.. HKLD... MKLD... DBC.V.. "
			"GBD.... SBE.... HGA.... MBCVD.. DCDT... "
			"GSZ.... SSOTD.. HBC.... MSOD... DHIT...";
		
		Message messages[140] = {};
		HunterView hv = HvNew(trail, messages);
		
		assert(HvGetRound(hv) == 29);
		assert(HvGetPlayerLocation(hv, PLAYER_LORD_GODALMING) == SZEGED);
		assert(HvGetPlayerLocation(hv, PLAYER_DR_SEWARD) == SOFIA);
		assert(HvGetPlayerLocation(hv, PLAYER_VAN_HELSING) == BUCHAREST);
		assert(HvGetPlayerLocation(hv, PLAYER_DRACULA) == CASTLE_DRACULA);
		assert(HvGetVampireLocation(hv) == NOWHERE);
		Round round = -1;
		assert(HvGetLastKnownDraculaLocation(hv, &round) == CASTLE_DRACULA);
		assert(round == 28);

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

	}

	{///////////////////////////////////////////////////////////////////

		printf("\t-> Testing hunter dying to a trap\n");
		
		char *trail =
			"GPA.... SPA.... HPA.... MPA.... DST.V.. "
			"GSTVD.. SPA.... HPA.... MPA.... DZUT... "
			"GGE.... SPA.... HPA.... MPA.... DMIT... "
			"GZUT... SPA.... HPA.... MPA.... DVET... "
			"GMIT... SPA.... HPA.... MPA.... DHIT... "
			"GVET... SPA.... HPA.... MPA.... DS?....";
		
		Message messages[30] = {};
		HunterView hv = HvNew(trail, messages);
		
		assert(HvGetRound(hv) == 6);
		assert(HvGetScore(hv) == 354);
		assert(HvGetHealth(hv, PLAYER_LORD_GODALMING) == 0);
		assert(HvGetHealth(hv, PLAYER_DR_SEWARD) == 9);
		assert(HvGetHealth(hv, PLAYER_VAN_HELSING) == 9);
		assert(HvGetHealth(hv, PLAYER_MINA_HARKER) == 9);
		assert(HvGetPlayerLocation(hv, PLAYER_LORD_GODALMING) == HOSPITAL_PLACE);
		assert(HvGetPlayerLocation(hv, PLAYER_DR_SEWARD) == PARIS);
		assert(HvGetPlayerLocation(hv, PLAYER_VAN_HELSING) == PARIS);
		assert(HvGetPlayerLocation(hv, PLAYER_DRACULA) == SEA_UNKNOWN);
		assert(HvGetVampireLocation(hv) == NOWHERE);
		Round round = -1;
		assert(HvGetLastKnownDraculaLocation(hv, &round) == VENICE);
		assert(round == 4);

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

	}

	{///////////////////////////////////////////////////////////////////

		printf("\t-> Testing Dracula NOWHERE\n");
		
		char *trail =
			"GVE.... SPA.... HZA.... MBR.... DC?.V..";
		
		Message messages[30] = {};
		HunterView hv = HvNew(trail, messages);
		
		assert(HvGetRound(hv) == 1);
		assert(HvGetScore(hv) == 365);
		assert(HvGetHealth(hv, PLAYER_LORD_GODALMING) == 9);
		assert(HvGetHealth(hv, PLAYER_DR_SEWARD) == 9);
		assert(HvGetHealth(hv, PLAYER_VAN_HELSING) == 9);
		assert(HvGetHealth(hv, PLAYER_MINA_HARKER) == 9);
		assert(HvGetPlayerLocation(hv, PLAYER_LORD_GODALMING) == VENICE);
		assert(HvGetPlayerLocation(hv, PLAYER_DR_SEWARD) == PARIS);
		assert(HvGetPlayerLocation(hv, PLAYER_VAN_HELSING) == ZAGREB);
		assert(HvGetPlayerLocation(hv, PLAYER_MINA_HARKER) == BERLIN);
		assert(HvGetPlayerLocation(hv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(HvGetVampireLocation(hv) == CITY_UNKNOWN);
		Round round = -1;
		assert(HvGetLastKnownDraculaLocation(hv, &round) == NOWHERE);
		assert(round == -1);

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

	}

	{///////////////////////////////////////////////////////////////////

		printf("\t-> Testing Dracula revealed location through research\n");
		
		char *trail =
			"GVE.... SPA.... HZA.... MBR.... DC?.V.. "
			"GVE.... SPA.... HZA.... MBR.... DMUT...";
		
		Message messages[30] = {};
		HunterView hv = HvNew(trail, messages);
		
		assert(HvGetRound(hv) == 2);
		assert(HvGetScore(hv) == 364);
		assert(HvGetHealth(hv, PLAYER_LORD_GODALMING) == 9);
		assert(HvGetHealth(hv, PLAYER_DR_SEWARD) == 9);
		assert(HvGetHealth(hv, PLAYER_VAN_HELSING) == 9);
		assert(HvGetHealth(hv, PLAYER_MINA_HARKER) == 9);
		assert(HvGetPlayerLocation(hv, PLAYER_LORD_GODALMING) == VENICE);
		assert(HvGetPlayerLocation(hv, PLAYER_DR_SEWARD) == PARIS);
		assert(HvGetPlayerLocation(hv, PLAYER_VAN_HELSING) == ZAGREB);
		assert(HvGetPlayerLocation(hv, PLAYER_MINA_HARKER) == BERLIN);
		assert(HvGetPlayerLocation(hv, PLAYER_DRACULA) == MUNICH);
		assert(HvGetVampireLocation(hv) == CITY_UNKNOWN);
		Round round = -1;
		assert(HvGetLastKnownDraculaLocation(hv, &round) == MUNICH);
		assert(round == 1);

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

	}

	{///////////////////////////////////////////////////////////////////

		printf("\t-> Testing last known Dracula location\n");
		
		char *trail =
			"GKL.... SKL.... HGA.... MGA.... DCD.V.. "
			"GCDVD.. SCDD... HCDD... MCDD... DKLT... "
			"GKLTD..";
		
		Message messages[30] = {};
		HunterView hv = HvNew(trail, messages);
		
		assert(HvGetRound(hv) == 2);
		assert(HvGetScore(hv) == 358);
		assert(HvGetHealth(hv, PLAYER_LORD_GODALMING) == 0);
		assert(HvGetHealth(hv, PLAYER_DR_SEWARD) == 5);
		assert(HvGetHealth(hv, PLAYER_VAN_HELSING) == 5);
		assert(HvGetHealth(hv, PLAYER_MINA_HARKER) == 5);
		assert(HvGetPlayerLocation(hv, PLAYER_LORD_GODALMING) == HOSPITAL_PLACE);
		assert(HvGetPlayerLocation(hv, PLAYER_DR_SEWARD) == CASTLE_DRACULA);
		assert(HvGetPlayerLocation(hv, PLAYER_VAN_HELSING) == CASTLE_DRACULA);
		assert(HvGetPlayerLocation(hv, PLAYER_MINA_HARKER) == CASTLE_DRACULA);
		assert(HvGetPlayerLocation(hv, PLAYER_DRACULA) == KLAUSENBURG);
		assert(HvGetVampireLocation(hv) == NOWHERE);
		Round round = -1;
		assert(HvGetLastKnownDraculaLocation(hv, &round) == KLAUSENBURG);
		assert(round == 1);

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");

	}

	{///////////////////////////////////////////////////////////////////
		
		printf("Testing shortest path 1\n");
		
		char *trail =
			"GLS.... SLS.... HSW.... MMR.... DCD.V..";
		
		Message messages[5] = {};
		HunterView hv = HvNew(trail, messages);
		
		{
			printf("\tLisbon -> Barcelona (Lord Godalming, Round 1)\n");
			int pathLength = -1;
			PlaceId *path = HvGetShortestPathTo(hv, PLAYER_LORD_GODALMING,
			                                    BARCELONA, &pathLength);
			assert(pathLength == 2);
			assert(path[0] == MADRID);
			assert(path[1] == BARCELONA);
			free(path);
		}
		
		{
			printf("\tLisbon -> Cologne (Lord Godalming, Round 1)\n");
			int pathLength = -1;
			PlaceId *path = HvGetShortestPathTo(hv, PLAYER_LORD_GODALMING,
			                                    COLOGNE, &pathLength);
			assert(pathLength == 3);
			assert(path[0] == MADRID);
			assert(path[1] == BORDEAUX);
			assert(path[2] == COLOGNE);
			free(path);
		}
		
		{
			printf("\tSwansea -> Hamburg (Van Helsing, Round 1)\n");
			int pathLength = -1;
			PlaceId *path = HvGetShortestPathTo(hv, PLAYER_VAN_HELSING,
			                                    HAMBURG, &pathLength);
			assert(pathLength == 3);
			assert(path[0] == EDINBURGH);
			assert(path[1] == NORTH_SEA);
			assert(path[2] == HAMBURG);
			free(path);
		}
		
		{
			printf("\tMarseilles -> Constanta (Mina Harker, Round 1)\n");
			int pathLength = -1;
			PlaceId *path = HvGetShortestPathTo(hv, PLAYER_MINA_HARKER,
			                                    CONSTANTA, &pathLength);
			assert(pathLength == 4);
			assert(path[0] == GENOA || path[0] == MILAN);
			assert(path[1] == VENICE);
			assert(path[2] == BUDAPEST);
			assert(path[3] == CONSTANTA);
			free(path);
		}
		
		{
			printf("\tLisbon -> Castle Dracula (Dr. Seward, Round 1)\n");
			int pathLength = -1;
			PlaceId *path = HvGetShortestPathTo(hv, PLAYER_DR_SEWARD,
			                                    CASTLE_DRACULA, &pathLength);
			assert(pathLength == 7);
			assert(path[0] == SARAGOSSA);
			assert(path[1] == MARSEILLES);
			assert(path[2] == GENOA || path[2] == MILAN);
			assert(path[3] == VENICE);
			assert(path[4] == BUDAPEST);
			assert(path[5] == GALATZ || path[5] == KLAUSENBURG);
			assert(path[6] == CASTLE_DRACULA);
			free(path);
		}
		
		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}

	{
		char *trail =
			"GCA.... SNU.... HSW.... MMR.... DCD.V.. "
			"GCA.... SNU.... HTO.... MBC.... DCD....";
		
		Message messages[5] = {};
		HunterView hv = HvNew(trail, messages);

		{
			printf("\tPersonal testing: Nuremberg -> Berlin (Dr. Seward, Round 2)\n");
			int pathLength = -1;
			PlaceId *path = HvGetShortestPathTo(hv, PLAYER_DR_SEWARD,
												BERLIN, &pathLength);
			assert(HvGetRound(hv) == 2);
			assert(pathLength == 1);
			assert(path[0] == BERLIN);
			free(path);
		}

		{
			printf("\tPersonal testing: Toulouse -> Constanta (Van Helsing, Round 2)\n");
			int pathLength = -1;
			PlaceId *path = HvGetShortestPathTo(hv, PLAYER_VAN_HELSING,
												CONSTANTA, &pathLength);
			assert(pathLength == 6);
			assert(HvGetRound(hv) == 2);
			assert(path[0] == MARSEILLES);
			assert(path[1] == GENOA);
			assert(path[2] == TYRRHENIAN_SEA);
			assert(path[3] == IONIAN_SEA);
			assert(path[4] == BLACK_SEA);
			assert(path[5] == CONSTANTA);
			free(path);
		}

		{
			printf("\tPersonal testing: Bucharest -> Hamburg (Mina Harker, Round 2)\n");
			int pathLength = -1;
			PlaceId *path = HvGetShortestPathTo(hv, PLAYER_MINA_HARKER, HAMBURG, 
																   &pathLength);
			assert(pathLength == 3);
			assert(HvGetRound(hv) == 2);
			assert(path[0] == SZEGED);
			assert(path[1] == VIENNA);
			assert(path[2] == HAMBURG);
			free(path);
		}

		{
			printf("\tPersonal testing: Bucharest -> Hamburg (Mina Harker, Round 2)\n");
			int pathLength = -1;
			PlaceId *path = HvGetShortestPathTo(hv, PLAYER_MINA_HARKER, 
												 CLERMONT_FERRAND, &pathLength);
			assert(pathLength == 6);
			assert(HvGetRound(hv) == 2);
			assert(path[0] == SZEGED);
			assert(path[1] == BUDAPEST);
			assert(path[2] == VENICE);
			assert(path[3] == MILAN);
			assert(path[4] == GENEVA);
			assert(path[5] == CLERMONT_FERRAND);
			free(path);
		}

		{
			printf("\tPersonal testing: Bucharest -> Paris (Mina Harker, Round 2)\n");
			int pathLength = -1;
			PlaceId *path = HvGetShortestPathTo(hv, PLAYER_MINA_HARKER, PARIS, 
																&pathLength);
			assert(pathLength == 5);
			assert(HvGetRound(hv) == 2);
			assert(path[0] == SZEGED);
			assert(path[1] == VIENNA);
			assert(path[2] == MUNICH);
			assert(path[3] == STRASBOURG);
			assert(path[4] == PARIS);
			free(path);
		}

		{
			printf("\tPersonal testing: Bucharest -> Santander (Mina Harker, Round 2)\n");
			int pathLength = -1;
			PlaceId *path = HvGetShortestPathTo(hv, PLAYER_MINA_HARKER, 
														SANTANDER, &pathLength);
			assert(pathLength == 7);
			assert(HvGetRound(hv) == 2);
			assert(path[0] == SZEGED);
			assert(path[1] == BUDAPEST);
			assert(path[2] == VENICE);
			assert(path[3] == MILAN);
			assert(path[4] == MARSEILLES);
			assert(path[5] == BORDEAUX);
			assert(path[6] == SANTANDER);
			free(path);
		}

		{
			printf("\tPersonal testing: Bucharest -> Santander (Mina Harker, Round 2)\n");
			int pathLength = -1;
			PlaceId *path = HvGetShortestPathTo(hv, PLAYER_MINA_HARKER, 
														SANTANDER, &pathLength);
			assert(pathLength == 7);
			assert(HvGetRound(hv) == 2);
			assert(path[0] == SZEGED);
			assert(path[1] == BUDAPEST);
			assert(path[2] == VENICE);
			assert(path[3] == MILAN);
			assert(path[4] == MARSEILLES);
			assert(path[5] == BORDEAUX);
			assert(path[6] == SANTANDER);
			free(path);
		}

		{
			printf("\tPersonal testing: Bucharest -> Edinburgh (Mina Harker, Round 2)\n");
			int pathLength = -1;
			PlaceId *path = HvGetShortestPathTo(hv, PLAYER_MINA_HARKER, 
														EDINBURGH, &pathLength);
			assert(pathLength == 5);
			assert(HvGetRound(hv) == 2);
			assert(path[0] == SZEGED);
			assert(path[1] == VIENNA);
			assert(path[2] == HAMBURG);
			assert(path[3] == NORTH_SEA);
			assert(path[4] == EDINBURGH);
			free(path);
		}

		{
			printf("\tPersonal testing: Bucharest -> Bucharest (Mina Harker, Round 2)\n");
			int pathLength = -1;
			PlaceId *path = HvGetShortestPathTo(hv, PLAYER_MINA_HARKER, 
														BUCHAREST, &pathLength);
			assert(pathLength == 1);
			assert(HvGetRound(hv) == 2);
			assert(path[0] == BUCHAREST);
			free(path);
		}

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Checking Galatz road connections "
		       "(Lord Godalming, Round 1)\n");
		
		char *trail = "GGA....";
		Message messages[1] = {};
		HunterView hv = HvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = HvWhereCanTheyGoByType(hv, PLAYER_LORD_GODALMING,
		                                       true, false, false, &numLocs);
		
		assert(numLocs == 5);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BUCHAREST);
		assert(locs[1] == CASTLE_DRACULA);
		assert(locs[2] == CONSTANTA);
		assert(locs[3] == GALATZ);
		assert(locs[4] == KLAUSENBURG);
		free(locs);
		
		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Checking Paris rail connections "
		       "(Lord Godalming, Round 1)\n");
		
		char *trail = "GPA....";
		Message messages[1] = {};
		HunterView hv = HvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = HvWhereCanTheyGoByType(hv, PLAYER_LORD_GODALMING,
		                                       false, true, false, &numLocs);
		
		assert(numLocs == 5);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BORDEAUX);
		assert(locs[1] == BRUSSELS);
		assert(locs[2] == LE_HAVRE);
		assert(locs[3] == MARSEILLES);
		assert(locs[4] == PARIS);
		free(locs);
		
		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Checking Adriatic Sea boat connections "
		       "(Lord Godalming, Round 1)\n");
		
		char *trail = "GAS....";
		Message messages[1] = {};
		HunterView hv = HvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = HvWhereCanTheyGoByType(hv, PLAYER_LORD_GODALMING,
		                                       false, false, true, &numLocs);
		
		assert(numLocs == 4);
		sortPlaces(locs, numLocs);
		assert(locs[0] == ADRIATIC_SEA);
		assert(locs[1] == BARI);
		assert(locs[2] == IONIAN_SEA);
		assert(locs[3] == VENICE);
		free(locs);
		
		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");
		printf("\033[0m");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Checking Szeged road connections "
		       "(Dracula, Round 1)\n");
		
		char *trail =
			"GSZ.... SGE.... HGE.... MGE.... DSZ.V..";
		
		Message messages[5] = {};
		HunterView hv = HvNew(trail, messages);
		
		int numLocs = -1;
		PlaceId *locs = HvWhereCanTheyGoByType(hv, PLAYER_DRACULA,
		                                       true, false, false, &numLocs);
		assert(numLocs == 5);
		sortPlaces(locs, numLocs);
		assert(locs[0] == BELGRADE);
		assert(locs[1] == BUDAPEST);
		assert(locs[2] == KLAUSENBURG);
		assert(locs[3] == SZEGED);
		assert(locs[4] == ZAGREB);
		free(locs);
		
		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");	
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Testing HvWhereCanTheyGo\n");
		
		char *trail =
			"GSZ.... SGE.... HGE.... MGE.... DBE.V..";
		
		Message messages[5] = {};
		HunterView hv = HvNew(trail, messages);
		
		{
			printf("\tPersonal testing: Lord Godalming\n");
			int numLocations = -1;
			PlaceId *places = HvWhereCanTheyGo(hv, PLAYER_LORD_GODALMING, 
																&numLocations);
			assert(numLocations == 11);
			assert(HvGetRound(hv) == 1);
			assert(places[0] == BUDAPEST);
			assert(places[1] == BUCHAREST);
			assert(places[2] == BELGRADE);
			assert(places[3] == VIENNA);
			assert(places[4] == GALATZ);
			assert(places[5] == CONSTANTA);
			assert(places[6] == SOFIA);
			assert(places[7] == ZAGREB);
			assert(places[8] == ST_JOSEPH_AND_ST_MARY);
			assert(places[9] == KLAUSENBURG);
			assert(places[10] == SZEGED);
			free(places);
		}

		{
			printf("\tPersonal testing: Dr. Seward\n");
			int numLocations = -1;
			PlaceId *places = HvWhereCanTheyGo(hv, PLAYER_DR_SEWARD, 
																&numLocations);
			assert(numLocations == 10);
			assert(HvGetRound(hv) == 1);
			assert(places[0] == MILAN);
			assert(places[1] == ZURICH);
			assert(places[2] == GENOA);
			assert(places[3] == FLORENCE);
			assert(places[4] == STRASBOURG);
			assert(places[5] == ROME);
			assert(places[6] == PARIS);
			assert(places[7] == MARSEILLES);
			assert(places[8] == CLERMONT_FERRAND);
			assert(places[9] == GENEVA);
			free(places);
		}

		{
			printf("\tPersonal testing: Van Helsing\n");
			int numLocations = -1;
			PlaceId *places = HvWhereCanTheyGo(hv, PLAYER_VAN_HELSING, 
																&numLocations);
			assert(numLocations == 6);
			assert(HvGetRound(hv) == 1);
			assert(places[0] == ZURICH);
			assert(places[1] == STRASBOURG);
			assert(places[2] == PARIS);
			assert(places[3] == MARSEILLES);
			assert(places[4] == CLERMONT_FERRAND);
			assert(places[5] == GENEVA);
			free(places);
		}

		{
			printf("\tPersonal testing: Dracula 1\n");
			int numLocations = -1;
			PlaceId *places = HvWhereCanTheyGo(hv, PLAYER_DRACULA, 
																&numLocations);
			assert(numLocations == 6);
			assert(HvGetRound(hv) == 1);
			assert(places[0] == SZEGED);
			assert(places[1] == SOFIA);
			assert(places[2] == SARAJEVO);
			assert(places[3] == KLAUSENBURG);
			assert(places[4] == BUCHAREST);
			assert(places[5] == BELGRADE);
			free(places);
		}

		{
			char *trail =
			"GSZ.... SGE.... HGE.... MGE.... DDU.V..";
		
			Message messages[5] = {};
			HunterView hv = HvNew(trail, messages);

			printf("\tPersonal testing: Dracula 2\n");
			int numLocations = -1;
			PlaceId *places = HvWhereCanTheyGo(hv, PLAYER_DRACULA, 
																&numLocations);
			assert(numLocations == 3);
			assert(HvGetRound(hv) == 1);
			assert(places[0] == GALWAY);
			assert(places[1] == IRISH_SEA);
			assert(places[2] == DUBLIN);
			free(places);
			HvFree(hv);
		}

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");	
		printf("\033[0m");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Testing HvWhereCanTheyGoByType\n");
		
		char *trail =
			"GSZ.... SGE.... HCN.... MIO.... DBE.V..";
		
		Message messages[5] = {};
		HunterView hv = HvNew(trail, messages);
		
		{
			printf("\tPersonal testing: Lord Godalming\n");
			int numLocations = -1;
			PlaceId *places = HvWhereCanTheyGoByType(hv, PLAYER_LORD_GODALMING, 
											true, false, true, &numLocations);
			assert(numLocations == 6);
			assert(HvGetRound(hv) == 1);
			sortPlaces(places, numLocations);
			assert(places[0] == BELGRADE);
			assert(places[1] == BUDAPEST);
			assert(places[2] == KLAUSENBURG);
			assert(places[3] == ST_JOSEPH_AND_ST_MARY);
			assert(places[4] == SZEGED);
			assert(places[5] == ZAGREB);
			free(places);
		}

		{
			printf("\tPersonal testing: Dr. Seward\n");
			int numLocations = -1;
			PlaceId *places = HvWhereCanTheyGoByType(hv, PLAYER_DR_SEWARD, 
											true, true, false, &numLocations);
			assert(numLocations == 10);
			assert(HvGetRound(hv) == 1);
			assert(places[0] == MILAN);
			assert(places[1] == ZURICH);
			assert(places[2] == GENOA);
			assert(places[3] == FLORENCE);
			assert(places[4] == STRASBOURG);
			assert(places[5] == ROME);
			assert(places[6] == PARIS);
			assert(places[7] == MARSEILLES);
			assert(places[8] == CLERMONT_FERRAND);
			assert(places[9] == GENEVA);
			free(places);
		}

		{
			printf("\tPersonal testing: Van Helsing\n");
			int numLocations = -1;
			PlaceId *places = HvWhereCanTheyGoByType(hv, PLAYER_VAN_HELSING, 
											false, true, true, &numLocations);
			assert(numLocations == 2);
			assert(HvGetRound(hv) == 1);
			sortPlaces(places, numLocations);
			assert(places[0] == BLACK_SEA);
			assert(places[1] == CONSTANTA);
			free(places);
		}

		{
			printf("\tPersonal testing: Dracula 1\n");
			int numLocations = -1;
			PlaceId *places = HvWhereCanTheyGoByType(hv, PLAYER_MINA_HARKER, 
											false, false, true, &numLocations);
			assert(numLocations == 7);
			assert(HvGetRound(hv) == 1);
			sortPlaces(places, numLocations);
			assert(places[0] == ADRIATIC_SEA);
			assert(places[1] == ATHENS);
			assert(places[2] == BLACK_SEA);
			assert(places[3] == IONIAN_SEA);
			assert(places[4] == SALONICA);
			assert(places[5] == TYRRHENIAN_SEA);
			assert(places[6] == VALONA);
			free(places);
		}

		{
			char *trail =
			"GSZ.... SGE.... HGE.... MGE.... DDU.V..";
		
			Message messages[5] = {};
			HunterView hv = HvNew(trail, messages);

			printf("\tPersonal testing: Dracula 2\n");
			int numLocations = -1;
			PlaceId *places = HvWhereCanTheyGoByType(hv, PLAYER_DRACULA, 
											true, false, true, &numLocations);
			assert(numLocations == 3);
			assert(HvGetRound(hv) == 1);
			sortPlaces(places, numLocations);
			assert(places[0] == DUBLIN);
			assert(places[1] == GALWAY);
			assert(places[2] == IRISH_SEA);
			free(places);
			HvFree(hv);
		}

		HvFree(hv);
		printf("\033[1;32m");
		printf("Test passed!\n");	
		printf("\033[0m");
	}
	
	return EXIT_SUCCESS;
}
