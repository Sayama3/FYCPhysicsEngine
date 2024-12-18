//
// Created by Sayama on 03/12/2024.
//

#include "Application.hpp"

#include <raylib.h>

#include <imgui.h>
#include <rlImGui.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

Application::Application(int width, int height, const std::string& name)
	: m_Width(width), m_Height(height), m_Camera(m_Width, m_Height)
{
	// Initialization
	//--------------------------------------------------------------------------------------
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);    // Window configuration flags
	InitWindow(width, height, name.c_str());

	rlImGuiSetup(true); 	// sets up ImGui with ether a dark or light default theme
}

#if defined(PLATFORM_WEB)
void UpdateLoop(void* arg)
{
	static_cast<Application*>(arg)->Update();
}
#endif

void Application::Run() {
#if defined(PLATFORM_WEB)
	emscripten_set_main_loop_arg(&UpdateLoop, this, 0, 1);
#else
	SetTargetFPS(60);   // Set our game to run at 60 frames-per-secon
	//--------------------------------------------------------------------------------------
	// Main game loop
	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		Update();
	}
#endif
}

void Application::Update()
{
	// Logic such as Physics or Camera Update here
	//----------------------------------------------------------------------------------

	m_Width = GetScreenWidth();
	m_Height = GetScreenHeight();

	UpdateLogic();
	//----------------------------------------------------------------------------------

	// Draw
	//----------------------------------------------------------------------------------
	BeginDrawing();

	ClearBackground(RAYWHITE);

	BeginMode2D(m_Camera.GetRaylibCamera());

	UpdateRendering();

	EndMode2D();

	rlImGuiBegin();			// starts the ImGui content mode. Make all ImGui calls after this
	UpdateUI();
	m_ImGuiIsActive = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemActive();
	rlImGuiEnd();			// ends the ImGui content mode. Make all ImGui calls before this

	EndDrawing();
	//----------------------------------------------------------------------------------
}

void Application::UpdateLogic()
{
	m_Camera.SetViewport(m_Width, m_Height);

	if (!m_ImGuiIsActive)
	{
		if (IsKeyDown(KEY_R)) {
			m_Camera.SetPosition({0,0});
			m_Camera.SetZoom(static_cast<FYC::Real>(m_Height) * 0.2f);
		} else {
			FYC::Vec2 movement{};

			if (IsKeyDown(KeyboardKey::KEY_W) || IsKeyDown(KeyboardKey::KEY_UP))	movement += {+0, -1};
			if (IsKeyDown(KeyboardKey::KEY_S) || IsKeyDown(KeyboardKey::KEY_DOWN))	movement += {+0, +1};
			if (IsKeyDown(KeyboardKey::KEY_D) || IsKeyDown(KeyboardKey::KEY_RIGHT))	movement += {+1, +0};
			if (IsKeyDown(KeyboardKey::KEY_A) || IsKeyDown(KeyboardKey::KEY_LEFT))	movement += {-1, +0};

			Vector2 mouseWheel = GetMouseWheelMoveV();
			FYC::Real zoom = 1 + mouseWheel.y * m_CameraZoomSpeed;

			bool isMovingFast = IsKeyDown(KeyboardKey::KEY_LEFT_SHIFT) || IsKeyDown(KeyboardKey::KEY_RIGHT_SHIFT) || IsKeyDown(KeyboardKey::KEY_KP_0);
			FYC::Real cameraSpeed = isMovingFast ? m_CameraMoveFastSpeed : m_CameraMoveSpeed;

			m_Camera.Move(movement * (GetFrameTime() * cameraSpeed));
			m_Camera.MultiplyZoom(zoom);
		}
	}
}

void Application::UpdateRendering() {
	DrawCircleV({0.0f,-1.5f},0.5f, Color{20, 30, 200, 255});
	DrawRectangleV({-0.5,-0.5},{1,1}, Color{200, 30, 20, 255});
}

void Application::UpdateUI() {
	static bool showDemo = true;
	if (showDemo) ImGui::ShowDemoWindow(&showDemo);
}

Application::~Application() {
	// De-Initialization
	//--------------------------------------------------------------------------------------
	rlImGuiShutdown();    // cleans up ImGui
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------
}
