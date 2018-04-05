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
		GameProcess(Player *p1, Player *p2, int num_ends, int rule_type);
		~GameProcess();

		const int rule_type_;  // Type of rule (0: standard, 1:mix_doubles) 

		// Send 'SETSTATE' command and 'POSITION' command
		bool SendState();

		GameState gs_;         // State of the game

		Player    *player1_;   // Player 1
		Player    *player2_;   // Player 2

	private:
	};

	// Split message as tokens
	std::vector<std::string> SpritAsTokens(char *message);
}