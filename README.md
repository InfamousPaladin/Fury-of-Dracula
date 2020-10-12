# fury-of-dracula
This repository is for COMP2521 20T2 Assignment 2. Team project by Gabriel Ting (gtangelo), Christian Ilagan (cmilagan), Tam Do (InfamousPaladin) and Prathamesh Jagtap (praths7).

NOTE: Game engine which ran this game was provided by UNSW CSE. This project contains the interactions to the gamestate through an ADT and the implementation of the Dracula and Hunter AI.

This project is based on a real game called **The Fury of Dracula**, but the rules have been simplified. This project involves setting up the infrastructure and creating an AI to interact with the gamestate to make a move. The Hunter AI main goal is to hunt down Dracula whilst the Dracula AI aims to evade the hunters.

This project involves two components:
- View Phase: This involved designing and building an ADT that will allow player characters to gain access to the current state of the game. There is one ADT for the hunters (all hunters behave the same) and one ADT for Dracula. The player AIs must then work out what’s happening in the game via this ADT, which will give them an appropriately restricted view of the game state.
- Hunt Phase: Implemented an AI for both the Hunter characters and Dracula to decide the appropriate moveset for the given turn. Interacts with the gamesate through the ADT which influences the AI's move decision.
