////////////////////////////////////////////////////////////////////////
// Runs a player's game turn ...
//
// Can  produce  either a hunter player or a Dracula player depending on
// the setting of the I_AM_DRACULA #define
//
// This  is  a  dummy  version of the real player.c used when you submit
// your AIs. It is provided so that you can test whether  your  code  is
// likely to compile ...
//
// Note that this is used to drive both hunter and Dracula AIs. It first
// creates an appropriate view, and then invokes the relevant decideMove
// function,  which  should  use the registerBestPlay() function to send
// the move back.
//
// The real player.c applies a timeout, and will halt your  AI  after  a
// fixed  amount of time if it doesn 't finish first. The last move that
// your AI registers (using the registerBestPlay() function) will be the
// one used by the game engine. This version of player.c won't stop your
// decideMove function if it goes into an infinite loop. Sort  that  out
// before you submit.
//
// Based on the program by David Collien, written in 2012
//
// 2017-12-04	v1.1	Team Dracula <cs2521@cse.unsw.edu.au>
// 2018-12-31	v1.2	Team Dracula <cs2521@cse.unsw.edu.au>
// 2020-07-10	v1.3	Team Dracula <cs2521@cse.unsw.edu.au>
//
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Game.h"
#ifdef I_AM_DRACULA
# include "dracula.h"
# include "DraculaView.h"
#else
# include "hunter.h"
# include "HunterView.h"
#endif

// Moves given by registerBestPlay are this long (including terminator)
#define MOVE_SIZE 3

// The minimum static globals I can get away with
static char latestPlay[MOVE_SIZE] = "";
static char latestMessage[MESSAGE_SIZE] = "";

// A pseudo-generic interface, which defines
// - a type `View',
// - functions `ViewNew', `decideMove', `ViewFree',
// - a trail `xtrail', and a message buffer `xmsgs'.
#ifdef I_AM_DRACULA

typedef DraculaView View;

# define ViewNew DvNew
# define decideMove decideDraculaMove
# define ViewFree DvFree

// TODO: dracula test string
# define xPastPlays "GLS.... SLS.... HLS.... MLS...."
# define xMsgs { "", "", "", "" }

#else

typedef HunterView View;

# define ViewNew HvNew
# define decideMove decideHunterMove
# define ViewFree HvFree

# define xPastPlays "GSW.... SLS.... HMR.... MHA.... DPA...."
# define xMsgs { "", "", "" }

#endif

// Test case for immature vampire
// "GSW.... SLS.... HMR.... MHA.... DPA.V.. "
// "GLO.... SLS.... HMR.... MHA.... DC?T...";

// Test for the random moves
// "GSW.... SLS.... HMR.... MHA.... DC?.V.. "
// "GLO.... SLS.... HMR.... MHA.... DC?T... "
// "GEC.... SLS.... HMR.... MHA.... DC?T... "
// "GLE.... SLS.... HMR.... MHA.... DC?T... "
// "GPA.... SLS.... HMR.... MHA.... DC?T... "
// "GGE.... SLS.... HMR.... MHA.... DC?T... "
// "GGE.... SLS.... HMR.... MHA.... DC?T...";

// Test case for random moves

