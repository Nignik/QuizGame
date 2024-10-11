#pragma once

#include <string>
#include <functional>

#include <imgui.h>
#include <imgui_stdlib.h>

struct InputField
{
	std::string name;
	std::function<void(std::string&)> func;

	std::string val;
};

class UI
{
public:
	UI();

	void Render();

	void AddInputField(InputField&& inputField);

private:
	std::vector<InputField> m_inputFields;
};