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
#include "Map.h"
#include <stdlib.h>
#include <time.h> 
// #include <stdio.h>

#define DONT 	-1000
#define LIMIT	100
#define START_SCORE 1000

typedef struct hunter
{
	PlaceId currLoc;					// current location of player
	int health;							// current player health
	PlaceId *reachable;
	int nReach;
} hunterInfo;

typedef struct gameState 
{
	Round currRound;
	int score;

	hunterInfo hunterID[4];
	PlaceId allHunterLocs[NUM_REAL_PLACES * 4];
	int totalHunterLocs;

	PlaceId dangerHunterLocs[NUM_REAL_PLACES * 4];

	PlaceId currDracLoc;

	PlaceId *DracShouldGoLocs;
	int nShouldLocs;

	PlaceId *dracLocs;
	int nLocs;

	PlaceId *dracMoves;
	int nMoves;

	PlaceId dracHealth;

	PlaceId *activeTrapLocs;
	int nTraps;

	DraculaView dv;

} GameState;

int maxScoreIndex(int scoreMove[], int nDracPlays);
int calScore(DraculaView dv, PlaceId move);
bool isDbHide(PlaceId move);
bool castleArea(GameState gameInfo);
bool hunterNotMove(DraculaView dv, Player player);
bool doSouthLoop(GameState gameInfo);

