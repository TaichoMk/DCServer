#include "game_process.h"

using std::cerr;
using std::endl;

namespace digital_curling
{
	GameProcess::GameProcess(Player *p1, Player *p2, int num_ends, float random, int rule_type) : rule_type_(rule_type) {

		// Initialize state of the game
		memset(&gs_, 0, sizeof(GameState));
		gs_.LastEnd = num_ends;  // set LastEnd as number of ends

		// Set random number
		random_ = random;

		// Initialize bestshot and runshot
		memset(&best_shot_, 0, sizeof(ShotVec));
		memset(&run_shot_, 0, sizeof(ShotVec));

		// Set players
		player1_ = p1;
		player2_ = p2;
	}

	GameProcess::~GameProcess() {}

	// Send 'ISREADY' command and wait for recieving 'READYOK' from a player
	bool GameProcess::IsReady(Player *p) {
		// Send 'ISREADY' to Player
		p->Send("ISREADY");

		char msg[Player::kBufferSize];
		// Recieve "READYOK" from player1
		p->Recv(msg);
		// split message as token
		std::vector<std::string> tokens = digital_curling::SpritAsTokens(msg, " ");
		if (tokens.size() == 0) {
			cerr << "Error: empty message" << endl;
			return false;
		}
		if (tokens[0] != "READYOK") {
			cerr << "Error: invalid command '" << tokens[0] << "'" << endl;
			return false;
		}

		return true;
	}

	// Send 'NEWGAMWE' command to both player
	bool GameProcess::NewGame() {
		if (player1_ == nullptr || player2_ == nullptr) {
			return false;
		}

		//Send 'NEWGAMWE p1->name_ p2->name_' command to both player
		std::stringstream sstream;
		sstream << "NEWGAME " << player1_->name_ << " " << player2_->name_;
		player1_->Send(sstream.str().c_str());
		if (player1_ != player2_) {
			player2_->Send(sstream.str().c_str());
		}

		return true;
	}

	// Send 'SETSTATE' command and 'POSITION' command
	bool GameProcess::SendState() {
		if (player1_ == nullptr || player2_ == nullptr) {
			return false;
		}
		std::stringstream sstream;

		// Send SETSTATE command ('SETSTATE ShotNum CurEnd LastEnd WhiteToMove')
		sstream << "SETSTATE " << gs_.ShotNum << " " << gs_.CurEnd << " " << gs_.LastEnd << " " << gs_.WhiteToMove;
		player1_->Send(sstream.str().c_str());
		if (player1_ != player2_) {
			player2_->Send(sstream.str().c_str());
		}

		// Clear sstream
		sstream.str("");
		sstream.clear(std::stringstream::goodbit);

		// Send POSITION command ('POSITION body[0][0] body[0][1] body[1][0] body[1][1] ... body[15][0] body[15][0]')
		sstream << "POSITION";
		for (int i = 0; i < 16; i++) {
			sstream << " " <<  gs_.body[i][0] << " " << gs_.body[i][1];
		}
		player1_->Send(sstream.str().c_str());
		if (player1_ != player2_) {
			player2_->Send(sstream.str().c_str());
		}

		return true;
	}

	// Simulate a shot
	bool GameProcess::RunSimulation() {
		{ // temporary code
			cerr << "Simulating..." << endl;
			if (gs_.ShotNum < 15) {
				gs_.ShotNum++;
			}
			else {
				gs_.ShotNum = 0;
				gs_.CurEnd++;
			}
		}
		//Simulation(&gs_, best_shot_, random_, &run_shot_, -1);

		return true;
	}

	// Send 'SCORE' command
	bool GameProcess::SendScore() {
		if (player1_ == nullptr || player2_ == nullptr || gs_.CurEnd == 0) {
			return false;
		}
		std::stringstream sstream;
		sstream << "SCORE " << gs_.Score[gs_.CurEnd - 1];
		player1_->Send(sstream.str().c_str());
		if (player1_ != player2_) {
			player2_->Send(sstream.str().c_str());
		}

		return true;
	}

	// Split message as tokens (message will be destroyed)
	std::vector<std::string> SpritAsTokens(char *message, const char* const delim) {
		std::vector<std::string> tokens;
		char *ctx;
		char *token = strtok_s(message, delim, &ctx);
		while (token != nullptr) {
			tokens.push_back(token);
			token = strtok_s(nullptr, delim, &ctx);
		}
		return tokens;
	}
}