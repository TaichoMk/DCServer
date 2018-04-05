#pragma warning(disable:4996)  // disable error (string::copy)
#include "game_player.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

namespace digital_curling
{
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