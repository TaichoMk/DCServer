#pragma once

#include <iostream>
#include <string>

namespace digital_curling
{
	// Information of player (anstruct class)
	class Player {
	public:
		static const size_t kBufferSize = 1024;

		// Send message from player
		virtual int Send( const char *message) = 0;
		// Recieve message from player
		virtual int Recv( char *message) = 0;

		std::string name_;  // Player's name
		int time_limit_;    // Timelimit
		int time_remain_;   // Timelimit remaining
		
	};

	// Player running on local (using standard IO to communicate)
	class LocalPlayer : public Player {
	public:

		LocalPlayer( std::string path, int time_limit);
		~LocalPlayer();

		// Send message to player
		int Send(const char *message);
		// Recieve message from player
		int Recv(char *message);

		std::string path_;  // Full path of .exe file

	};

	// Player running on nerwork (using socket to communicate)
	/*
	// TODO: ŽÀ‘•‚·‚é
	class NetworkPlayer : public Player {
	public:

		NetworkPlayer();
		~NetworkPlayer();

		// Send message to player
		int Send(const char *message);
		// Recieve message from player
		int Recv(char *message);
	};
	*/
}