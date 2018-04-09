#pragma once

#include "CurlingSimulator.h"
#include "dc_util.h"
#include "game_player.h"

#include <string>
#include <sstream>
#include <vector>

namespace digital_curling
{
	// DigitalCrling game
	class GameProcess {
	public:
		GameProcess(Player *p1, Player *p2, int num_ends, float random, int rule_type);
		~GameProcess();

		const int rule_type_;  // Type of rule (0: standard, 1:mix_doubles) 

		// Send 'NEWGAMWE' command to both player
		bool NewGame();

		// Send 'SETSTATE' command and 'POSITION' command to both player
		bool SendState();

		// Simulate a shot
		bool RunSimulation();

		// Send 'SCORE' command
		bool SendScore();

		GameState gs_;         // State of the game
		float     random_;     // Size of random number

		ShotVec best_shot_;    // Shot vector recieved
		ShotVec run_shot_;     // Shot vector (add random)

		Player    *player1_;   // Player 1
		Player    *player2_;   // Player 2

	};

	// Split message as tokens
	std::vector<std::string> SpritAsTokens(char *message, const char* const delim);
}