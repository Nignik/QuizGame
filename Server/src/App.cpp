#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "Server.h"

int main()
{
	Server server;

	server.Run();
}