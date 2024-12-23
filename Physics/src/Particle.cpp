//
// Created by ianpo on 23/12/2024.
//

#include "Physics/Particle.hpp"

namespace FYC {
	Particle::~Particle() { }

	Particle::Particle() { }

	Particle::Particle(const Vec2 &position) : m_Position(position) { }

	Particle::Particle(const Vec2 &position, const Vec2 &velocity) : m_Position(position), m_Velocity(velocity) { }

	Particle::Particle(const Vec2 &position, const Vec2 &velocity, const Vec2 &constantAcceleration) : m_Position(position), m_Velocity(velocity), m_ConstantAccelerations(constantAcceleration) { }

	void Particle::AddConstantAcceleration(const Vec2 &constantAcceleration) {
		m_ConstantAccelerations += constantAcceleration;
	}

	void Particle::SubConstantAcceleration(const Vec2 &constantAcceleration) {
		m_ConstantAccelerations -= constantAcceleration;
	}

	void Particle::AddAcceleration(const Vec2 &acceleration) {
		m_SummedAccelerations += acceleration;
	}

	void Particle::SubAcceleration(const Vec2 &acceleration) {
		m_SummedAccelerations -= acceleration;
	}
} // FYC