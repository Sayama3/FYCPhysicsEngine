//
// Created by Sayama on 03/12/2024.
//

#pragma once

#include <raylib.h>

#include "Camera.hpp"

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

	void UpdateLogic();
	void UpdateRendering();
	void UpdateUI();
private:
	uint32_t m_Width, m_Height;
	FYC::Real m_CameraMoveSpeed = 4;
	FYC::Real m_CameraZoomSpeed = 0.05;
	FYC::Application::Camera m_Camera;
};
