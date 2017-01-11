#pragma once

#include <GLFW/glfw3.h>

class Core_API GUI
{
	DECLARE_SINGLETON(GUI);
private:
	//std::shared_ptr<spdlog::logger> Console;
	std::thread GUIThread;
	GLFWwindow* Window;
public:
	int Init();
	void Main();
	void Halt();
};