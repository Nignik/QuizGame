#include "Client.h"
#include "UserInterface.h"
#include "Renderer.h"

int main(int argc, const char* argv[])
{
	int score = 0;

	if (argc < 2)
	{
		Client::Init("192.168.56.1", &score);
	}
	else
	{
		Client::Init(argv[1], &score);
	}
	
	Client* client = Client::s_instance;

	client->SendQuizFilePathsRequest();
	while (!client->m_quizFilePathsAvailable)
	{
		client->OnTick();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	Renderer renderer;

	InputField<std::string> inputField = { "Text input", std::bind(&Client::SendAnswer, client, std::placeholders::_1), "" };
	UIButton createQuiz = { "Create quiz", std::bind(&Client::SendQuizChangeRequest, client)};
	InputField<int> repeats = { "Repeats", std::bind(&QuizBlueprint::SetRepeatTimes, &client->m_quizBlueprint, std::placeholders::_1), 1};
	QuizOptions quizOptions = { createQuiz, repeats, client->GetQuizFilePaths(), std::bind(&QuizBlueprint::SetPath, &client->m_quizBlueprint, std::placeholders::_1)};
	UI ui(std::move(inputField), std::move(quizOptions), &score);

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