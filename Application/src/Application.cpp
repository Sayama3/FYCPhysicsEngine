//
// Created by Sayama on 03/12/2024.
//

#include "Application.hpp"

#include <raylib.h>

#include <imgui.h>
#include <rlImGui.h>
#include "ImGuiLib.hpp"

using namespace FYC::Literal;

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

Application::Application(int width, int height, const std::string &name)
	: m_Width(width), m_Height(height), m_Camera(m_Width, m_Height) {
	// Initialization
	//--------------------------------------------------------------------------------------
	SetConfigFlags(FLAG_WINDOW_RESIZABLE); // Window configuration flags
	InitWindow(width, height, name.c_str());

	rlImGuiSetup(true); // sets up ImGui with either a dark or light default theme
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
	SetTargetFPS(60); // Set our game to run at 60 frames-per-secon
	//--------------------------------------------------------------------------------------
	// Main game loop
	while (!WindowShouldClose()) // Detect window close button or ESC key
	{
		Update();
	}
#endif
}

void Application::Update() {
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

	rlImGuiBegin(); // starts the ImGui content mode. Make all ImGui calls after this
	UpdateUI();
	m_ImGuiIsActive = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemActive();
	rlImGuiEnd(); // ends the ImGui content mode. Make all ImGui calls before this

	EndDrawing();
	//----------------------------------------------------------------------------------
}

void Application::UpdateLogic() {
	m_Camera.SetViewport(m_Width, m_Height);

	if (!m_ImGuiIsActive) {
		if (IsKeyDown(KEY_R)) {
			m_Camera.SetPosition({0, 0});
			m_Camera.SetZoom(static_cast<FYC::Real>(m_Height) * 0.2f);
		} else {
			FYC::Vec2 movement{};

			if (IsKeyDown(KeyboardKey::KEY_W) || IsKeyDown(KeyboardKey::KEY_UP)) movement += {+0, -1};
			if (IsKeyDown(KeyboardKey::KEY_S) || IsKeyDown(KeyboardKey::KEY_DOWN)) movement += {+0, +1};
			if (IsKeyDown(KeyboardKey::KEY_D) || IsKeyDown(KeyboardKey::KEY_RIGHT)) movement += {+1, +0};
			if (IsKeyDown(KeyboardKey::KEY_A) || IsKeyDown(KeyboardKey::KEY_LEFT)) movement += {-1, +0};

			Vector2 mouseWheel = GetMouseWheelMoveV();
			FYC::Real zoom = 1 + mouseWheel.y * m_CameraZoomSpeed;

			bool isMovingFast = IsKeyDown(KeyboardKey::KEY_LEFT_SHIFT) || IsKeyDown(KeyboardKey::KEY_RIGHT_SHIFT) ||
			                    IsKeyDown(KeyboardKey::KEY_KP_0);
			FYC::Real cameraSpeed = isMovingFast ? m_CameraMoveFastSpeed : m_CameraMoveSpeed;

			m_Camera.Move(movement * (GetFrameTime() * cameraSpeed));
			m_Camera.MultiplyZoom(zoom);
		}
	}

	if (m_PhysicsMode == PhysicsMode::Play) {
		float stepTime = std::min(GetFrameTime(), 1.0f);
		m_WorldPlay.Step(stepTime);
	}
}

void Application::UpdateRendering() {
	for (const auto& particle : GetWorld()) {
		if (particle.Data.type() != typeid(Color)) continue;
		auto pos = particle.GetPosition();

		static_assert(std::is_same<FYC::Particle::Shape, std::variant<FYC::Circle, FYC::AABB>>());

		if (FYC::Circle circle; particle.HasShape<FYC::Circle>(circle)) {
			DrawCircleV({pos.x, pos.y}, circle.Radius, std::any_cast<Color>(particle.Data));
		} else if (FYC::AABB rectangle; particle.HasShape<FYC::AABB>(rectangle)) {
			const auto size = rectangle.GetSize();
			DrawRectangleV({rectangle.Min.x, rectangle.Min.y}, {size.x, size.y}, std::any_cast<Color>(particle.Data));
		}
	}

	if (const FYC::AABB* aabb = std::get_if<FYC::AABB>(&GetWorld().Bounds)) {
		const auto size = aabb->GetSize();
		constexpr float linethick = 0.05;
		DrawRectangleLinesEx(Rectangle{aabb->Min.x - linethick, aabb->Min.y - linethick, size.x + (linethick * 2), size.y + (linethick * 2)}, linethick, {0,180, 0, 160});
	}
}

