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

Application::Application(int width, int height, const std::string &name, bool isEditing)
	: m_IsEditing(isEditing), m_Width(width), m_Height(height), m_Camera(static_cast<FYC::Real>(m_Width), static_cast<FYC::Real>(m_Height), 30) {
	// Initialization
	//--------------------------------------------------------------------------------------
	SetConfigFlags(FLAG_WINDOW_RESIZABLE); // Window configuration flags
	InitWindow(width, height, name.c_str());

	rlImGuiSetup(true); // sets up ImGui with either a dark or light default theme
	LoadAll();

	if (!m_IsEditing) Play();
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

void Application::UpdateCharacter(FYC::Real stepTime) {
	FYC::Particle* ptr = m_WorldPlay.GetParticle(m_CharacterController.MainCharacter);
	if (!ptr) return;

	FYC::Vec2 movement{0};
	if (IsKeyDown(m_CharacterController.LeftKey)) movement.x -= 1;
	if (IsKeyDown(m_CharacterController.RightKey)) movement.x += 1;
	if (movement.x != 0) {
		ptr->AddAcceleration(movement * m_CharacterController.MovementAcceleration);
	}

	if (IsKeyPressed(m_CharacterController.JumpKey) && m_CharacterController.CanJump) {
		ptr->SubAcceleration({0, m_CharacterController.JumpImpulse / stepTime});
		ptr->SetPosition(ptr->GetPosition() - FYC::Vec2{0,0.1});
		m_CharacterController.CanJump = false;
	}
}

void Application::UpdateEnemies(FYC::Real stepTime) {
	for (auto id : m_EnemyIds) {
		auto* particle = m_WorldPlay.GetParticle(id);
		if (!particle) continue;
		auto vel = particle->GetVelocity();
		particle->SetVelocity({FYC::Math::Sign(vel.x) * m_EnemyParameters.Speed, vel.y});
	}
}

void Application::UpdateCamera() {
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
}

void Application::UpdateLogic() {
	m_Camera.SetViewport(m_Width, m_Height);

	if (m_IsEditing) UpdateCamera();

	if (m_PhysicsMode == PhysicsMode::Play) {
		if (IsKeyPressed(KeyboardKey::KEY_E)) TryRestart();
		float stepTime = std::min(GetFrameTime(), 1.0f);

		UpdateCharacter(stepTime);
		m_WorldPlay.Step(stepTime);
		UpdateEnemies(stepTime);

		if (m_ShouldStop) {
			Stop();
		}
	}

	if (m_ShouldPlay) {
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

void Application::ClearWorld() {
	m_WorldPlay = m_WorldEdit = FYC::World{};
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

void Application::ClearCharacter() {
	m_CharacterController = {};
}

void Application::LoadCharacter(const std::filesystem::path &filepath)
{
	std::ifstream file(filepath, std::ios::in  | std::ios::binary);
	if (file) {
		std::vector<char> binary((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		if (binary.size() != sizeof(m_CharacterController)) return;
		m_CharacterController = *reinterpret_cast<FYC::Application::CharacterController*>(binary.data());
	}
}

void Application::SaveCharacter(const std::filesystem::path &filename) const
{
	std::ofstream file(filename, std::ios::out  | std::ios::binary | std::ios::trunc);
	if (file) {
		file.write(reinterpret_cast<const char*>(&m_CharacterController), sizeof(m_CharacterController));
	}
}

void Application::ClearDeadlyPlatforms()
{
	m_DeadlyPlatform = {};
}

void Application::LoadDeadlyPlatforms(const std::filesystem::path &filepath)
{
	std::ifstream file(filepath, std::ios::in  | std::ios::binary);
	if (file) {
		std::vector<char> binary((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		uint64_t count = binary.size() / sizeof(FYC::World::ID);
		m_DeadlyPlatform.clear();
		m_DeadlyPlatform.assign(reinterpret_cast<FYC::World::ID*>(binary.data()), reinterpret_cast<FYC::World::ID*>(binary.data()) + count);
	}
}

void Application::SaveDeadlyPlatforms(const std::filesystem::path &filename) const
{
	std::ofstream file(filename, std::ios::out  | std::ios::binary | std::ios::trunc);
	if (file) {
		file.write(reinterpret_cast<const char*>(m_DeadlyPlatform.data()), sizeof(FYC::World::ID) * m_DeadlyPlatform.size());
	}
}

void Application::ClearEnemies()
{
	m_EnemyParameters = {};
	m_EnemyIds = {};
}

void Application::LoadEnemies(const std::filesystem::path &filepath)
{
	std::ifstream file(filepath, std::ios::in  | std::ios::binary);
	if (file) {
		std::vector<char> binary((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		if (binary.size() < sizeof(FYC::Application::EnemyParameters)) return;
		const auto* params = reinterpret_cast<const FYC::Application::EnemyParameters*>(binary.data());
		m_EnemyParameters = *params;

		uint64_t count = (binary.size() - sizeof(FYC::Application::EnemyParameters)) / sizeof(FYC::World::ID);
		m_EnemyIds.clear();
		m_EnemyIds.assign(
			reinterpret_cast<FYC::World::ID*>(binary.data() + sizeof(FYC::Application::EnemyParameters)),
			reinterpret_cast<FYC::World::ID*>(binary.data() + sizeof(FYC::Application::EnemyParameters)) + count);
	}
}

void Application::SaveEnemies(const std::filesystem::path &filename) const
{
	std::ofstream file(filename, std::ios::out  | std::ios::binary | std::ios::trunc);
	if (file) {
		file.write(reinterpret_cast<const char*>(&m_EnemyParameters), sizeof(m_EnemyParameters))
			.write(reinterpret_cast<const char*>(m_EnemyIds.data()), sizeof(FYC::World::ID) * m_EnemyIds.size());
	}
}


void Application::ClearEndPlatform()
{
	m_EndPlatform = {};
}
void Application::LoadEndPlatform(const std::filesystem::path &filepath)
{
	std::ifstream file(filepath, std::ios::in  | std::ios::binary);
	if (file) {
		std::vector<char> binary((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		if (binary.size() != sizeof(m_EndPlatform)) return;
		m_EndPlatform = *reinterpret_cast<FYC::World::ID*>(binary.data());
	}
}
void Application::SaveEndPlatform(const std::filesystem::path &filename) const
{
	std::ofstream file(filename, std::ios::out  | std::ios::binary | std::ios::trunc);
	if (file) {
		file.write(reinterpret_cast<const char*>(&m_EndPlatform), sizeof(m_EndPlatform));
	}
}

void Application::ClearAll() {
	ClearWorld();
	ClearCharacter();
	ClearDeadlyPlatforms();
	ClearEnemies();
	ClearEndPlatform();
}

void Application::LoadAll()
{
	LoadWorld(c_WorldFilePath);
	LoadCharacter(c_CharacterFilePath);
	LoadDeadlyPlatforms(c_DeadlyPlatformsFilePath);
	LoadEnemies(c_EnemiesFilePath);
	LoadEndPlatform(c_EndPlatformFilePath);
}

void Application::SaveAll() const
{
	SaveWorld(c_WorldFilePath);
	SaveCharacter(c_CharacterFilePath);
	SaveDeadlyPlatforms(c_DeadlyPlatformsFilePath);
	SaveEnemies(c_EnemiesFilePath);
	SaveEndPlatform(c_EndPlatformFilePath);
}


void Application::TryStop()
{
	m_ShouldStop = true;
}

void Application::TryPlay()
{
	m_ShouldPlay = true;
}

void Application::TryRestart()
{
	TryStop();
	TryPlay();
}

void Application::Stop() {
	m_PhysicsMode = PhysicsMode::Edit;
	m_ShouldStop = false;
	m_WorldPlay.RemoveAllCallback();
}

void Application::Pause(bool isPause) {
	m_PhysicsMode = isPause ? PhysicsMode::Pause : PhysicsMode::Play;
}

void Application::Play() {
	m_PhysicsMode = PhysicsMode::Play;
	m_WorldPlay = m_WorldEdit;
	m_ShouldPlay = false;
	m_HasWon = false;
	if(m_WorldPlay.GetParticle(m_CharacterController.MainCharacter)) m_WorldPlay.SetCallback(m_CharacterController.MainCharacter, [this](FYC::World::WorldIterator particle, FYC::World::WorldIterator other, FYC::Collision collision){OnCollision(particle, other, collision);});
}

bool Application::RenderImGuiCamera() {
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
	return false;
}

Application::ImGuiParticleResult Application::RenderImGuiParticle(FYC::World::WorldIterator it, bool canBeDeleted) {
	ImGuiParticleResult result;

	FYC::Particle& particle = *it;
	ImGui::PushID(reinterpret_cast<void*>(it.GetID()));
	if (!particle.HasShape<FYC::AABB>()) {
		if (ImGui::Button("Change Shape to Rectangle")) {
			particle.SetRectangleSize({1,1});
			result.hasChanged = true;
		}
	}
	if (!particle.HasShape<FYC::Circle>()) {
		if (ImGui::Button("Change Shape to Circle")) {
			particle.SetCircleRadius(0.5);
			result.hasChanged = true
		;}
	}

	bool isKinematic = particle.IsKinematic();
	auto position = particle.GetPosition();

	std::vector<FYC::World::ID>::iterator objectIt;
	bool isEndPlatform = m_EndPlatform == it.GetID();
	bool isMainCharacter = m_CharacterController.MainCharacter == it.GetID();
	bool isDeadlyPlatform = !isMainCharacter && (objectIt = std::find(m_DeadlyPlatform.begin(), m_DeadlyPlatform.end(), it.GetID())) != m_DeadlyPlatform.end();
	bool isEnemy = !isMainCharacter && !isDeadlyPlatform && (objectIt = std::find(m_EnemyIds.begin(), m_EnemyIds.end(), it.GetID())) != m_EnemyIds.end();

	if (isKinematic && !isDeadlyPlatform && !isEnemy && !isEndPlatform) {
		if (isMainCharacter && ImGui::Button("Unset Main Character")) {
			m_CharacterController.MainCharacter = FYC::World::NULL_ID;
			result.hasChanged = true;
		}
		else if (!isMainCharacter && ImGui::Button("Set as Main Character")) {
			m_CharacterController.MainCharacter = it.GetID();
			result.hasChanged = true;
		}
	}

	if (!isMainCharacter && !isEnemy && !isEndPlatform) {
		if (isDeadlyPlatform) {
			if (ImGui::Button("Unset Deadly Platform")) {
				m_DeadlyPlatform.erase(objectIt);
				result.hasChanged = true;
			}
		} else {
			if (ImGui::Button("Set as Deadly Platform")) {
				m_DeadlyPlatform.push_back(it.GetID());
				result.hasChanged = true;
			}
		}
	}

	if (isKinematic && !isMainCharacter && !isDeadlyPlatform && !isEndPlatform) {
		if (isEnemy) {
			if (ImGui::Button("Unset Enemy")) {
				m_EnemyIds.erase(objectIt);
				result.hasChanged = true;
			}
		} else {
			if (ImGui::Button("Set as Enemy")) {
				m_EnemyIds.push_back(it.GetID());
				result.hasChanged = true;
			}
		}
	}

	if (!isMainCharacter && !isDeadlyPlatform && !isEnemy) {
		if (isEndPlatform) {
			if (ImGui::Button("Unset End Platform")) {
				m_EndPlatform = it.GetID();
				result.hasChanged = true;
				SaveEndPlatform(c_EndPlatformFilePath);
			}
		} else {
			if (ImGui::Button("Set as End Platform")) {
				m_EndPlatform = it.GetID();
				result.hasChanged = true;
				SaveEndPlatform(c_EndPlatformFilePath);
			}
		}
	}

	if (ImGui::Checkbox("Is Kinematic", &isKinematic)) {
		particle.SetKinematic(isKinematic);
		result.hasChanged = true;
	}
	if (ImGuiLib::DragReal2("Position", position.data, 0.1)) {
		particle.SetPosition(position);
		result.hasChanged = true;
	}
	if (isKinematic) {
		auto velocity = particle.GetVelocity();
		if (ImGuiLib::DragReal2("Velocity", velocity.data, 0.1)) {
			particle.SetVelocity(velocity);
			result.hasChanged = true;
		}
		FYC::Vec2 acc = particle.GetConstantAccelerations();
		if (ImGuiLib::DragReal2("Acceleration", acc.data, 0.1)) {
			particle.SetConstantAcceleration(acc);
			result.hasChanged = true;
		}
		FYC::Real rebound = particle.GetRebound();
		if (ImGuiLib::SliderReal("Rebound", &rebound, 0, 1)) {
			particle.SetRebound(rebound);
			result.hasChanged = true;
		}
		FYC::Real inv_drag = 1-particle.GetDrag();
		if (ImGuiLib::SliderReal("Inverse Drag", &inv_drag, 1, 0, "%.5f", ImGuiSliderFlags_NoRoundToFormat | ImGuiSliderFlags_Logarithmic)) {
			particle.SetDrag(1-inv_drag);
			 result.hasChanged = true;
		}
		bool awake = particle.IsAwake();
		if (ImGui::Checkbox("Awake", &awake)) {
			particle.SetIsAwake(awake);
			result.hasChanged = true;
		}
	}

	static_assert(std::is_same<FYC::Particle::Shape, std::variant<FYC::Circle, FYC::AABB> >());
	if (auto maybeRadius = particle.GetCircleRadius()) {
		FYC::Real radius = maybeRadius.value();
		if (ImGuiLib::DragReal("Radius", &radius, 0.01, 0.01, REAL_MAX, "%.2f")) {
			particle.TrySetCircleRadius(radius);
			result.hasChanged = true;
		}
	} else if (auto maybeSize = particle.GetRectangleSize()) {
		FYC::Vec2 size = maybeSize.value();
		if (ImGuiLib::DragReal2("Size", size.data, 0.01, 0.01, REAL_MAX, "%.2f")) {
			particle.TrySetRectangleSize(size);
			result.hasChanged = true;
		}
	}

	if (particle.Data.type() == typeid(Color)) {
		Color color = std::any_cast<Color>(particle.Data);
		float color3[3] = {color.r / 255.f, color.g / 255.f, color.b / 255.f};
		if (ImGui::ColorEdit3("Color", color3, ImGuiColorEditFlags_Uint8)) {
			particle.Data = Color{(unsigned char)(color3[0]*255), (unsigned char)(color3[1]*255), (unsigned char)(color3[2]*255), 255};
			result.hasChanged = true;
		}
	}
	if (canBeDeleted && ImGui::Button("Delete")) {
		result.shouldLive = false;
	}
	ImGui::Separator();
	ImGui::PopID();

	return result;
}

bool Application::RenderImGuiPhysics() {
	bool hasChanged = false;
	ImGui::SetNextWindowSize({400, 400}, ImGuiCond_Once);
	ImGui::Begin("Physics"); {

		ImGui::BeginDisabled(!IsEdit());
		if (ImGui::Button("Save")) {
			SaveWorld(c_WorldFilePath);
		}

		if (ImGui::Button("Load")) {
			LoadWorld(c_WorldFilePath);
		}

		if (ImGui::Button("Clear")) {
			ClearWorld();
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

		if (!IsEdit()) {
			bool isPause = m_PhysicsMode == PhysicsMode::Pause;
			if (ImGui::Checkbox("Pause", &isPause)) {
				Pause(isPause);
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
				hasChanged = true;
			}
		}

		ImGui::Separator();

		std::vector<FYC::World::ID> toDelete;

		// Adding this variable to prevent removing object from the wrong world.
		auto& currentWorld = GetWorld();
		ImGui::PushID(&currentWorld);
		for (auto it = currentWorld.begin() ; it != currentWorld.end(); ++it) {
			ImGuiParticleResult result = RenderImGuiParticle(it);
			hasChanged |= result.hasChanged || !result.shouldLive;
			if (!result.shouldLive) {
				toDelete.push_back(it.GetID());
			}
		}
		ImGui::PopID();

		for (const FYC::World::ID id : toDelete) {
			currentWorld.RemoveParticle(id);
		}
	}
	ImGui::End();
	return hasChanged;
}

bool Application::RenderImGuiCharacter() {
	bool hasChanged = false;
	ImGui::SetNextWindowSize({400, 200}, ImGuiCond_Once);
	ImGui::Begin("Character");
	{
		ImGui::BeginDisabled(!IsEdit());
		if (ImGui::Button("Save")) SaveCharacter(c_CharacterFilePath);
		if (ImGui::Button("Load")) LoadCharacter(c_CharacterFilePath);
		if (ImGui::Button("Clear")) ClearCharacter();
		ImGui::EndDisabled();

		FYC::Particle* ptr = GetWorld().GetParticle(m_CharacterController.MainCharacter);
		if (ptr) {
			auto[particleHasChanged, particleShouldLive] = RenderImGuiParticle({GetWorld(), m_CharacterController.MainCharacter}, false);
			hasChanged |= particleHasChanged;
		} else {
			ImGui::Text("No Main Character in the scene");
			ImGui::Separator();
		}

		hasChanged |= ImGuiLib::DragReal("Movement Acceleration", &m_CharacterController.MovementAcceleration, 0.1);
		hasChanged |= ImGuiLib::DragReal("Jump Impulse", &m_CharacterController.JumpImpulse, 0.1);
		hasChanged |= ImGui::Checkbox("Can Jump", &m_CharacterController.CanJump);
	}
	ImGui::End();
	return hasChanged;
}

bool Application::RenderImGuiDeadlyPlatforms() {
	bool changed = false;
	ImGui::SetNextWindowSize({400, 500}, ImGuiCond_Once);
	ImGui::Begin("Deadly Platforms");

	ImGui::BeginDisabled(!IsEdit());
	if (ImGui::Button("Save")) SaveDeadlyPlatforms(c_DeadlyPlatformsFilePath);
	if (ImGui::Button("Load")) SaveDeadlyPlatforms(c_DeadlyPlatformsFilePath);
	if (ImGui::Button("Clear")) ClearDeadlyPlatforms();
	ImGui::EndDisabled();

	for (auto id : m_DeadlyPlatform) {
		auto it = GetWorld().find(id);
		if (it == GetWorld().end()) continue;
		auto result = RenderImGuiParticle(it, false);
		changed |= result.hasChanged;
	}
	ImGui::End();
	return changed;
}

bool Application::RenderImGuiEnemies() {
	bool changed = false;
	ImGui::SetNextWindowSize({400, 500}, ImGuiCond_Once);
	ImGui::Begin("Enemies");

	ImGui::BeginDisabled(!IsEdit());
	if (ImGui::Button("Save")) SaveEnemies(c_EnemiesFilePath);
	if (ImGui::Button("Load")) LoadEnemies(c_EnemiesFilePath);
	if (ImGui::Button("Clear")) ClearEnemies();
	ImGui::EndDisabled();

	for (auto id : m_EnemyIds) {
		auto it = GetWorld().find(id);
		if (it == GetWorld().end()) continue;
		auto result = RenderImGuiParticle(it, false);
		changed |= result.hasChanged;
	}

	changed |= ImGuiLib::DragReal("Speed", &m_EnemyParameters.Speed);
	changed |= ImGuiLib::DragReal("Kill Threshold", &m_EnemyParameters.KillThreshold);

	ImGui::End();
	return changed;
}

void Application::OnCollision(FYC::World::WorldIterator particle, FYC::World::WorldIterator other, FYC::Collision collision) {
	if (particle.GetID() != m_CharacterController.MainCharacter) return;

	if (collision.HalfWayInterpenetratingPoint.y > particle->GetPosition().y)
		m_CharacterController.CanJump = true;

	if (other.GetID() == FYC::World::NULL_ID) {
		TryRestart();
	}

	if (std::find(m_DeadlyPlatform.begin(), m_DeadlyPlatform.end(), other.GetID()) != m_DeadlyPlatform.end()) {
		TryRestart();
	}

	if (std::find(m_EnemyIds.begin(), m_EnemyIds.end(), other.GetID()) != m_EnemyIds.end()) {
		if (other->GetPosition().y - particle->GetPosition().y > m_EnemyParameters.KillThreshold) {
			m_WorldPlay.RemoveParticle(other.GetID());
		} else {
			TryRestart();
		}
	}

	if (other.GetID() == m_EndPlatform) {
		m_HasWon = true;
	}
}

void Application::RenderImGui() {
	static bool showDemo = true;
	if (showDemo) ImGui::ShowDemoWindow(&showDemo);

	bool hasChanged = false;
	hasChanged |= RenderImGuiCamera();
	hasChanged |= RenderImGuiCharacter();
	hasChanged |= RenderImGuiDeadlyPlatforms();
	hasChanged |= RenderImGuiEnemies();
	hasChanged |= RenderImGuiPhysics();

	if (hasChanged && IsEdit()) {
		TraceLog(TraceLogLevel::LOG_INFO, "Saving...");
		SaveAll();
	}
}

void Application::UpdateUI() {
	if (!IsEdit()) {
		DrawText("To move use 'Q'/'D'", 10, 10, 26, Color{30, 40, 50, 255});
		DrawText("To Jump press 'Space'", 10, 40, 26, Color{30, 40, 50, 255});
		DrawText("To restart press 'E'", 10, 70, 26, Color{30, 40, 50, 255});
	}

	if (!IsEdit() && m_HasWon) {
		static const char* const YouWonText = "YOU WON !!!";
		static constexpr int fontSize = 46;
		int textSize = MeasureText(YouWonText, fontSize);
		DrawText(YouWonText, m_Width / 2 - textSize / 2, m_Height / 4, fontSize, Color{180, 50, 40, 180});
	}

	if (m_IsEditing) RenderImGui();
}

Application::~Application() {
	// De-Initialization
	//--------------------------------------------------------------------------------------
	rlImGuiShutdown(); // cleans up ImGui
	CloseWindow(); // Close window and OpenGL context
	//--------------------------------------------------------------------------------------
}
