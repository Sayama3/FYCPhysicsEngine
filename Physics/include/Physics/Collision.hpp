//
// Created by ianpo on 26/12/2024.
//

#pragma once

#include "Math.hpp"
#include "Particle.hpp"
#include "Circle.hpp"
#include "AABB.hpp"

namespace FYC {

	struct Collision {
		Vec2 HalfWayInterpenetratingPoint;
		Vec2 CollisionNormal;
		Real Interpenetration;
		bool IsColliding;

		[[nodiscard]] explicit operator bool() const {return IsColliding;}
	};

	class CollisionDetector {
	public:
		[[nodiscard]] static Collision Collide(const Circle& a, const Circle& b);
		[[nodiscard]] static Collision Collide(const AABB& a, const AABB& b);
		[[nodiscard]] static Collision Collide(const Circle& a, const AABB& b);
		[[nodiscard]] static Collision Collide(const AABB& a, const Circle& b);
	};

} // FYC
