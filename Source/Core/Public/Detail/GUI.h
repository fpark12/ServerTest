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
	virtual void Frame() {}
	void Show();
	void Hide();
	void Main();
	void Halt();
};