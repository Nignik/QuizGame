#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "Server.h"

int main()
{
	Server::Init();
	Server* server = Server::s_instance;

	server->Run();

	server->Shutdown();
}