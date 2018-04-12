#include "game_process.h"

#include <ctime>
#include <iostream>
#include <iomanip>
#include <random>
#include <sstream>

using std::cerr;
using std::endl;

namespace digital_curling
{
	GameProcess::GameProcess(Player *p1, Player *p2, int num_ends, float random, int rule_type) : rule_type_(rule_type), log_file_(p1, p2, random) {

		// Initialize state of the game
		memset(&gs_, 0, sizeof(GameState));
		gs_.LastEnd = num_ends;  // set LastEnd as number of ends

		// Set random number
		//random_ = random;

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

	// Prepare for End
	bool GameProcess::PrepareEnd() {
		if (player1_ == nullptr || player2_ == nullptr) {
			return false;
		}

		// Clear gs_.body
		memset(gs_.body, 0, 2 * 16 * sizeof(int));

		// For mix doubles rule
		if (rule_type_ == 1) {
			using namespace constant_num;
			const float   GUARD_OFFSET_X = 1.070f;
			const float   GUARD_OFFSET_Y = 2.286f;
			const SHOTPOS CENTER_HOUSE(kCenterX, kTeeY - kHouse4FootR - kStoneR, 0);
			const SHOTPOS CENTER_GUARD(kCenterX, kTeeY + kHouseR + GUARD_OFFSET_Y, 0);
			const SHOTPOS SIDE_HOUSE(kCenterX - kHouse8FootR, kTeeY + kStoneR, 0);
			const SHOTPOS SIDE_GUARD(kCenterX - GUARD_OFFSET_X, kTeeY + kHouseR + GUARD_OFFSET_Y, 0);

			// Set player which "PUTSTONE" command send to
			Player *player = (gs_.WhiteToMove) ? player1_ : player2_;

			// Send "PUTSTONE" command
			char msg[Player::kBufferSize];
			std::vector<std::string> tokens;
			player->Send("PUTSTONE");
			// Recieve from player
			//player->Recv(msg);

			// set putstone_type
			int putstone_type = 1;
			tokens = digital_curling::SpritAsTokens(msg, " ");
			if (tokens.size() == 0) {
				cerr << "Error: too few aguments in message: '" << msg << "'" << endl;
			}
			if (tokens[0] == "PUTSTONE") {
				if (tokens.size() >= 2) {
					putstone_type = atoi(tokens[1].c_str());
				}
			}
			switch (putstone_type)
			{
			case 0:
				gs_.body[0][0] = CENTER_GUARD.x;
				gs_.body[0][1] = CENTER_GUARD.y;
				gs_.body[1][0] = CENTER_HOUSE.x;
				gs_.body[1][1] = CENTER_HOUSE.y;
				gs_.WhiteToMove ^= 1;
				break;
			case 1:
				gs_.body[0][0] = CENTER_HOUSE.x;
				gs_.body[0][1] = CENTER_HOUSE.y;
				gs_.body[1][0] = CENTER_GUARD.x;
				gs_.body[1][1] = CENTER_GUARD.y;
				break;
			case 2:
				gs_.body[0][0] = SIDE_GUARD.x;
				gs_.body[0][1] = SIDE_GUARD.y;
				gs_.body[1][0] = SIDE_HOUSE.x;
				gs_.body[1][1] = SIDE_HOUSE.y;
				gs_.WhiteToMove ^= 1;
				break;
			case 3:
				gs_.body[0][0] = SIDE_HOUSE.x;
				gs_.body[0][1] = SIDE_HOUSE.y;
				gs_.body[1][0] = SIDE_GUARD.x;
				gs_.body[1][1] = SIDE_GUARD.y;
				break;
			default:
				gs_.body[0][0] = CENTER_HOUSE.x;
				gs_.body[0][1] = CENTER_HOUSE.y;
				gs_.body[1][0] = CENTER_GUARD.x;
				gs_.body[1][1] = CENTER_GUARD.y;
				break;
			}

			// Write dummy statements to logfile
			std::stringstream sstream;
			for (int i = 0; i < 6; i++) {
				// Clear sstream
				sstream.str("");
				sstream.clear(std::stringstream::goodbit);

				// Set dummy statements
				sstream << "[" << 
					std::setfill('0') << std::setw(2) << gs_.CurEnd <<
					std::setfill('0') << std::setw(2) << i << "]" << endl;
				sstream << "POSITION=POSITION";
				for (int i = 0; i < 16; i++) {
					sstream << " " << gs_.body[i][0] << " " << gs_.body[i][1];
				}
				sstream << endl;
				sstream << "SETSTATE=SETSTATE " << i << " " << gs_.CurEnd << " " << gs_.LastEnd << " " << gs_.WhiteToMove << endl;
				sstream << "BESTSHOT=BESTSHOT 0 0 0" << endl;
				sstream << "RUNSHOT=RUNSHOT 0 0 0";

				// Write to logfile
				log_file_.Write(sstream.str());
			}

			gs_.ShotNum = 6;
		}

		return true;
	}

	// Send 'SETSTATE' command and 'POSITION' command
	bool GameProcess::SendState() {
		if (player1_ == nullptr || player2_ == nullptr) {
			return false;
		}
		// Write "[0000]" statement to logfile
		std::stringstream sstream;
		sstream << "[" <<
			std::setfill('0') << std::setw(2) << gs_.CurEnd <<
			std::setfill('0') << std::setw(2) << gs_.ShotNum << "]";
		log_file_.Write(sstream.str());

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
		// Write to logfile
		log_file_.Write("POSITION=" + sstream.str());

		// Clear sstream
		sstream.str("");
		sstream.clear(std::stringstream::goodbit);

		// Send SETSTATE command ('SETSTATE ShotNum CurEnd LastEnd WhiteToMove')
		sstream << "SETSTATE " << gs_.ShotNum << " " << gs_.CurEnd << " " << gs_.LastEnd << " " << gs_.WhiteToMove;
		player1_->Send(sstream.str().c_str());
		if (player1_ != player2_) {
			player2_->Send(sstream.str().c_str());
		}
		// Write to logfile
		log_file_.Write("SETSTATE=" + sstream.str());

		return true;
	}

	// Send 'GO' command and wait for recieving 'BESTSHOT' from a player
	bool GameProcess::Go() {
		if (player1_ == nullptr || player2_ == nullptr) {
			return false;
		}
		// Prepare "GO" command
		std::stringstream sstream;
		sstream << "GO " << player1_->time_remain_ << " " << player2_->time_remain_;
		Player *next_player = (gs_.WhiteToMove == 0) ? player1_ : player2_;

		// get starting time
		time_t time_start = clock();

		// Send "GO" command
		next_player->Send(sstream.str().c_str());

		// Recieve message
		char msg[digital_curling::Player::kBufferSize];
		next_player->Recv(msg);

		// Write to logfile
		log_file_.Write("BESTSHOT=" + std::string(msg));  // TODO: move to appropriate place

		// Check timelimit
		time_t time_used = clock() - time_start;
		if (next_player->time_limit_ > 0) {
			if (time_used > next_player->time_remain_) {
				// TODO: jump to timeover and exit process
				cerr << "TimeOver" << endl;
				return false;
			}
			next_player->time_remain_ -= (int)time_used;
		}

		// Split message as token
		std::vector<std::string> tokens;
		tokens = digital_curling::SpritAsTokens(msg, " ");
		if (tokens.size() == 0) {
			cerr << "Error: too few aguments in message: '" << msg << "'" << endl;
			return false;
		}
		if (tokens[0] == "BESTSHOT") {
			// Set best_shot_ if command is 'BESTSHOT'
			if (tokens.size() < 4) {
				cerr << "Error: too few aguments in message: '" << msg << "'" << endl;
				return false;
			}
			best_shot_.x = (float)atof(tokens[1].c_str());
			best_shot_.y = (float)atof(tokens[2].c_str());
			best_shot_.angle = (bool)atoi(tokens[3].c_str());

			return true;
		}
		else if (tokens[0] == "CONSEED") {
			// Jump to conseed and exit process if command is 'CONSEED'
			// TODO: jump to conseed and exit process
			return false;
		}
		else {
			// Print error message and exit
			cerr << "Error: invalid command '" << tokens[0] << "'" << endl;
			return false;
		}
	}

	// Simulate a shot
	bool GameProcess::RunSimulation() {
		Player *p = (gs_.WhiteToMove) ? player2_ : player1_;
		Simulation(&gs_, best_shot_, p->random_x_, &run_shot_, -1);
		//Simmulation(&gs_, best_shot_, p->random_x_, p->random_y_, &run_shot, -1, nullptr, 0);

		// Write to log file
		std::stringstream sstream;
		sstream << "RUNSHOT=RUNSHOT " << run_shot_.x << ' ' << run_shot_.y << ' ' << run_shot_.angle;
		log_file_.Write(sstream.str());

		return true;
	}

	// Send 'SCORE' command
	bool GameProcess::SendScore() {
		if (player1_ == nullptr || player2_ == nullptr || gs_.CurEnd == 0) {
			return false;
		}

		std::stringstream sstream;

		// Write "[0016]" statement to logfile
		sstream << "[" <<
			std::setfill('0') << std::setw(2) << gs_.CurEnd-1 <<
			std::setfill('0') << std::setw(2) << 16 << "]" << endl;
		sstream << "POSITION=POSITION";
		for (int i = 0; i < 16; i++) {
			sstream << " " << gs_.body[i][0] << " " << gs_.body[i][1];
		}
		log_file_.Write(sstream.str());

		// Clear sstream
		sstream.str("");
		sstream.clear(std::stringstream::goodbit);

		// Sned "SCORE" command
		sstream << "SCORE " << gs_.Score[gs_.CurEnd - 1];
		player1_->Send(sstream.str().c_str());
		if (player1_ != player2_) {
			player2_->Send(sstream.str().c_str());
		}
		// Write to logfile
		log_file_.Write("SCORE=" + sstream.str());

		return true;
	}

	bool GameProcess::Exit() {
		if (player1_ == nullptr || player2_ == nullptr) {
			return false;
		}
		// Calclate total score
		int score_p1 = 0;
		int score_p2 = 0;
		for (int i = 0; i < gs_.LastEnd; i++) {
			if (gs_.Score[i] > 0) {
				score_p1 += gs_.Score[i];
			}
			else {
				score_p2 -= gs_.Score[i];
			}
		}
		if (score_p1 > score_p2) {
			player1_->Send("GAMEOVER WIN");
			if (player1_ != player2_) {
				player2_->Send("GAMEOVER LOSE");
			}
		}
		else if (score_p1 < score_p2) {
			player1_->Send("GAMEOVER LOSE");
			if (player1_ != player2_) {
				player2_->Send("GAMEOVER WIN");
			}
		}
		else {
			player1_->Send("GAMEOVER DRAW");
			if (player1_ != player2_) {
				player2_->Send("GAMEOVER DRAW");
			}
		}

		Sleep(10);

		// Exit Player Process
		if (player1_->ExitProcess() == 0) {
			cerr << "failed to player1_->ExitProcess()" << endl;
		}
		if (player1_ != player2_) {
			if (player2_->ExitProcess() == 0) {
				cerr << "failed to player2_->ExitProcess()" << endl;
			}
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