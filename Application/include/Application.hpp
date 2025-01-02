//
// Created by Sayama on 03/12/2024.
//

#pragma once

#include <raylib.h>

#include "Camera.hpp"
#include "CharacterController.hpp"
#include "EnemyParameters.hpp"
#include "Physics/World.hpp"

#if defined(PLATFORM_WEB)
void UpdateLoop(void* arg);
#endif

enum class PhysicsMode {
	Edit,
	Play,
	Pause,
};

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
	static inline constexpr bool c_IsEditing = false;
	static inline const std::filesystem::path c_WorldFilePath = "world.fyc";
	static inline const std::filesystem::path c_CharacterFilePath = "character.fyc";
	static inline const std::filesystem::path c_DeadlyPlatformsFilePath = "deadly_platforms.fyc";
	static inline const std::filesystem::path c_EnemiesFilePath = "enemies.fyc";
	static inline const std::filesystem::path c_EndPlatformFilePath = "end_platform.fyc";
public:
	void Run();
private:
	void Update();
	void UpdateCharacter(FYC::Real stepTime);
	void UpdateEnemies(FYC::Real stepTime);

	void UpdateCamera();

	void UpdateLogic();
	void UpdateRendering();

	void ClearWorld();
	void LoadWorld(const std::filesystem::path &filepath);
	void SaveWorld(const std::filesystem::path &filename) const;

	void ClearCharacter();
	void LoadCharacter(const std::filesystem::path &filepath);
	void SaveCharacter(const std::filesystem::path &filename) const;

	void ClearDeadlyPlatforms();
	void LoadDeadlyPlatforms(const std::filesystem::path &filepath);
	void SaveDeadlyPlatforms(const std::filesystem::path &filename) const;

	void ClearEnemies();
	void LoadEnemies(const std::filesystem::path &filepath);
	void SaveEnemies(const std::filesystem::path &filename) const;

	void ClearEndPlatform();
	void LoadEndPlatform(const std::filesystem::path &filepath);
	void SaveEndPlatform(const std::filesystem::path &filename) const;

	void ClearAll();
	void LoadAll();
	void SaveAll() const;

	void UpdateUI();
private:
	void TryStop();
	void TryPlay();
	void TryRestart();
	void Stop();
	void Play();

	void RenderImGui();
	bool RenderImGuiCamera();
	bool RenderImGuiPhysics();
	bool RenderImGuiCharacter();
	bool RenderImGuiDeadlyPlatforms();
	bool RenderImGuiEnemies();

	void OnCollision(FYC::World::WorldIterator particle, FYC::World::WorldIterator other, FYC::Collision collision);

	struct ImGuiParticleResult {bool hasChanged{false}; bool shouldLive{true};};
	ImGuiParticleResult RenderImGuiParticle(FYC::World::WorldIterator it, bool canBeDeleted = true);

	void Pause(bool isPause);
private:
	[[nodiscard]] inline bool IsPause() const {return m_PhysicsMode == PhysicsMode::Pause;}
	[[nodiscard]] inline bool IsPlay() const {return m_PhysicsMode == PhysicsMode::Play;}
	[[nodiscard]] inline bool IsEdit() const {return m_PhysicsMode == PhysicsMode::Edit;}
private:
	FYC::World& GetWorld() {return m_PhysicsMode == PhysicsMode::Edit ? m_WorldEdit : m_WorldPlay ;}
private:
	FYC::Application::CharacterController m_CharacterController;
	FYC::Application::EnemyParameters m_EnemyParameters;
	std::vector<FYC::World::ID> m_EnemyIds;
	std::vector<FYC::World::ID> m_DeadlyPlatform;
	FYC::World::ID m_EndPlatform = FYC::World::NULL_ID;
	bool m_ShouldStop = false;
	bool m_ShouldPlay = false;
	bool m_HasWon = false;
private:
	bool m_ImGuiIsActive = false;
	PhysicsMode m_PhysicsMode = PhysicsMode::Edit;
	uint32_t m_Width, m_Height;
	FYC::Real m_CameraMoveSpeed = 4.0;
	FYC::Real m_CameraMoveFastSpeed = 12.0;
	FYC::Real m_CameraZoomSpeed = 0.05;
	FYC::Application::Camera m_Camera;
	FYC::World m_WorldEdit;
	FYC::World m_WorldPlay;
};
