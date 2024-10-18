#include "UserInterface.h"

UI::UI(InputField<std::string>&& inputField, QuizOptions&& quizOptions, int* score)
	: m_inputField(std::move(inputField)),
	m_quizOptions(std::move(quizOptions)),
	m_score(score)
{
	ImGui::StyleColorsDark();
}

void UI::Render()
{
	RenderInputField();
	RenderQuizOptions();
	RenderScore();
}

void UI::RenderInputField()
{
	ImGui::Begin("Input");

	if (ImGui::InputText(m_inputField.label.c_str(), &m_inputField.val, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		m_inputField.OnInput(m_inputField.val);
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

	ImGui::NewLine();
	ImGui::Text("Quiz files: ");
	auto& paths = m_quizOptions.quizPathSelection;
	for (size_t i = 0; i < paths.selection.size(); i++)
	{
		std::string path = paths.selection[i];
		if (ImGui::Selectable(path.c_str(), paths.selected == i))
		{
			if (paths.selected != i)
				paths.OnSelected(path);

			paths.selected = i;
		}
	}
	
	ImGui::NewLine();
	int* reps = &m_quizOptions.repeats.val;
	if (ImGui::InputInt("Repeats", reps))
	{
		if (*reps < 1) *reps = 1;
		m_quizOptions.repeats.OnInput(m_quizOptions.repeats.val);
	}

	ImGui::End();
}

void UI::RenderScore() const
{
	ImGui::Begin("Score: ");

	ImGui::Text(std::to_string(*m_score).c_str());

	ImGui::End();
}

