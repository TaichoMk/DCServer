#include "main.h"

#include <windows.h>

#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;

void test_command(char *msg)
{
	// split message as token
	vector<string> tokens = digital_curling::SpritAsTokens(msg);
	if (tokens.size() == 0) {
		cerr << "Error: empty message" << endl;
		return;
	}
	if (tokens[0] != "COMMAND") {
		cerr << "Error: invalid command '" << tokens[0] << "'" << endl;
		return;
	}
	// print tokens
	for (int i = 1; i < tokens.size(); i++) {
		cerr << "tokens[" << i << "] = " << tokens[i] << endl;
	}
	cerr << "msg = '" << msg << "'" << endl;
}

int main(void)
{
	digital_curling::Player *p1 = new digital_curling::LocalPlayer("hoge", 0);
	digital_curling::Player *p2 = new digital_curling::LocalPlayer("hoge", 0);
	digital_curling::GameProcess game_process(p1, p2, 8, 0.145, 0);

	digital_curling::PrintState(game_process.gs_);

	string str = "Hoge Message";
	char msg[1024] = "";

	// send message to player1
	game_process.player1_->Send(str.c_str());

	// recieve message from player1
	game_process.player1_->Recv(msg);
	cout << "Recieved message = '" << msg << "'" << endl;

	//test_command(msg);
	game_process.SendState();

	return 0;
}