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
		bool IsColliding;
		Vec2 CollisionPoint;
		Vec2 CollisionNormal;
		Real Interpenetration;

		[[nodiscard]] explicit operator bool() const {return IsColliding;}
	};

	class Collider {
	public:
		static Collision Collide(const Circle& a, const Circle& b);
	};

} // FYC
