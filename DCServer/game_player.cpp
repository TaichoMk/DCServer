#pragma warning(disable:4996)  // disable error (string::copy)
#include "game_player.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

namespace digital_curling
{
	LocalPlayer::LocalPlayer(std::string path, int time_limit)
	{
		path_ = path;
		if (time_limit > 0) {
			time_limit_ = time_remain_ = time_limit;
		}
		else {
			time_limit_ = time_remain_ = -1;
		}
	}

	LocalPlayer::~LocalPlayer() {}

	// recieve message from player
	int LocalPlayer::Send(const char *message)
	{
		cout << "LocalPlayer <- Server '" << message << "'" << endl;

		return 1;
	}

	// recieve message from player
	int LocalPlayer::Recv(char *message)
	{
		std::string str;
		std::getline(std::cin, str);
		if (str.size() > kBufferSize) {
			return 0;
		}
		str.copy(message, str.size(), 0);

		cerr << "LocalPlayer -> Server '" << message << "'" << endl;
		
		return 1;
	}
}