int main(void)
{
	// Replace this with the define instead
	char *pastPlays ="GLS.... SHA.... HDU.... MSZ.... DAO.... GSN.... SNS.... HGW.... MSZ.... DIR.... GSN.... SHA.... HDU.... MBE.... DSWT... GLS.... SNS.... HGW.... MSA.... DHIT... GSN.... SHA.... HDU.... MBE.... DD1T... GSR.... SNS.... HGW.... MBE.... DLVT... GSR.... SNS.... HGW.... MBE.... DMNT... GLS.... SAO.... HAO.... MBD.... DLOT... GAO.... SAO.... HAO.... MBR.... DSWT.M. GAO.... SAO.... HAO.... MHA.... DHIT.M. GAO.... SAO.... HAO.... MNS.... DD1T.M. GAO.... SAO.... HAO.... MAO.... DLVT.M. GAO.... SAO.... HAO.... MAO.... DMNT.M. GEC.... SEC.... HEC.... MEC....";
				    //   "GSW.... SLS.... HMR.... MHA.... DVI.V.. "
					//   "GLO.... SLS.... HMR.... MHA.... DC?T... " // BD
					//   "GEC.... SLS.... HMR.... MHA.... DC?T... " // SZ
					//   "GLE.... SLS.... HMR.... MHA.... DC?T... " // BE
					//   "GPA.... SLS.... HMR.... MHA.... DC?T... " // BC
					//   "GGE.... SLS.... HMR.... MHA.... DC?T... " // CN
					//   "GGE.... SLS.... HMR.... MHA.... DC?T.V. " // GA
					//   "GMI.... SAO.... HZU.... MBR.... DCDT.M. " // CD
					//   "GVE.... SNS.... HMU.... MBD.... DHIT.M. " // CD
					//   "GVI.... SHA.... HZA.... MKL.... DD1T.M. " // CD
					//   "GBD.... SVI.... HSZ.... MCDTTTD DC?T.M. " // KL
					//   "GKLTD.. SBD.... HKLD... MSZ.... DSZT.M. "
					//   "GSZTD.. SZA.... HSZD... MZA.... DC?T.M. " // BE
					//   "GZA.... SSZ.... HSZ.... MSZ.... DC?.V.. " // SO
					//   "GSZ.... SZA.... HZA.... MZA.... DC?T... " // BC
					//   "GZA.... SSZ.... HSZ.... MSZ.... DC?T... " // GA
					//   "GSZ.... SZA.... HZA.... MZA.... DCDT... "
					//   "GKL.... SSZ.... HSZ.... MSZ.... DHIT... "
					//   "GCDTTD. SKL.... HKL.... MKL.... DD1T.M. "
					//   "GCDT... SCDD... HCDD... MCDD... DC?T.V. " // KL
					//   "GSZ.... SKLTD..";  

	Message msgs[] = xMsgs;

	View state = ViewNew(pastPlays, msgs);
	decideMove(state);
	ViewFree(state);

	printf("Move: %s, Message: %s\n", latestPlay, latestMessage);
	return EXIT_SUCCESS;
}

// Saves characters from play (and appends a terminator)
// and saves characters from message (and appends a terminator)
void registerBestPlay(char *play, Message message)
{
	strncpy(latestPlay, play, MOVE_SIZE - 1);
	latestPlay[MOVE_SIZE - 1] = '\0';

	strncpy(latestMessage, message, MESSAGE_SIZE - 1);
	latestMessage[MESSAGE_SIZE - 1] = '\0';
}
// Simulation string
// GSW.... SLS.... HMR.... MHA.... DVI.V.. GLO.... SLS.... HMR.... MHA.... DBDT... GEC.... SLS.... HMR.... MHA.... DSZT... GLE.... SLS.... HMR.... MHA.... DBET... GPA.... SLS.... HMR.... MHA.... DBCT... GGE.... SLS.... HMR.... MHA.... DCNT... GGE.... SLS.... HMR.... MHA.... DGAT.V. GMI.... SAO.... HZU.... MBR.... DCDT.M. GVE.... SNS.... HMU.... MBD.... DHIT.M. GVI.... SHA.... HZA.... MKL.... DD1T.M. GBD.... SVI.... HSZ.... MCDTTTD DKLT.M. GKLTD.. SBD.... HKLD... MSZ.... DSZT.M. GSZTD.. SZA.... HSZD... MZA.... DBET.M. GZA.... SSZ.... HSZ.... MSZ.... DSO.V.. GSZ.... SZA.... HZA.... MZA.... DBCT... GZA.... SSZ.... HSZ.... MSZ.... DGAT... GSZ.... SZA.... HZA.... MZA.... DCDT... GKL.... SSZ.... HSZ.... MSZ.... DHIT... GCDTTD. SKL.... HKL.... MKL.... DD1T.M. GCDT... SCDD... HCDD... MCDD... DKLT.V. GSZ.... SKLTD..