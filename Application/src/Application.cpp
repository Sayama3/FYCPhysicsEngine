//
// Created by Sayama on 03/12/2024.
//

#include "Application.hpp"

#include <raylib.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

Application::Application(int width, int height, const std::string& name)
	: m_Width(width), m_Height(height), m_Camera2D({{static_cast<float>(m_Width)*0.5f,static_cast<float>(m_Height)*0.5f},{0,0}, 0, static_cast<float>(m_Height) * 0.2f})
{
	// Initialization
	//--------------------------------------------------------------------------------------
	InitWindow(width, height, name.c_str());
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
	// Draw
	//----------------------------------------------------------------------------------
	BeginDrawing();

	ClearBackground(RAYWHITE);

	BeginMode2D(m_Camera2D);

	DrawCircleV({0.0f,-1.5f},0.5f, Color{20, 30, 200, 255});
	DrawRectangleV({-0.5,-0.5},{1,1}, Color{200, 30, 20, 255});

	EndMode2D();

	DrawText("Congrats! You created your first window!", 190, 350, 20, LIGHTGRAY);

	EndDrawing();
	//----------------------------------------------------------------------------------
}

Application::~Application() {
	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------
}