void decideDraculaMove(DraculaView dv)
{
	char *play = "CD";
	registerBestPlay("BI", "Default");
	GameState gameInfo;

	time_t t;
	srand((unsigned) time(&t));

	gameInfo.dv = dv;

	// Get players current location
	gameInfo.hunterID[0].currLoc = DvGetPlayerLocation(dv, PLAYER_LORD_GODALMING);
	gameInfo.hunterID[1].currLoc = DvGetPlayerLocation(dv, PLAYER_DR_SEWARD);
	gameInfo.hunterID[2].currLoc = DvGetPlayerLocation(dv, PLAYER_VAN_HELSING);
	gameInfo.hunterID[3].currLoc = DvGetPlayerLocation(dv, PLAYER_MINA_HARKER);
	gameInfo.currDracLoc = DvGetPlayerLocation(dv, PLAYER_DRACULA);

	// Get players next reachable location
	gameInfo.hunterID[0].reachable = DvWhereCanTheyGo(dv, PLAYER_LORD_GODALMING, &gameInfo.hunterID[0].nReach); 
	gameInfo.hunterID[1].reachable = DvWhereCanTheyGo(dv, PLAYER_DR_SEWARD, &gameInfo.hunterID[1].nReach); 
	gameInfo.hunterID[2].reachable = DvWhereCanTheyGo(dv, PLAYER_VAN_HELSING, &gameInfo.hunterID[2].nReach); 
	gameInfo.hunterID[3].reachable = DvWhereCanTheyGo(dv, PLAYER_MINA_HARKER, &gameInfo.hunterID[3].nReach);
	gameInfo.dracLocs = DvWhereCanIGo(dv, &gameInfo.nLocs);
	gameInfo.dracMoves = DvGetValidMoves(dv, &gameInfo.nMoves);

	// Get players health
	gameInfo.hunterID[0].health = DvGetHealth(dv, PLAYER_LORD_GODALMING);
	gameInfo.hunterID[1].health = DvGetHealth(dv, PLAYER_DR_SEWARD);
	gameInfo.hunterID[2].health = DvGetHealth(dv, PLAYER_VAN_HELSING);
	gameInfo.hunterID[3].health = DvGetHealth(dv, PLAYER_MINA_HARKER);
	gameInfo.dracHealth = DvGetHealth(dv, PLAYER_DRACULA);

	gameInfo.currRound = DvGetRound(dv);
	gameInfo.score = DvGetScore(dv);
	gameInfo.activeTrapLocs = DvGetTrapLocations(dv, &gameInfo.nTraps);


    if (DvGetRound(dv) == 0) {
        int allMoves[NUM_REAL_PLACES];
        for (int i = 0; i < NUM_REAL_PLACES; i++) {
            PlaceId move = allMoves[i];
            allMoves[i] = calScore(dv, move);
            int score = allMoves[i];
            // Add additional scoring factors
            // 1) Consider if its on land
            if (placeIsLand(move)) score += 75;
            // 2) Go to CASTLE_DRACULA when health is critical
            if (move == CASTLE_DRACULA && !castleArea(gameInfo)) score += 10;
            allMoves[i] = score;
        }
        int i = maxScoreIndex(allMoves, NUM_REAL_PLACES);
        PlaceId move = DvConvertLocToMove(dv, allMoves[i]);
        char *play = (char *) placeIdToAbbrev(move);
        registerBestPlay(play, "begin");
        return;
    }


    // Get all possible locations all hunters can go to
    int nHunterPlays = 0;
    PlaceId hunterPlays[NUM_REAL_PLACES * 4];
    for (int player = PLAYER_LORD_GODALMING; player < PLAYER_DRACULA; player++) {
        int nPlayerLocs = 0;
        PlaceId *playerPlays = DvWhereCanTheyGoByType(dv, player, true, true, false, &nPlayerLocs);
        for (int i = 0; i < nPlayerLocs; i++) {
            hunterPlays[nHunterPlays] = playerPlays[i];
            nHunterPlays++;
        }
        free(playerPlays);
    }

    // Get all rechable locations from dracula
    int nDracPlays = 0;
    PlaceId *dracPlays = DvWhereCanIGo(dv, &nDracPlays);

    // Find how many locs of dracula overlap with current hunter location
    int badLoc = 0;
    for (int i = 0; i < nHunterPlays; i++) {
        for (int j = 0; j < nDracPlays; j++) {
            if (dracPlays[j] == hunterPlays[i]) {
                dracPlays[i] = DONT;
                badLoc++;
            }
        }
    }

    // Case where there is no more valid moves to make
    if (nDracPlays == 0) {
        registerBestPlay("TP", "Goodbye");
    }

    // Case where all valid moves may have a hunter encounter
    else if (nDracPlays == badLoc) {
        // Calculate how well the next move will be
        int scoreMove[nDracPlays];
        for (int i = 0; i < nDracPlays; i++) {
            PlaceId move = dracPlays[i];
            scoreMove[i] = calScore(dv, move);
        }
        int i = maxScoreIndex(scoreMove, nDracPlays);
        PlaceId move = DvConvertLocToMove(dv, dracPlays[i]);
        char *play = (char *) placeIdToAbbrev(move);
        registerBestPlay(play, "yolo v");
    }
    else {
        // Get all moves that cant be reach by hunters
        PlaceId shouldGo[nDracPlays];
        int nShouldGo = 0;
        for (int i = 0; i < nDracPlays; i++) {
            if (dracPlays[i] != DONT) {
                shouldGo[nShouldGo] = dracPlays[i];
                nShouldGo++;
            }
        }

        int scoreMove[nShouldGo];
        for (int i = 0; i < nShouldGo; i++) {
            PlaceId move = shouldGo[i];
            scoreMove[i] = calScore(dv, move);
            int score = scoreMove[i];
            // Add additional scoring factors
            // 1) Consider if its on land
            if (placeIsLand(move)) score += 75;
            // 2) Go to CASTLE_DRACULA when health is critical
            int pathLen = 0;
            PlaceId *pathToCastle = DvGetShortestPathTo(dv, CASTLE_DRACULA, &pathLen);
            if (DvGetHealth(dv, PLAYER_DRACULA) <= 30 && !castleArea(gameInfo) && pathLen > 0) {
                if (pathLen == 1) score += 100;
                if (move == pathToCastle[0]) score += 50;
            }
            free(pathToCastle);
            // 3) Check if hide or double back
            if (isDbHide(move)) score -= 150;
            // 4) Go to the south loop
            PlaceId *pathToMadrid = DvGetShortestPathTo(dv, MADRID, &pathLen);
            if (DvGetHealth(dv, PLAYER_DRACULA) > 30 && doSouthLoop(gameInfo) && pathLen == 1) {
                int nMove = 0;  bool canFree = true;
                PlaceId *lastMove = DvGetLastMoves(dv, PLAYER_DRACULA, 1, &nMove, &canFree);
                if (nMove == 1 && (lastMove[0] == LISBON || lastMove[0] == CADIZ || lastMove[0] == GRANADA ||
                    lastMove[0] == ALICANTE || lastMove[0] == MADRID || lastMove[0] == SANTANDER)) {
                        score += 100;
                    }
            }
            free(pathToMadrid);
            scoreMove[i] = score;
        }
        int i = maxScoreIndex(scoreMove, nDracPlays);
        PlaceId move = DvConvertLocToMove(dv, dracPlays[i]);
        char *play = (char *) placeIdToAbbrev(move);
        registerBestPlay(play, "yolo v");
    }
}
bool doSouthLoop(GameState gameInfo) {
    // Strategy One: Go to LS CA GR AL MA SN then repeat
    bool doSouthLoop = true;
    int nPlayers = 0;
    if (DvGetRound(gameInfo.dv) >= TRAIL_SIZE) {
        for (int i = PLAYER_LORD_GODALMING; i < PLAYER_DRACULA; i++) {
            for (int j = 0; j < gameInfo.hunterID[i].nReach; j++) {
                if (gameInfo.hunterID[i].reachable[j] == LISBON ||
                gameInfo.hunterID[i].reachable[j] == CADIZ ||
                gameInfo.hunterID[i].reachable[j] == GRANADA ||
                gameInfo.hunterID[i].reachable[j] == ALICANTE ||
                gameInfo.hunterID[i].reachable[j] == MADRID ||
                gameInfo.hunterID[i].reachable[j] == SANTANDER ||
                gameInfo.hunterID[i].reachable[j] == SARAGOSSA ||
                gameInfo.hunterID[i].reachable[j] == BARCELONA ||
                gameInfo.hunterID[i].reachable[j] == BORDEAUX ||
                gameInfo.hunterID[i].reachable[j] == TOULOUSE) {
                    doSouthLoop = false;
                    break;
                }
            }
        }
    }
    return doSouthLoop;
}


