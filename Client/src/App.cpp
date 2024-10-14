#include "Client.h"
#include "UserInterface.h"
#include "Renderer.h"

int main(int argc, const char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <server_address>\n";
		return 1;
	}

	Client::Init(argv[1]);
	Client* client = Client::s_instance;

	client->SendQuizFilePathsRequest();
	while (!client->m_quizFilePathsAvailable)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	Renderer renderer;

	InputField inputField = { "Text input", std::bind(&Client::SendAnswer, client, std::placeholders::_1), "" };
	UIButton createQuiz = { "Create quiz", std::bind(&Client::SendQuizChangeRequest, client)};
	QuizOptions quizOptions = { createQuiz, client->GetQuizFilePaths(), std::bind(&QuizBlueprint::SetPath, &client->m_quizBlueprint, std::placeholders::_1)};
	UI ui(std::move(inputField), std::move(quizOptions));

	while (client->IsRunning())
	{
		client->OnTick();

		renderer.StartFrame();

		ui.Render();
		renderer.RenderFrame();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	client->Shutdown();
}