#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "Server.h"

int main(int argc, const char* argv[])
{
	Server::Init();
	Server* server = Server::s_instance;

	server->LoadQuiz(argv[1]);
	server->Run();

	server->Shutdown();
}