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
	{///////////////////////////////////////////////////////////////////	
		printf("Basic initialisation\n");

		char *trail = "";
		Message messages[] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 0);
		assert(GvGetPlayer(gv) == PLAYER_LORD_GODALMING);
		assert(GvGetScore(gv) == GAME_START_SCORE);
		assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
		assert(GvGetHealth(gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == NOWHERE);
		assert(GvGetVampireLocation(gv) == NOWHERE);

		GvFree(gv);
		printf("Test passed!\n");
	}

	
	{///////////////////////////////////////////////////////////////////
	
		printf("After Lord Godalming's turn\n");

		char *trail =
			"GST....";
		
		Message messages[1] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 0);
		assert(GvGetPlayer(gv) == PLAYER_DR_SEWARD);
		// assert(GvGetScore(gv) == GAME_START_SCORE);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(GvGetPlayerLocation(gv, PLAYER_DR_SEWARD) == NOWHERE);

		GvFree(gv);
		printf("Test passed!\n");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("After Mina Harker's turn\n");

		char *trail =
			"GST.... SAO.... HZU.... MBB....";
		
		Message messages[4] = {};
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 0);
		assert(GvGetPlayer(gv) == PLAYER_DRACULA);
		// assert(GvGetScore(gv) == GAME_START_SCORE);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(GvGetPlayerLocation(gv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(GvGetPlayerLocation(gv, PLAYER_VAN_HELSING) == ZURICH);
		assert(GvGetPlayerLocation(gv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == NOWHERE);

		GvFree(gv);
		printf("Test passed!\n");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("After Dracula's turn\n");

		char *trail =
			"GST.... SAO.... HZU.... MBB.... DC?.V..";
		
		Message messages[] = {
			"Hello", "Goodbye", "Stuff", "...", "Mwahahahaha"
		};
		
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 1);
		assert(GvGetPlayer(gv) == PLAYER_LORD_GODALMING);
		// assert(GvGetScore(gv) == GAME_START_SCORE - SCORE_LOSS_DRACULA_TURN);
		// assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
		// assert(GvGetHealth(gv, PLAYER_DRACULA) == GAME_START_BLOOD_POINTS);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == STRASBOURG);
		assert(GvGetPlayerLocation(gv, PLAYER_DR_SEWARD) == ATLANTIC_OCEAN);
		assert(GvGetPlayerLocation(gv, PLAYER_VAN_HELSING) == ZURICH);
		assert(GvGetPlayerLocation(gv, PLAYER_MINA_HARKER) == BAY_OF_BISCAY);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(GvGetVampireLocation(gv) == CITY_UNKNOWN);

		GvFree(gv);
		printf("Test passed!\n");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Encountering Dracula\n");

		char *trail =
			"GST.... SAO.... HCD.... MAO.... DGE.V.. "
			"GGEVD..";
		
		Message messages[] = {
			"Hello", "Goodbye", "Stuff", "...", "Mwahahahaha",
			"Aha!"
		};
		
		GameView gv = GvNew(trail, messages);
		
		// assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) ==
		// 		GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_DRACULA_ENCOUNTER);
		// assert(GvGetHealth(gv, PLAYER_DRACULA) ==
		// 		GAME_START_BLOOD_POINTS - LIFE_LOSS_HUNTER_ENCOUNTER);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == GENEVA);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == GENEVA);
		assert(GvGetVampireLocation(gv) == NOWHERE);

		GvFree(gv);
		printf("Test passed\n");
	}

	{///////////////////////////////////////////////////////////////////
	
		printf("Test for Dracula doubling back at sea, "
		       "and losing blood points (Hunter View)\n");

		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DS?.... "
			"GST.... SST.... HST.... MST.... DD1....";
		
		Message messages[] = {
			"Party at Geneva", "Okay", "Sure", "Let's go", "Mwahahahaha",
			"", "", "", "", "Back I go"
		};
		
		GameView gv = GvNew(trail, messages);

		assert(GvGetRound(gv) == 2);
		assert(GvGetPlayer(gv) == PLAYER_LORD_GODALMING);
		// assert(GvGetScore(gv) == GAME_START_SCORE - 2 * SCORE_LOSS_DRACULA_TURN);
		// assert(GvGetHealth(gv, PLAYER_DRACULA) ==
		// 		GAME_START_BLOOD_POINTS - (2 * LIFE_LOSS_SEA));
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == SEA_UNKNOWN);

		GvFree(gv);
		printf("Test passed!\n");
	}


	{///////////////////////////////////////////////////////////////////
	
		printf("Test for Dracula doubling back at sea, "
			   "and losing blood points (Dracula View)\n");

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
		// assert(GvGetScore(gv) == GAME_START_SCORE - 2 * SCORE_LOSS_DRACULA_TURN);
		// assert(GvGetHealth(gv, PLAYER_DRACULA) ==
		// 		GAME_START_BLOOD_POINTS - (2 * LIFE_LOSS_SEA));
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == ENGLISH_CHANNEL);

		GvFree(gv);
		printf("Test passed!\n");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Checking that hunters' health points are capped\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DC?.V.. "
			"GGE....";
	
		Message messages[6] = {};
		GameView gv = GvNew(trail, messages);
	
		// assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) == GAME_START_HUNTER_LIFE_POINTS);
		
		GvFree(gv);
		printf("Test passed!\n");
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
		GameView gv = GvNew(trail, messages);
		
		// assert(GvGetScore(gv) == GAME_START_SCORE
		//                          - 4 * SCORE_LOSS_DRACULA_TURN
		//                          - SCORE_LOSS_HUNTER_HOSPITAL);
		// assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) == 0);
		// TODO: error
		// assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == HOSPITAL_PLACE);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == STRASBOURG);
		
		GvFree(gv);
		printf("Test passed!\n");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Testing Dracula doubling back to Castle Dracula\n");
		
		char *trail =
			"GGE.... SGE.... HGE.... MGE.... DCD.V.. "
			"GGE.... SGE.... HGE.... MGE.... DD1T...";
		
		Message messages[10] = {};
		GameView gv = GvNew(trail, messages);
		
		// assert(GvGetHealth(gv, PLAYER_DRACULA) ==
		// 		GAME_START_BLOOD_POINTS + (2 * LIFE_GAIN_CASTLE_DRACULA));
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CASTLE_DRACULA);
		
		GvFree(gv);
		printf("Test passed!\n");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Testing vampire/trap locations\n");
		
		char *trail =
			"GVI.... SGE.... HGE.... MGE.... DCD.V.. "
			"GBD.... SGE.... HGE.... MGE.... DKLT... "
			"GSZ.... SGE.... HGE.... MGE.... DGAT... "
			"GSZ.... SGE.... HGE.... MGE....";
		
		Message messages[19] = {};
		GameView gv = GvNew(trail, messages);
		
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == GALATZ);
		assert(GvGetVampireLocation(gv) == CASTLE_DRACULA);
		// int numTraps = 0;
		// PlaceId *traps = GvGetTrapLocations(gv, &numTraps);
		// assert(numTraps == 2);
		// sortPlaces(traps, numTraps);
		// assert(traps[0] == GALATZ && traps[1] == KLAUSENBURG);
		// free(traps);
		
		GvFree(gv);
		printf("Test passed!\n");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		printf("Testing trap locations after one is destroyed\n");
		
		char *trail =
			"GVI.... SGE.... HGE.... MGE.... DBC.V.. "
			"GBD.... SGE.... HGE.... MGE.... DKLT... "
			"GSZ.... SGE.... HGE.... MGE.... DGAT... "
			"GBE.... SGE.... HGE.... MGE.... DCNT... "
			"GKLT... SGE.... HGE.... MGE....";
		
		Message messages[24] = {};
		GameView gv = GvNew(trail, messages);
		
		// assert(GvGetHealth(gv, PLAYER_LORD_GODALMING) ==
		// 		GAME_START_HUNTER_LIFE_POINTS - LIFE_LOSS_TRAP_ENCOUNTER);
		assert(GvGetPlayerLocation(gv, PLAYER_LORD_GODALMING) == KLAUSENBURG);
		assert(GvGetVampireLocation(gv) == BUCHAREST);
		// int numTraps = 0;
		// PlaceId *traps = GvGetTrapLocations(gv, &numTraps);
		// assert(numTraps == 2);
		// sortPlaces(traps, numTraps);
		// assert(traps[0] == CONSTANTA && traps[1] == GALATZ);
		// free(traps);
		
		GvFree(gv);
		printf("Test passed!\n");
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
		GameView gv = GvNew(trail, messages);
		
		// assert(GvGetScore(gv) == GAME_START_SCORE
		//                          - 7 * SCORE_LOSS_DRACULA_TURN
		//                          - SCORE_LOSS_VAMPIRE_MATURES);
		assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CITY_UNKNOWN);
		assert(GvGetVampireLocation(gv) == NOWHERE);
		
		GvFree(gv);
		printf("Test passed!\n");
	}
	
	{///////////////////////////////////////////////////////////////////
	
		
	// 	printf("============ Testing move/location history ============\n");
	
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
		
		// assert(GvGetHealth(gv, PLAYER_DR_SEWARD) ==
		// 		GAME_START_HUNTER_LIFE_POINTS - 2 * LIFE_LOSS_TRAP_ENCOUNTER);
		// assert(GvGetPlayerLocation(gv, PLAYER_DRACULA) == CITY_UNKNOWN);
		// assert(GvGetVampireLocation(gv) == NOWHERE);

		printf("===> Testing GvGetMoveHistory and GvGetLastMoves Functions\n");
		printf("==> Test Cases:\n");
		
		// Test Case: Lord Godalming (player is before current player)
		printf("-> Lord Godalming's entire move history: ");
		{
			int numMoves = 0; bool canFree = false;
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
		printf("Test Passed!\n");
		
		printf("-> Lord Godalming's last n move history: ");
		{
			int lastNMoves = 3;
			int numMoves = 0; bool canFree = false;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_LORD_GODALMING,
											lastNMoves, &numMoves, &canFree);
			assert(numMoves == 3);
			assert(moves[0] == SARAGOSSA);
			assert(moves[1] == SANTANDER);
			assert(moves[2] == MADRID);
			if (canFree) free(moves);
		}
		printf("Test Passed!\n");

		// Test Case: Dr Seward (player is the next to current player)
		printf("-> Dr Seward's entire move history: ");
		{
			int numMoves = 0; bool canFree = false;
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
		printf("Test Passed!\n");
		
		printf("-> Dr Seward's last n move history: ");
		{
			int lastNMoves = 3;
			int numMoves = 0; bool canFree = false;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_DR_SEWARD,
											lastNMoves, &numMoves, &canFree);
			assert(numMoves == 3);
			assert(moves[0] == PARIS);
			assert(moves[1] == STRASBOURG);
			assert(moves[2] == FLORENCE);
			if (canFree) free(moves);
		}
		printf("Test Passed!\n");

		// Test Case: Dr Van Helsing (player is current player)
		printf("-> Dr Van Helsing's entire move history: ");
		{
			int numMoves = 0; bool canFree = false;
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
		printf("Test Passed!\n");
		
		printf("-> Dr Van Helsing's last n move history: ");
		{
			int lastNMoves = 3;
			int numMoves = 0; bool canFree = false;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_VAN_HELSING,
											lastNMoves, &numMoves, &canFree);
			assert(numMoves == 3);
			assert(moves[0] == NANTES);
			assert(moves[1] == PARIS);
			assert(moves[2] == STRASBOURG);
			if (canFree) free(moves);
		}
		printf("Test Passed!\n");

		// Test Case: Dracula (player after current player)
		printf("-> Dracula's entire move history: ");
		{
			int numMoves = 0; bool canFree = false;
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
		printf("Test Passed!\n");
		
		printf("-> Dracula's last n move history: ");
		{
			int lastNMoves = 3;
			int numMoves = 0; bool canFree = false;
			PlaceId *moves = GvGetLastMoves(gv, PLAYER_DRACULA,
											lastNMoves, &numMoves, &canFree);
			assert(numMoves == 3);
			assert(moves[0] == DOUBLE_BACK_3);
			assert(moves[1] == HIDE);
			assert(moves[2] == CITY_UNKNOWN);
			if (canFree) free(moves);
		}
		printf("Test Passed!\n");
		
		printf("===> Testing GvGetLocationHistory and GvGetLastLocations Functions\n");
		printf("==> Test Cases:\n");

		// Test Case: Hunters
		// Test to see GvGetLocationHistory functions the same as GvGetLastMoves
		// for hunters
		printf("-> Dr Van Helsing's entire location history: ");
		{
			int numLocs = 0; bool canFree = false;
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
		printf("Test Passed!\n");

		// Test Case: Dracula
		printf("-> Dracula's entire location history: ");
		{
			int numLocs = 0; bool canFree = false;
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
		printf("Test Passed!\n");

		GvFree(gv);

		// Test with teleport
		char *newTrail =
			"GLS.... SLS.... HLS.... MGE.... DST.V.. "
			"GCA.... SAL.... HAL.... MGE.... DTPT... "
			"GGR.... SBO.... HBO.... MGE.... DC?T... "
			"GAL.... SNA.... HNA.... MGE.... DD2T... "
			"GSR.... SPA.... HPA.... MGE.... DHIT... "
			"GSN.... SST.... HST.... MGE.... DC?T... "
			"GMA.... SFLTTV.";
		GameView gvNew = GvNew(newTrail, messages);

		// Test Case: Dracula (player after current player)
		printf("-> Dracula's entire move history with teleport: ");
		{
			int numMoves = 0; bool canFree = false;
			PlaceId *moves = GvGetMoveHistory(gvNew, PLAYER_DRACULA,
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
		printf("Test Passed!\n");

		printf("-> Dracula's entire location history with teleport: ");
		{
			int numLocs = 0; bool canFree = false;
			PlaceId *locs = GvGetLocationHistory(gvNew, PLAYER_DRACULA,
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
		printf("Test Passed!\n");

		// printf("-> Dracula's last N location history with teleport: ");
		// {
		// 	int lastNMoves = 3;
		// 	int numLocs = 0; bool canFree = false;
		// 	PlaceId *locs = GvGetLastLocations(gvNew, PLAYER_DRACULA, lastNMoves,
		// 	                                     &numLocs, &canFree);
		// 	assert(numLocs == 3);
		// 	assert(locs[0] == CASTLE_DRACULA);
		// 	assert(locs[1] == CASTLE_DRACULA);
		// 	assert(locs[2] == CITY_UNKNOWN);
		// 	if (canFree) free(locs);
		// }
		// printf("Test Passed!\n");

		GvFree(gvNew);

	} 

	{///////////////////////////////////////////////////////////////////
	
		printf("Testing connections\n");
		
		char *trail = "";
		Message messages[] = {};
		GameView gv = GvNew(trail, messages);
		printf("===> Testing GvGetReachable Functions\n");

		{
			printf("\tChecking Draculas moveable\n");
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
		}

		{
			printf("\tChecking Hunter rail connections\n");
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

		}

		{
			printf("\tChecking Dracula can't reach the hospital\n");
			int numLocs = -1;
			PlaceId *locs = GvGetReachable(gv, PLAYER_DRACULA,
										1, BELGRADE, &numLocs);
			for (int i = 0; i < numLocs; i++) {
				assert(locs[i] != HOSPITAL_PLACE);
			}
		}

		// {
		// 	printf("Checking Galatz all possible connections "
		// 	       "(Lord Godalming, Round 1)\n");
		// 	int numLocs = -1;
		// 	PlaceId *locs = GvGetReachable(gv, PLAYER_LORD_GODALMING,
		// 	                            	3, ALICANTE, &numLocs);
		// 	sortPlaces(locs, numLocs);
		// 	for (int i = 0; i < numLocs; i++) {
		// 		printf("%d\n", locs[i]);
		// 	}
		// 	free(locs);
		// }

		{
			printf("\tChecking Draculas Rail connections (none)\n");
			int numLocs = -1;
			PlaceId *locs = GvGetReachableByType(gv, PLAYER_DRACULA,
											1, BORDEAUX, false, true,
											false, &numLocs);
			assert(numLocs == 1);
			assert(locs[1] == BORDEAUX);
		}

		{
			printf("\tChecking rounds where hunters cant use RAIL"
					"LORD_GODALMING ROUND 4\n");
			int numLocs = -1;
			PlaceId *locs = GvGetReachableByType(gv, PLAYER_LORD_GODALMING,
												4, PARIS, false, true,
												false, &numLocs);
			assert(numLocs == 1);
			assert(locs[1] == PARIS);
		}

		{
			printf("\tChecking Galatz road connections "
			       "(Lord Godalming, Round 1)\n");
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

		{
			printf("\tChecking Ionian Sea boat connections "
			       "(Lord Godalming, Round 1)\n");
			
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

		{
			printf("\tChecking Paris rail connections "
			       "(Lord Godalming, Round 2)\n");
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
		
		{
			printf("\tChecking Athens rail connections (none)\n");
			int numLocs = -1;
			PlaceId *locs = GvGetReachableByType(gv, PLAYER_LORD_GODALMING,
			                                     1, ATHENS, false, true,
			                                     false, &numLocs);
			
			assert(numLocs == 1);
			assert(locs[0] == ATHENS);
			free(locs);
		}

	// own test
		

		GvFree(gv);
		printf("Test passed!\n");
	}

	return EXIT_SUCCESS;
}
