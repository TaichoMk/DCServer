#pragma once

#include <iostream>
#include <string>

namespace digital_curling
{
	class Player {
	public:
		static const size_t kBufferSize = 1024;

		// send message from player
		virtual int Send( const char *message) = 0;
		// recieve message from player
		virtual int Recv( char *message) = 0;
	};

	class LocalPlayer : public Player {
	public:
		// recieve message from player
		int Send(const char *message);
		// recieve message from player
		int Recv(char *message);
	};
}