void Application::UpdateUI() {
	static bool showDemo = true;
	if (showDemo) ImGui::ShowDemoWindow(&showDemo);

	ImGui::SetNextWindowSize({300, 200}, ImGuiCond_Once);
	ImGui::Begin("Camera"); {
		FYC::Vec2 pos = m_Camera.GetPosition();
		if (ImGuiLib::DragReal2("Position", pos.data, 0.1)) {
			m_Camera.SetPosition(pos);
		}

		FYC::Real zoom = m_Camera.GetZoom();
		if (ImGuiLib::DragReal("Zoom", &zoom)) {
			m_Camera.SetZoom(zoom);
		}
	}
	ImGui::End();

	ImGui::SetNextWindowSize({400, 400}, ImGuiCond_Once);
	ImGui::Begin("Physics"); {
		if (m_PhysicsMode != PhysicsMode::Edit) {
			if (ImGui::Button("Stop")) {
				m_PhysicsMode = PhysicsMode::Edit;
			}
		} else {
			if (ImGui::Button("Play")) {
				m_PhysicsMode = PhysicsMode::Play;
				m_WorldPlay = m_WorldEdit;
			}
		}

		if (m_PhysicsMode != PhysicsMode::Edit) {
			bool isPause = m_PhysicsMode == PhysicsMode::Pause;
			if (ImGui::Checkbox("Pause", &isPause)) {
				m_PhysicsMode = isPause ? PhysicsMode::Pause : PhysicsMode::Play;
			}
		}

		ImGui::Spacing();

		if (FYC::AABB* aabb = std::get_if<FYC::AABB>(&GetWorld().Bounds)) {
			if (ImGui::Button("Remove Bounds")) {
				GetWorld().Bounds = std::monostate{};
			} else {
				bool changed = false;
				bool posSizeChanged = false;
				changed |= ImGuiLib::DragReal2("Min AABB", aabb->Min.data, 0.1);
				changed |= ImGuiLib::DragReal2("Max AABB", aabb->Max.data, 0.1);
				ImGui::Separator();
				auto pos = aabb->GetCenter();
				auto size = aabb->GetSize();
				changed |= posSizeChanged |= ImGuiLib::DragReal2("Position AABB", pos.data, 0.1);
				changed |= posSizeChanged |= ImGuiLib::DragReal2("Size AABB", size.data, 0.1);
				if (posSizeChanged) *aabb = FYC::AABB::FromCenterSize(pos, size);
				if (changed) aabb->Validate();
			}
		} else {
			if (ImGui::Button("Add Bounds")) {
				GetWorld().Bounds = FYC::AABB::FromCenterSize({0,0}, {5, 5});
			}
		}

		ImGui::Spacing();

		{
			FYC::World::WorldIterator p;
			if (ImGui::Button("Add Circle")) {
				p = GetWorld().AddParticle(FYC::Particle::CreateCircle({}, 0.5));
			}

			if (ImGui::Button("Add Rectangle")) {
				p = GetWorld().AddParticle(FYC::Particle::CreateRectangle({}, {1,1}));
			}

			if (p) {
				p->AddConstantAcceleration({0, 10});
				p->Data = Color{static_cast<uint8_t>(rand() % 256), static_cast<uint8_t>(rand() % 256), static_cast<uint8_t>(rand() % 256), 255};
			}
		}

		ImGui::Separator();

		std::vector<FYC::World::ID> toDelete;

		// Adding this variable to prevent removing object from the wrong world.
		auto& currentWorld = GetWorld();
		ImGui::PushID(&currentWorld);
		for (auto it = currentWorld.begin() ; it != currentWorld.end(); ++it) {
			FYC::Particle& particle = *it;
			ImGui::PushID(reinterpret_cast<void*>(it.GetID()));

			if (!particle.HasShape<FYC::AABB>()) {
				if (ImGui::Button("Change Shape to Rectangle")) particle.SetRectangleSize({1,1});
			}
			if (!particle.HasShape<FYC::Circle>()) {
				if (ImGui::Button("Change Shape to Circle")) particle.SetCircleRadius(0.5);
			}

			bool isKinematic = particle.IsKinematic();
			if (ImGui::Checkbox("Is Kinematic", &isKinematic)) particle.SetKinematic(isKinematic);
			auto position = particle.GetPosition();
			if (ImGuiLib::DragReal2("Position", position.data, 0.1)) particle.SetPosition(position);
			if (isKinematic) {
				auto velocity = particle.GetVelocity();
				if (ImGuiLib::DragReal2("Velocity", velocity.data, 0.1)) particle.SetVelocity(velocity);
				FYC::Vec2 acc = particle.GetConstantAccelerations();
				if (ImGuiLib::DragReal2("Acceleration", acc.data, 0.1)) particle.SetConstantAcceleration(acc);
				FYC::Real rebound = particle.GetRebound();
				if (ImGuiLib::SliderReal("Rebound", &rebound, 0, 1)) particle.SetRebound(rebound);
			}

			static_assert(std::is_same<FYC::Particle::Shape, std::variant<FYC::Circle, FYC::AABB> >());
			if (auto maybeRadius = particle.GetCircleRadius()) {
				FYC::Real radius = maybeRadius.value();
				if (ImGuiLib::DragReal("Radius", &radius, 0.01, 0.01, REAL_MAX, "%.2f")) {
					particle.TrySetCircleRadius(radius);
				}
			} else if (auto maybeSize = particle.GetRectangleSize()) {
				FYC::Vec2 size = maybeSize.value();
				if (ImGuiLib::DragReal2("Size", size.data, 0.01, 0.01, REAL_MAX, "%.2f")) {
					particle.TrySetRectangleSize(size);
				}
			}

			if (particle.Data.type() == typeid(Color)) {
				Color color = std::any_cast<Color>(particle.Data);
				float color3[3] = {color.r / 255.f, color.g / 255.f, color.b / 255.f};
				if (ImGui::ColorEdit3("Color", color3, ImGuiColorEditFlags_Uint8)) {
					particle.Data = Color{(unsigned char)(color3[0]*255), (unsigned char)(color3[1]*255), (unsigned char)(color3[2]*255), 255};
				}
			}
			if (ImGui::Button("Delete")) {
				toDelete.push_back(it.GetID());
			}
			ImGui::Separator();
			ImGui::PopID();
		}
		ImGui::PopID();

		for (const FYC::World::ID id : toDelete) {
			currentWorld.RemoveParticle(id);
		}
	}
	ImGui::End();
}

Application::~Application() {
	// De-Initialization
	//--------------------------------------------------------------------------------------
	rlImGuiShutdown(); // cleans up ImGui
	CloseWindow(); // Close window and OpenGL context
	//--------------------------------------------------------------------------------------
}
