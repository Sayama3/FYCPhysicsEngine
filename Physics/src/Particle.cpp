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

	Particle::Particle(Particle &&other) noexcept
	:	Data(std::move(other.Data)),
		m_Position(std::move(other.m_Position)),
		m_Velocity(std::move(other.m_Velocity)),
		m_ConstantAccelerations(std::move(other.m_ConstantAccelerations)),
		m_SummedAccelerations(std::move(other.m_SummedAccelerations))
	{
	}

	Particle& Particle::operator=(Particle&& other) noexcept {
		swap(other);
		return *this;
	}

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

	void Particle::swap(Particle& other) noexcept
	{
		std::swap(Data, other.Data);
		std::swap(m_Position, other.m_Position);
		std::swap(m_Velocity, other.m_Velocity);
		std::swap(m_ConstantAccelerations, other.m_ConstantAccelerations);
		std::swap(m_SummedAccelerations, other.m_SummedAccelerations);
	}
} // FYC