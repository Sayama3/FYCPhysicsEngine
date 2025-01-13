//
// Created by ianpo on 13/01/2025.
//

#include "Canon.hpp"
#include "ImGuiLib.hpp"
#include "imgui.h"
#include <raylib.h>

namespace FYC::Application {

	void Canon::Draw() const {
		const Rectangle rect {
			m_CanonPosition.x, m_CanonPosition.y,
			m_CanonSize.x, m_CanonSize.y
		};
		DrawRectanglePro(rect, {0,m_CanonSize.y*static_cast<Real>(0.5)}, m_CanonAngleDegree, m_CanonColor);
	}

	std::optional<Particle> Canon::RenderImGui(const bool canShoot) {
		std::optional<Particle> p = std::nullopt;
		ImGui::Begin("Canon");
		{
			ImGuiLib::DragReal2("Position", m_CanonPosition.data, 0.1);
			ImGuiLib::DragReal2("Size", m_CanonSize.data, 0.1, 0.1, REAL_MAX);
			ImGuiLib::DragReal("Canon Angle Degree", &m_CanonAngleDegree, 0.1, -360, 360);
			ImGuiLib::DragReal("Canon Ball Radius", &m_CanonBallRadius, 0.1);
			ImGuiLib::DragReal("Canon Ball Initial Speed", &m_CanonBallInitialSpeed, 0.1);
			ImGui::Spacing();

			ImGui::BeginDisabled(!canShoot);
			if (ImGui::Button("Shoot")) {
				p = Shoot();
			}
			ImGui::EndDisabled();
		}
		ImGui::End();
		return p;
	}

	Particle Canon::Shoot() const {
		const Real rad = m_CanonAngleDegree * Math::deg2rad;
		const Real cos = std::cos(rad);
		const Real sin = std::sin(rad);
		Particle particle = Particle::CreateCircle(
			m_CanonPosition + Vec2{cos * m_CanonSize.x, sin * m_CanonSize.x},
			m_CanonBallRadius,
			Vec2{cos * m_CanonBallInitialSpeed,sin * m_CanonBallInitialSpeed},
			Vec2{0,10}
		);
		particle.Data = m_CanonColor;
		return std::move(particle);
	}
} // FYC