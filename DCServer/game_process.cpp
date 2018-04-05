#include "game_process.h"

using std::cerr;
using std::endl;

namespace digital_curling
{
	GameProcess::GameProcess(Player *p1, Player *p2, int num_ends, int rule_type) : rule_type_(rule_type) {
		// Initialize state of the game
		memset(&gs_, 0, sizeof(GameState));
		gs_.LastEnd = num_ends;  // set LastEnd as number of ends

		// Set players
		player1_ = p1;
		player2_ = p2;
	}

	GameProcess::~GameProcess() {}

	// Send 'SETSTATE' command and 'POSITION' command
	bool GameProcess::SendState() {
		std::stringstream sstream;

		// Send SETSTATE command ('SETSTATE ShotNum CurEnd LastEnd WhiteToMove')
		sstream << "SETSTATE " << gs_.ShotNum << " " << gs_.CurEnd << " " << gs_.LastEnd << " " << gs_.WhiteToMove;
		player1_->Send(sstream.str().c_str());
		//player2_->Send(sstream.str().c_str());

		// Clear sstream
		sstream.str("");
		sstream.clear(std::stringstream::goodbit);

		// Send POSITION command ('POSITION body[0][0] body[0][1] body[1][0] body[1][1] ... body[15][0] body[15][0]')
		sstream << "POSITION";
		for (int i = 0; i < 16; i++) {
			sstream << " " <<  gs_.body[i][0] << " " << gs_.body[i][1];
		}
		player1_->Send(sstream.str().c_str());
		//player2_->Send(sstream.str().c_str());

		return true;
	}

	// Split message as tokens (message will be destroyed)
	std::vector<std::string> SpritAsTokens(char *message) {
		std::vector<std::string> tokens;
		char *ctx;
		char *token = strtok_s(message, " ", &ctx);
		while (token != nullptr) {
			tokens.push_back(token);
			token = strtok_s(nullptr, " ", &ctx);
		}
		return tokens;
	}
}