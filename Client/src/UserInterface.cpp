#include "UserInterface.h"

UI::UI()
{
	ImGui::StyleColorsDark();
}

void UI::Render()
{
	ImGui::Begin("Input");

	for (auto& inputField : m_inputFields)
	{
		if (ImGui::InputText(inputField.name.c_str(), &inputField.val, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			inputField.func(inputField.val);
			inputField.val.clear();
			ImGui::SetKeyboardFocusHere(-1);
		}
	}

	ImGui::End();
}

void UI::AddInputField(InputField&& inputField)
{
	m_inputFields.push_back(inputField);
}

