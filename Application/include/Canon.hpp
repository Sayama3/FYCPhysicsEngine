//
// Created by ianpo on 13/01/2025.
//

#pragma once

#include <rlImGuiColors.h>
#include "Physics/Math.hpp"
#include "Physics/World.hpp"

namespace FYC::Application {

	class Canon {
	public:
		Canon() = default;
		~Canon() = default;
	public:
		[[nodiscard]] Particle Shoot() const;
		void Draw() const;
		std::optional<Particle> RenderImGui(bool canShoot);
	private:
		Vec2 m_CanonPosition = {-1,0};
		Vec2 m_CanonSize = {1.0,0.5};
		Color m_CanonColor = {180, 80, 40, 255};
		Real m_CanonAngleDegree = 0;
		Real m_CanonBallRadius = 0.2;
		Real m_CanonBallInitialSpeed = 10;
	};
} // Application
// FYC
