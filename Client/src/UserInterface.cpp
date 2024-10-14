#include "UserInterface.h"

UI::UI(InputField&& inputField, QuizOptions&& quizOptions)
	: m_inputField(std::move(inputField)),
	m_quizOptions(std::move(quizOptions))
{
	ImGui::StyleColorsDark();
}

void UI::Render()
{
	RenderInputField();
	RenderQuizOptions();
}

void UI::RenderInputField()
{
	ImGui::Begin("Input");

	if (ImGui::InputText(m_inputField.label.c_str(), &m_inputField.val, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		m_inputField.func(m_inputField.val);
		m_inputField.val.clear();
		ImGui::SetKeyboardFocusHere(-1);
	}

	ImGui::End();
}

void UI::RenderQuizOptions()
{
	ImGui::Begin("Quiz creator");

	if (ImGui::Button(m_quizOptions.createQuiz.label.c_str()))
	{
		m_quizOptions.createQuiz.onClick();
	}

	ImGui::Text("Quiz files: ");
	for (auto& quiz : m_quizOptions.quizPaths)
	{
		if (ImGui::Selectable(quiz.c_str()))
		{
			m_quizOptions.setPath(quiz);
		}
	}

	ImGui::End();
}

