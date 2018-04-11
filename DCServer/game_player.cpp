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
			time_limit_ = time_remain_ = 0;
		}
	}

	LocalPlayer::~LocalPlayer() {}

	// recieve message from player
	int LocalPlayer::Send(const char *message)
	{
		//cout << "Server -> LocalPlayer: '" << message << "'" << endl;
		DWORD NumberOfBytesWritten = 0;
		if (this->write_pipe_ != NULL) {
			WriteFile(this->write_pipe_, message, (DWORD)strlen(message) + 1, &NumberOfBytesWritten, NULL);
		}

		return NumberOfBytesWritten;
	}

	// recieve message from player
	int LocalPlayer::Recv(char *message)
	{
		/*
		// Recieve from console
		std::string str;
		std::getline(std::cin, str);
		if (str.size() > kBufferSize) {
			return 0;
		}
		str.copy(message, str.size(), 0);
		*/
		DWORD NumberOfBytesRead = 0;
		memset(message, 0, kBufferSize);
		if (this->read_pipe_ != NULL) {
			ReadFile(this->read_pipe_, message, kBufferSize, &NumberOfBytesRead, NULL);
		}

		//cout << "LocalPlayer -> Server: '" << message << "'" << endl;
		
		return NumberOfBytesRead;
	}

	// Create Proccess
	// This function returns 0 when CreateProcess was failed
	int LocalPlayer::InitProcess()
	{
		/*** create pipe ***/
		HANDLE read_tmp, write_tmp;
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;

		// server -> COM
		if (!CreatePipe(&read_tmp, &write_pipe_, &sa, 0)) {
			return 0;
		}
		if (!DuplicateHandle(GetCurrentProcess(), read_tmp, GetCurrentProcess(), &read_pipe_, 0, TRUE, DUPLICATE_SAME_ACCESS)) {
			return 0;
		}
		CloseHandle(read_tmp);

		// COM -> server
		if (!CreatePipe(&read_pipe_, &write_tmp, &sa, 0)) {
			return 0;
		}
		if (!DuplicateHandle(GetCurrentProcess(), write_tmp, GetCurrentProcess(), &write_pipe_, 0, TRUE, DUPLICATE_SAME_ACCESS)) {
			return 0;
		}
		CloseHandle(write_tmp);

		// redirection
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(STARTUPINFO);
		si.dwFlags = STARTF_USESTDHANDLES;
		si.hStdInput = this->read_pipe_;
		si.hStdOutput = this->write_pipe_;
		si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

		if (si.hStdOutput == INVALID_HANDLE_VALUE || si.hStdError == INVALID_HANDLE_VALUE) {
			return 0;
		}

		/*** create process ***/
		return CreateProcess(NULL, (LPSTR)path_.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi_);
	}

	// Exit process
	int LocalPlayer::ExitProcess() {
		if (!TerminateProcess(pi_.hProcess, 0)) {
			return 0;
		}
		return 1;
	}
}