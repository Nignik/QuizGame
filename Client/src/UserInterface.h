#pragma once

#include <string>
#include <functional>
#include <string>

#include <imgui.h>
#include <imgui_stdlib.h>

template <typename T>
struct InputField
{
	std::string label;
	std::function<void(T&)> OnInput;

	T val;
};

struct UIButton
{
	std::string label;
	std::function<void()> onClick;
};

struct ListSelection
{
	std::vector<std::string> selection{};
	std::function<void(std::string&)> OnSelected;

	int selected = -1;
};

struct QuizOptions
{
	UIButton createQuiz;
	InputField<int> repeats;
	ListSelection quizPathSelection;
};

class UI
{
public:
	UI(InputField<std::string>&& inputField, QuizOptions&& quizOptions, int* score);

	void Render();

	void RenderInputField();
	void RenderQuizOptions();
	void RenderScore() const;

private:
	InputField<std::string> m_inputField;
	QuizOptions m_quizOptions;
	int* m_score = nullptr;
};