#include "main.h"

using std::cerr;
using std::endl;
using std::string;

// Simple server for DigitalCurling
int main(void)
{
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

	// Initialise LocalPlayer 1
	digital_curling::Player *p1;
	p1 = new digital_curling::LocalPlayer(config_params[1][1], atoi(config_params[1][2].c_str()));
	if (p1->InitProcess() == 0) {
		cerr << "failed to create process for player 1" << endl;
		return 0;
	}
	// Initialise LocalPlayer 2
	digital_curling::Player *p2;
	if (config_params[1][1] == config_params[2][1]) {
		// Create only one process if player1 == player2
		p2 = p1;
	}
	else {
		p2 = new digital_curling::LocalPlayer(config_params[2][1], atoi(config_params[2][2].c_str()));
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
	game_process.player1_->kBufferSize;
	char msg[digital_curling::Player::kBufferSize];

	// send message to player1
	game_process.player1_->Send("ISREADY");

	game_process.player1_->Recv(msg);
	// split message as token
	std::vector<std::string> tokens = digital_curling::SpritAsTokens(msg, " ");
	if (tokens.size() == 0) {
		cerr << "Error: empty message" << endl;
		return 0;
	}
	if (tokens[0] != "READYOK") {
		cerr << "Error: invalid command '" << tokens[0] << "'" << endl;
		return 0;
	}
	game_process.SendState();
	Sleep(10);  // MAGIC NUMBER: wait for SendState();

	game_process.player1_->Send("GO 0 0");

	game_process.player1_->Recv(msg);
	// split message as token
	tokens = digital_curling::SpritAsTokens(msg, " ");
	if (tokens.size() == 0) {
		cerr << "Error: empty message" << endl;
		return 0;
	}
	if (tokens[0] != "BESTSHOT") {
		cerr << "Error: invalid command '" << tokens[0] << "'" << endl;
		return 0;
	}
	// print tokens
	for (int unsigned i = 1; i < tokens.size(); i++) {
		cerr << "tokens[" << i << "] = " << tokens[i] << endl;
	}

	return 1;
}