#include <windows.h>

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

#include "game_process.h"

using std::cerr;
using std::endl;
using std::string;

namespace digital_curling {

	// CUI server
	int CuiServer() { return 0; }

	// Simple server for DigitalCurling
	int SimpleServer() {
		// Open config file
		const string config_file_path = "config.txt";
		std::vector<string> config_params[3];
		//                               [0][0] : type of rules (0: standard, 1:mix doubles)
		//                               [0][1] : number of ends (1 - 10)
		//                               [0][2] : size of random X (default 0.145)
		//                               [0][3] : size of random Y (default 0.145)
		//                               [1][0] : type of player1 (0: Local AI, 1: Network AI)
		//                               [1][1] : path of .exe file of player1
		//                               [1][2] : timelimit of player1 (msec, 0:infinite)
		//                               [2][0] : type of player1 (0: Local AI, 1: Network AI)
		//                               [2][1] : path of .exe file of player1
		//                               [2][2] : timelimit
		std::ifstream config_file(config_file_path);
		if (!config_file.is_open()) {
			cerr << "failed to open " << config_file_path << endl;
			return 0;
		}
		// Read config file
		string str_in;
		int i = 0;
		while (std::getline(config_file, str_in)) {  // read a line from config_file
			std::stringstream sstring(str_in);
			string param;
			while (std::getline(sstring, param, ',')) {  // sprit as token by delimiter ','
														 //cerr << "param[" << i << "][" << config_params[i].size() << "] = " << param << endl;
				config_params[i].push_back(param);
			}
			i++;
		}

		// Load 'CurlingSimulator.dll
		//string library_path = "CurlingSimulator.dll";
		//if (!LoadFunction(library_path)) {
		//	cerr << "failed to open " << library_path << endl;
		//	return 0;
		//}

		// Initialise LocalPlayer 1
		digital_curling::Player *p1;
		p1 = new digital_curling::LocalPlayer(
			config_params[1][1], 
			atoi(config_params[1][2].c_str()),
			(float)atof(config_params[0][2].c_str()),
			(float)atof(config_params[0][3].c_str()));
		if (p1->InitProcess() == 0) {
			cerr << "failed to create process for player 1" << endl;
			return 0;
		}
		// Initialise LocalPlayer 2
		digital_curling::Player *p2;
		if (config_params[1][1] == config_params[2][1]
			&& atoi(config_params[1][2].c_str()) == 0 && atoi(config_params[2][2].c_str()) == 0) {
			// Create only one process if player1 == player2 and both timelimit == 0
			p2 = p1;
		}
		else {
			p2 = new digital_curling::LocalPlayer(
				config_params[2][1],
				atoi(config_params[2][2].c_str()),
				(float)atof(config_params[0][2].c_str()),
				(float)atof(config_params[0][3].c_str()));
			if (p2->InitProcess() == 0) {
				cerr << "failed to create process for player 2" << endl;
				return 0;
			}
		}
		Sleep(10);  // MAGIC NUMBER: wait for process created

		// Create GameProcess
		digital_curling::GameProcess game_process(
			p1,
			p2,
			atoi(config_params[0][1].c_str()),
			(float)atof(config_params[0][2].c_str()),
			atoi(config_params[0][0].c_str())
		);

		// Send "ISREADY" to player1
		if (!game_process.IsReady(game_process.player1_)) {
			return 0;
		}
		if (game_process.player1_ != game_process.player2_) {
			if (!game_process.IsReady(game_process.player2_)) {
				return 0;
			}
		}

		// Send "NEWGAME" to players
		game_process.NewGame();
		Sleep(10);  // MAGIC NUMBER: wait for NewGame

		while (game_process.gs_.CurEnd < game_process.gs_.LastEnd) {

			// Prepare for End
			game_process.PrepareEnd();

			do {
				// Send "SETSTATE" and "POSITION" to players
				game_process.SendState();
				Sleep(10);  // MAGIC NUMBER: wait for SendState;

				// Send "GO" to player
				game_process.Go();

				// Simulation
				game_process.RunSimulation();
				Sleep(10);  // MAGIC NUMBER: wait for Simulation;
			} while (game_process.gs_.ShotNum < 16);

			// Send 'SCORE' to players
			game_process.SendScore();
			game_process.gs_.Clear();
			game_process.gs_.CurEnd++;
			Sleep(10);  // MAGIC NUMBER: wait for SendScore;
		}

		// Exit Game
		game_process.Exit();

		return 1;
	}

}

int main(void)
{
	// run single game
	digital_curling::SimpleServer();

	return 0;
}