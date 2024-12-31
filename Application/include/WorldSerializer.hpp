//
// Created by ianpo on 31/12/2024.
//

#pragma once

#include "rlImGuiColors.h"
#include "Physics/Math.hpp"
#include "Physics/AABB.hpp"
#include "Physics/Circle.hpp"
#include "Physics/Particle.hpp"
#include "Physics/World.hpp"

namespace FYC::Application {

	struct SerializedParticle {
		enum Shape : uint8_t {CIRCLE, RECTANGLE};
		World::ID id;
		Vec2 position;
		Vec2 velocity;
		Vec2 constantAccelerations;
		Real rebound;
		Real drag;
		bool isKinematic;
		bool isAwake;
		Shape shapeType;
		Circle circle;
		AABB rectangle;
		Color color;

		static std::pair<FYC::Particle, World::ID> ToParticle(const SerializedParticle& serializedParticle);
		static SerializedParticle ToSerializedParticle(const FYC::Particle& particle, World::ID id);
	};

	struct SerializedBounds {
		AABB bounds;
		bool hasBounds;
	};

	class WorldSerializer {
	public:
		static std::vector<char> ToBinary(World world);
		static World FromBinary(const std::vector<char>& binary);
	};

}
