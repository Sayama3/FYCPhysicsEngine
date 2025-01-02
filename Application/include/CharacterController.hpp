//
// Created by ianpo on 02/01/2025.
//

#pragma once

#include "Physics/Math.hpp"
#include "Physics/World.hpp"
#include <raylib.h>

namespace FYC::Application {
	class CharacterController {
	public:
		World::ID MainCharacterParticle = World::NULL_ID;
	public:
		KeyboardKey RightKey = KeyboardKey::KEY_D;
		KeyboardKey LeftKey = KeyboardKey::KEY_A;
		KeyboardKey JumpKey = KeyboardKey::KEY_SPACE;
	public:
		Real MovementAcceleration = 10;
		Real JumpImpulse = 5;
		bool CanJump = false;
	};
} // Application
// FYC