bool castleArea(GameState gameInfo)
{
	bool nearCastle = false;
	for (int i = 0; i < gameInfo.totalHunterLocs; i++) {
		if (gameInfo.allHunterLocs[i] == CASTLE_DRACULA ||
			gameInfo.allHunterLocs[i] == KLAUSENBURG	||
			gameInfo.allHunterLocs[i] == GALATZ 		|| 
			gameInfo.allHunterLocs[i] == BUCHAREST 		||
			gameInfo.allHunterLocs[i] == SZEGED 		||
			gameInfo.allHunterLocs[i] == CONSTANTA) 
		{
			nearCastle = true;
			break;
		}
	}
	if (gameInfo.currRound > 4 && !nearCastle) {
		for (int i = 0; i < PLAYER_DRACULA; i++) {
			int nMoves = 0;	bool canFree = true;
			PlaceId *moves = DvGetLastMoves(gameInfo.dv, i, TRAIL_SIZE, &nMoves, &canFree);
			for (int i = 0; i < nMoves - 1; i++) {
				if (moves[i] == moves[i + 1]) {
					nearCastle = false;
					break;
				}
				else nearCastle = true;
			}
			if (nearCastle) break;
			free(moves);
		}
	}
	return nearCastle;
}

int maxScoreIndex(int scoreMove[], int nDracPlays) {
    int min = 0;
    for (int i = 0; i < nDracPlays; i++) {
        if (scoreMove[i] > scoreMove[min]) {
            min = i;
        }
    }
    return min;
}

int calScore(DraculaView dv, PlaceId move) {
    int score = START_SCORE;

    if (isDbHide(move)) score -= 10;
    if (isDbHide(move) && placeIsSea(move)) score += 25;
    else if (placeIsSea(move)) score += 50;
    if (move == CASTLE_DRACULA && DvGetHealth(dv, PLAYER_DRACULA) < 20) score += 50;
    else if (move == CASTLE_DRACULA) score += 25;
    if (placeIsLand(move) && DvGetRound(dv) % 13 == 0) score += 5;
    if (move == CASTLE_DRACULA) score += 20;
    if (move != DvGetVampireLocation(dv)) score += 10;

    // Check how many hunters can go to that location
    for (int i = 0; i < 4; i++) {
        int nPlays = 0;
        PlaceId *hunterPlays = DvWhereCanTheyGoByType(dv, i, true, true, false, &nPlays);
        for (int j = 0; j < nPlays; j++) {
            if (move == hunterPlays[j]) score -= 30;
        }
        free(hunterPlays);
    }

    // Consider how many traps are in that location
    int nTraps = 0;
    PlaceId *traps = DvGetTrapLocations(dv, &nTraps);
    for (int i = 0; i < nTraps; i++) {
        if (move == traps[i]) {
            score += 10;
        }
    }
    free(traps);

    // Consider if dracula can take out the hunter
    int dracHealth = DvGetHealth(dv, PLAYER_DRACULA);
    // Check hunter health and go there
    for (int i = 0; i < 4; i++) {
        PlaceId currLoc = DvGetPlayerLocation(dv, i);
        int health = DvGetHealth(dv, i);
        if (health <= 4 && dracHealth > 30) score += 20;
    }

    // Consider if hunter has not moved and was disqualified
    for (int i = 0; i < 4; i++) {
        if (hunterNotMove(dv, i) && DvGetPlayerLocation(dv, i) != move) {
            score += 100;
        }
    }

    return score;
}

bool hunterNotMove(DraculaView dv, Player player) {
    if (DvGetRound(dv) >= TRAIL_SIZE) {
        bool notMove = true;
        int nMoves = 0; bool canFree = true;
        PlaceId *pastLocs = DvGetLastMoves(dv, player, TRAIL_SIZE, &nMoves, &canFree);
        for (int i = 0; i < nMoves - 1; i++) {
            if (pastLocs[i] != pastLocs[i + 1]) {
                notMove = false;
                break;
            }
        }
        if (canFree) free(pastLocs);
        return notMove;
    }
    return false;
}

bool isDbHide(PlaceId move) {
    if (move == DOUBLE_BACK_1 || move == DOUBLE_BACK_2 ||
        move == DOUBLE_BACK_3 || move == DOUBLE_BACK_4 ||
        move == DOUBLE_BACK_5 || move == HIDE)
    {
        return true;
    }
    return false;
}