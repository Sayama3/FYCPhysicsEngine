//
// Created by Sayama on 03/12/2024.
//

#include "Application.hpp"

#include <raylib.h>

#include <imgui.h>
#include <rlImGui.h>
#include "ImGuiLib.hpp"
#include "WorldSerializer.hpp"

using namespace FYC::Literal;

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

Application::Application(int width, int height, const std::string &name)
	: m_Width(width), m_Height(height), m_Camera(m_Width, m_Height, 30) {
	// Initialization
	//--------------------------------------------------------------------------------------
	SetConfigFlags(FLAG_WINDOW_RESIZABLE); // Window configuration flags
	InitWindow(width, height, name.c_str());
	rlImGuiSetup(true); // sets up ImGui with either a dark or light default theme

	LoadCharacter(c_CharacterSaveFile);
	LoadWorld(c_WorldSaveFile);
	LoadEnnemis(c_EnnemiSaveFile);
	LoadDeathPlatforms(c_DeathPlatformsSaveFile);
	LoadWinPlatforms(c_WinPlatformsSaveFile);

	Play();
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

#if false
	if (!m_ImGuiIsActive) {
		if (IsKeyDown(KEY_R)) {
			m_Camera.SetPosition({0, 0});
			m_Camera.SetZoom(static_cast<FYC::Real>(m_Height) * 0.2f);
		} else {
			FYC::Vec2 movement{};

			if (/*IsKeyDown(KeyboardKey::KEY_W) || */IsKeyDown(KeyboardKey::KEY_UP)) movement += {+0, -1};
			if (/*IsKeyDown(KeyboardKey::KEY_S) || */IsKeyDown(KeyboardKey::KEY_DOWN)) movement += {+0, +1};
			if (/*IsKeyDown(KeyboardKey::KEY_D) || */IsKeyDown(KeyboardKey::KEY_RIGHT)) movement += {+1, +0};
			if (/*IsKeyDown(KeyboardKey::KEY_A) || */IsKeyDown(KeyboardKey::KEY_LEFT)) movement += {-1, +0};

			Vector2 mouseWheel = GetMouseWheelMoveV();
			FYC::Real zoom = 1 + mouseWheel.y * m_CameraZoomSpeed;

			bool isMovingFast = IsKeyDown(KeyboardKey::KEY_LEFT_SHIFT) || IsKeyDown(KeyboardKey::KEY_RIGHT_SHIFT) ||
			                    IsKeyDown(KeyboardKey::KEY_KP_0);
			FYC::Real cameraSpeed = isMovingFast ? m_CameraMoveFastSpeed : m_CameraMoveSpeed;

			m_Camera.Move(movement * (GetFrameTime() * cameraSpeed));
			m_Camera.MultiplyZoom(zoom);
		}
	}
#endif

	if (m_PhysicsMode == PhysicsMode::Play) {
		if (IsKeyPressed(KeyboardKey::KEY_E)) TryRestart();
		float stepTime = std::min(GetFrameTime(), 1.0f);
		UpdateCharacter(stepTime);
		m_WorldPlay.Step(stepTime);
		UpdateEnnemies(stepTime);
		if (m_ShouldStop) {
			Stop();
		}
	}

	if (m_ShouldStart) {
		Play();
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

void Application::LoadWorld(const std::filesystem::path& filepath) {
	std::ifstream file(filepath, std::ios::in  | std::ios::binary);
	if (file) {
		std::vector<char> binary((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		m_WorldPlay = m_WorldEdit = FYC::Application::WorldSerializer::FromBinary(binary);
	}
}

void Application::SaveWorld(const std::filesystem::path& filename) const {
	std::ofstream file(filename, std::ios::out  | std::ios::binary | std::ios::trunc);
	if (file) {
		const std::vector<char> binary = FYC::Application::WorldSerializer::ToBinary(m_WorldEdit);
		file.write(binary.data(), binary.size());
	}
}

void Application::LoadCharacter(const std::filesystem::path &filepath)
{
	std::ifstream file(filepath, std::ios::in  | std::ios::binary);
	if (!file) return;
	std::vector<char> binary((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	if (binary.size() != sizeof(FYC::Application::CharacterController)) return;
	m_CharacterController = *reinterpret_cast<FYC::Application::CharacterController*>(binary.data());
}

void Application::SaveCharacter(const std::filesystem::path &filename) const
{
	std::ofstream file(filename, std::ios::out  | std::ios::binary | std::ios::trunc);
	if (file) {
		file.write(reinterpret_cast<const char*>(&m_CharacterController), sizeof(m_CharacterController));
	}
}


void Application::LoadEnnemis(const std::filesystem::path &filepath)
{
	std::ifstream file(filepath, std::ios::in  | std::ios::binary);
	if (file) {
		std::vector<char> binary((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		auto&&[params, ids] =  FYC::Application::EnnemiSerializer::FromBinary(binary);
		m_EnnemiParameters = params;
		m_EnnemisIds = std::move(ids);
	}
}

void Application::SaveEnnemis(const std::filesystem::path &filename) const
{
	std::ofstream file(filename, std::ios::out  | std::ios::binary | std::ios::trunc);
	if (file) {
		const std::vector<char> binary = FYC::Application::EnnemiSerializer::ToBinary(m_EnnemiParameters, m_EnnemisIds);
		file.write(binary.data(), binary.size());
	}
}

void Application::LoadDeathPlatforms(const std::filesystem::path &filepath) {
	std::ifstream file(filepath, std::ios::in  | std::ios::binary);
	if (!file) return;
	std::vector<char> binary((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	uint64_t count = binary.size() / sizeof(FYC::World::ID);
	m_DeathPlatforms.clear();
	m_DeathPlatforms.assign(reinterpret_cast<FYC::World::ID*>(binary.data()), reinterpret_cast<FYC::World::ID*>(binary.data()) + count);
}

void Application::SaveDeathPlatforms(const std::filesystem::path &filename) const {
	std::ofstream file(filename, std::ios::out  | std::ios::binary | std::ios::trunc);
	if (file) {
		file.write(reinterpret_cast<const char*>(m_DeathPlatforms.data()), sizeof(FYC::World::ID) * m_DeathPlatforms.size());
	}
}

void Application::LoadWinPlatforms(const std::filesystem::path &filepath) {
	std::ifstream file(filepath, std::ios::in  | std::ios::binary);
	if (!file) return;
	std::vector<char> binary((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	if (binary.size() != sizeof(FYC::World::ID)) return;
	m_WinPlatform = *reinterpret_cast<FYC::World::ID*>(binary.data());
}

void Application::SaveWinPlatforms(const std::filesystem::path &filename) const {
	std::ofstream file(filename, std::ios::out  | std::ios::binary | std::ios::trunc);
	if (file) {
		file.write(reinterpret_cast<const char*>(&m_WinPlatform), sizeof(m_WinPlatform));
	}
}

void Application::TryStop() {
	m_ShouldStop = true;
}

void Application::TryRestart() {
	m_ShouldStop = true;
	m_ShouldStart = true;
}

void Application::Stop() {
	m_PhysicsMode = PhysicsMode::Edit;
	m_ShouldStop = false;
	OnStop();
}

void Application::Play() {
	m_PhysicsMode = PhysicsMode::Play;
	m_WorldPlay = m_WorldEdit;
	m_ShouldStart = false;
	OnPlay();
}

void Application::SetPause(bool isPause) {
	m_PhysicsMode = isPause ? PhysicsMode::Pause : PhysicsMode::Play;
	if (m_PhysicsMode == PhysicsMode::Pause) OnPause();
	else if (m_PhysicsMode == PhysicsMode::Play) OnResume();
}

void Application::UpdateUI() {
	// ========== RAYLIB ==========

	static const char* const MoveText = "Move with 'Q'/'D'";
	static const char* const JumpText = "Jump with the space bar";
	static const char* const RestartText = "Restart with 'E'";

	if (m_PhysicsMode != PhysicsMode::Edit) {
		DrawText(MoveText, 10, 10, 26, Color{30, 40, 50, 255});
		DrawText(JumpText, 10, 40, 26, Color{30, 40, 50, 255});
		DrawText(RestartText, 10, 70, 26, Color{30, 40, 50, 255});
	}

	static const char* const YouWonText = "You Won !!!";
	static constexpr int fontSize = 42;

	if (m_HasWon) {
		const int textSize = MeasureText(YouWonText, fontSize);
		DrawText(YouWonText, m_Width / 2 - textSize/2, m_Height / 4, fontSize, Color{180, 50, 50, 180});
	}
#if false
	// ========== IMGUI ==========
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
	ImGui::Begin("Game");
	{
		if (auto ptr = GetWorld().GetParticle(m_CharacterController.MainCharacterParticle)) {
			ImGui::Text("MainCharaceter is '%ull'", m_CharacterController.MainCharacterParticle);
		} else {
			ImGui::Text("No Main Character selected.");
		}
		ImGui::Spacing();
		static const std::filesystem::path characterFilename = c_CharacterSaveFile;
		static const std::filesystem::path ennemiFilename = c_EnnemiSaveFile;
		static const std::filesystem::path deathPlatformFilename = c_DeathPlatformsSaveFile;
		static const std::filesystem::path winPlatformFilename = c_WinPlatformsSaveFile;
		if (ImGui::Button("Save")) {
			SaveCharacter(characterFilename);
			SaveEnnemis(ennemiFilename);
			SaveDeathPlatforms(deathPlatformFilename);
			SaveWinPlatforms(winPlatformFilename);
		}

		if (ImGui::Button("Load")) {
			LoadCharacter(characterFilename);
			LoadEnnemis(ennemiFilename);
			LoadDeathPlatforms(deathPlatformFilename);
			LoadWinPlatforms(winPlatformFilename);
		}

		if (ImGui::Button("Clear")) {
			m_CharacterController = {};
			m_EnnemiParameters = {};
			m_EnnemisIds = {};
			m_DeathPlatforms = {};
			m_WinPlatform = FYC::World::NULL_ID;
		}

		ImGui::Separator();
		ImGuiLib::DragReal("Movement Acceleration", &m_CharacterController.MovementAcceleration);
		ImGuiLib::DragReal("Jump Impulse", &m_CharacterController.JumpImpulse);
		ImGui::Checkbox("CanJump", &m_CharacterController.CanJump);
		ImGui::Separator();
		ImGuiLib::DragReal("Ennemi Speed", &m_EnnemiParameters.Speed);
		ImGuiLib::DragReal("Threshold to Kill an ennemi", &m_EnnemiParameters.ThresholdKillEnnemi);
	}
	ImGui::End();

	ImGui::SetNextWindowSize({400, 400}, ImGuiCond_Once);
	ImGui::Begin("Physics"); {

		static const std::filesystem::path filename = c_WorldSaveFile;
		ImGui::BeginDisabled(m_PhysicsMode != PhysicsMode::Edit);
		if (ImGui::Button("Save")) {
			SaveWorld(filename);
		}
		if (ImGui::Button("Load")) {
			LoadWorld(filename);
		}
		if (ImGui::Button("Clear")) {
			m_WorldPlay = m_WorldEdit = FYC::World{};
		}
		ImGui::EndDisabled();

		if (m_PhysicsMode != PhysicsMode::Edit) {
			if (ImGui::Button("Stop")) {
				Stop();
			}
		} else {
			if (ImGui::Button("Play")) {
				Play();
			}
		}

		if (m_PhysicsMode != PhysicsMode::Edit) {
			bool isPause = m_PhysicsMode == PhysicsMode::Pause;
			if (ImGui::Checkbox("Pause", &isPause)) {
				SetPause(isPause);
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

			bool isMainCharacter = m_CharacterController.MainCharacterParticle == it.GetID();
			std::vector<FYC::World::ID>::iterator objectIt;
			bool isEnnemi = !isMainCharacter && (objectIt = std::find(m_EnnemisIds.begin(), m_EnnemisIds.end(), it.GetID())) != m_EnnemisIds.end();
			bool isDeathPlatform = !isMainCharacter && !isEnnemi && (objectIt = std::find(m_DeathPlatforms.begin(), m_DeathPlatforms.end(), it.GetID())) != m_DeathPlatforms.end();

			if (!isMainCharacter && !isEnnemi && particle.IsKinematic()) {
				if (ImGui::Button("Set As MainCharacter")) {
					m_CharacterController.MainCharacterParticle = it.GetID();
				}
			}

			if (particle.IsKinematic() && !isMainCharacter) {
				if (isEnnemi) {
					if (ImGui::Button("Remove From Ennemi")) {
						m_EnnemisIds.erase(objectIt);
					}
				} else {
					if (ImGui::Button("Add To Ennemi")) {
						m_EnnemisIds.push_back(it.GetID());
					}
				}
			}

			if (!isMainCharacter && !isEnnemi) {
				if (isDeathPlatform) {
					if (ImGui::Button("Remove from Deadly")) {
						m_DeathPlatforms.erase(objectIt);
					}
				} else {
					if (ImGui::Button("Set as Deadly")) {
						m_DeathPlatforms.push_back(it.GetID());
					}
				}
			}

			if (!isMainCharacter && !isEnnemi && !isDeathPlatform) {
				if (it.GetID() == m_WinPlatform) {
					if (ImGui::Button("Remove from WinPlatform")) {
						m_WinPlatform = FYC::World::NULL_ID;
					}
				} else {
					if (ImGui::Button("Set as WinPlatform")) {
						m_WinPlatform = it.GetID();
					}
				}
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
				FYC::Real inv_drag = 1-particle.GetDrag();
				if (ImGuiLib::SliderReal("Inverse Drag", &inv_drag, 1, 0, "%.5f", ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_Logarithmic)) particle.SetDrag(1-inv_drag);
				bool awake = particle.IsAwake();
				if (ImGui::Checkbox("Awake", &awake)) particle.SetIsAwake(awake);
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
	#endif
}

void Application::UpdateCharacter(FYC::Real stepTime) {
	auto it = GetWorld().find(m_CharacterController.MainCharacterParticle);
	if (it) {
		FYC::Vec2 movement{};
		if (IsKeyDown(m_CharacterController.RightKey)) movement.x += 1;
		if (IsKeyDown(m_CharacterController.LeftKey)) movement.x -= 1;

		if (movement.x != 0)
		{
			FYC::Math::NormalizeInPlace(movement);
			it->AddAcceleration(movement * m_CharacterController.MovementAcceleration);
		}

		if (IsKeyPressed(m_CharacterController.JumpKey) && m_CharacterController.CanJump) {
			m_CharacterController.CanJump = false;
			it->SetPosition(it->GetPosition() - FYC::Vec2{0,0.005});
			it->SubAcceleration({0,m_CharacterController.JumpImpulse / stepTime});
		}
	}
}


void Application::UpdateEnnemies(FYC::Real stepTime) {
	for (auto id : m_EnnemisIds) {
		auto ptr = m_WorldPlay.GetParticle(id);
		if (!ptr) continue;
		ptr->SetVelocity({FYC::Math::Sign(ptr->GetVelocity().x) * m_EnnemiParameters.Speed, ptr->GetVelocity().y});
	}
}

void Application::OnCollision(FYC::World::WorldIterator particle, FYC::World::WorldIterator other, FYC::Collision collisionData)
{
	if (particle.GetID() != m_CharacterController.MainCharacterParticle) return;

	if (collisionData.HalfWayInterpenetratingPoint.y > particle->GetPosition().y)
		m_CharacterController.CanJump = true;

	if (!other) {
		TryRestart();
	}

	if (std::find(m_EnnemisIds.begin(), m_EnnemisIds.end(), other.GetID()) != m_EnnemisIds.end()) {
		if (other->GetPosition().y - particle->GetPosition().y > m_EnnemiParameters.ThresholdKillEnnemi) {
			m_WorldPlay.RemoveParticle(other.GetID());
		} else {
			TryRestart();
		}
	}

	if (std::find(m_DeathPlatforms.begin(), m_DeathPlatforms.end(), other.GetID()) != m_DeathPlatforms.end()) {
		TryRestart();
	}

	if (other.GetID() == m_WinPlatform) {
		SetWinFlag(true);
	}
}

void Application::SetWinFlag(bool win) {
	m_HasWon = win;
}

void Application::OnStop()
{
	m_CharacterController.CanJump = false;
	m_WorldPlay.RemoveAllCallback();
	m_HasWon = false;
}

void Application::OnPlay()
{
	m_HasWon = false;
	if (!m_WorldPlay.GetParticle(m_CharacterController.MainCharacterParticle)) return;
	m_WorldPlay.SetCallback(m_CharacterController.MainCharacterParticle, [this](FYC::World::WorldIterator particle, FYC::World::WorldIterator other, FYC::Collision collisionData){OnCollision(particle,other,collisionData);});
}

void Application::OnPause()
{

}

void Application::OnResume()
{

}


Application::~Application() {
	// De-Initialization
	//--------------------------------------------------------------------------------------
	rlImGuiShutdown(); // cleans up ImGui
	CloseWindow(); // Close window and OpenGL context
	//--------------------------------------------------------------------------------------
}
