//
// Created by Sayama on 03/12/2024.
//

#pragma once

#include <raylib.h>

#include "Camera.hpp"
#include "CharacterController.hpp"
#include "EnnemiParameters.hpp"
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
	inline static const char* const c_WorldSaveFile {"world.fyc"};
	inline static const char* const c_CharacterSaveFile {"character.fyc"};
	inline static const char* const c_EnnemiSaveFile {"ennemi.fyc"};
	inline static const char* const c_DeathPlatformsSaveFile {"deathplatform.fyc"};
	inline static const char* const c_WinPlatformsSaveFile {"win.fyc"};
public:
	void Run();
private:
	void Update();

	void UpdateLogic();
	void UpdateRendering();

	void LoadWorld(const std::filesystem::path &filepath);
	void SaveWorld(const std::filesystem::path &filename) const;

	void LoadCharacter(const std::filesystem::path &filepath);
	void SaveCharacter(const std::filesystem::path &filename) const;

	void LoadEnnemis(const std::filesystem::path &filepath);
	void SaveEnnemis(const std::filesystem::path &filename) const;

	void LoadDeathPlatforms(const std::filesystem::path &filepath);
	void SaveDeathPlatforms(const std::filesystem::path &filename) const;

	void LoadWinPlatforms(const std::filesystem::path &filepath);
	void SaveWinPlatforms(const std::filesystem::path &filename) const;

	void TryStop();
	void TryRestart();

	void Stop();
	void Play();
	void SetPause(bool isPause);

	void UpdateUI();
private:
	void UpdateCharacter(FYC::Real stepTime);
	void UpdateEnnemies(FYC::Real stepTime);
	void OnCollision(FYC::World::WorldIterator particle, FYC::World::WorldIterator other, FYC::Collision collisionData);
	void SetWinFlag(bool win);
	void OnStop();
	void OnPlay();
	void OnPause();
	void OnResume();
private:
	FYC::World& GetWorld() {return m_PhysicsMode == PhysicsMode::Edit ? m_WorldEdit : m_WorldPlay ;}
private:
	FYC::Application::CharacterController m_CharacterController;
	FYC::Application::EnnemiParameters m_EnnemiParameters;
	std::vector<FYC::World::ID> m_EnnemisIds;
	std::vector<FYC::World::ID> m_DeathPlatforms;
	FYC::World::ID m_WinPlatform = FYC::World::NULL_ID;
	bool m_ShouldStop = false;
	bool m_ShouldStart = false;
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
