//
// Created by ianpo on 02/01/2025.
//

#pragma once

#include <raylib.h>
#include "Physics/Math.hpp"
#include "Physics/World.hpp"

namespace FYC::Application {
	struct CharacterController
	{
		World::ID MainCharacter = World::NULL_ID;

		KeyboardKey LeftKey = KeyboardKey::KEY_A;
		KeyboardKey RightKey = KeyboardKey::KEY_D;
		KeyboardKey JumpKey = KeyboardKey::KEY_SPACE;

		Real MovementAcceleration = 40;
		Real JumpImpulse = 50;
		bool CanJump = true;
	};
} // Application
// FYC
