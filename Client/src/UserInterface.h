#pragma once

#include <string>
#include <functional>
#include <string>

#include <imgui.h>
#include <imgui_stdlib.h>

struct InputField
{
	std::string label;
	std::function<void(std::string&)> func;

	std::string val;
};

struct UIButton
{
	std::string label;
	std::function<void()> onClick;
};

struct QuizOptions
{
	UIButton createQuiz;
	std::vector<std::string> quizPaths{};
	std::function<void(std::string&)> setPath;

	int selectedQuizPath = -1;
};

class UI
{
public:
	UI(InputField&& inputField, QuizOptions&& quizOptions);

	void Render();

	void RenderInputField();
	void RenderQuizOptions();

private:
	InputField m_inputField;
	QuizOptions m_quizOptions;
};