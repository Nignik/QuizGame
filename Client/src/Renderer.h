#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <memory>

class Renderer
{
public:
	Renderer();
	~Renderer();

	void StartFrame();
	void RenderFrame();

	bool IsRunning();

private:
	GLFWwindow* m_window = nullptr;

	void InitWindow();
	void InitImGui();

	void Cleanup();

};