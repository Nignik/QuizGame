#include "Client.h"
#include "UserInterface.h"
#include "Renderer.h"

int main(int argc, const char* argv[])
{
	/*if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <server_address>\n";
		return 1;
	}*/

	Client::Init("127.0.0.1");
	Client* client = Client::s_instance;

	Renderer renderer;
	UI ui;
	ui.AddInputField({ "Text input", std::bind(&Client::SendAnswer, client, std::placeholders::_1), ""});

	while (client->IsRunning())
	{
		client->OnTick();

		renderer.StartFrame();

		ui.Render();
		renderer.RenderFrame();

		//std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	client->Shutdown();
}