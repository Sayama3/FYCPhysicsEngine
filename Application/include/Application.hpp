//
// Created by Sayama on 03/12/2024.
//

#pragma once

#include <raylib.h>

#if defined(PLATFORM_WEB)
void UpdateLoop(void* arg);
#endif

class Application {
#if defined(PLATFORM_WEB)
	friend void UpdateLoop(void* arg);
#endif
public:
	Application(int width = 800, int height = 450, const std::string& name = "Application");
	~Application();
	Application(const Application &) = delete;
	Application &operator=(const Application &) = delete;
public:
	void Run();
private:
	void Update();
private:
	uint32_t m_Width, m_Height;
	Camera2D m_Camera2D;
};
