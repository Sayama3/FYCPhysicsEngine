//
// Created by ianpo on 23/12/2024.
//

#pragma once

#include "Physics/Math.hpp"

namespace FYC {

	class Particle
	{
	public:
		Particle();
		~Particle();
		Particle(const Vec2& position);
		Particle(const Vec2& position, const Vec2& velocity);
		Particle(const Vec2& position, const Vec2& velocity, const Vec2& constantAcceleration);
	public:
		Particle(Particle&& other) noexcept;
		Particle& operator=(Particle&& other) noexcept;
		Particle(const Particle&) = default;
		Particle& operator=(const Particle&) = default;
	public:
		void AddConstantAcceleration(const Vec2& constantAcceleration);
		void SubConstantAcceleration(const Vec2& constantAcceleration);
		void SetConstantAcceleration(const Vec2& constantAcceleration);
		[[nodiscard]] Vec2 GetConstantAccelerations() const {return m_ConstantAccelerations;}

		void AddAcceleration(const Vec2& acceleration);
		void SubAcceleration(const Vec2& acceleration);
		void SetAcceleration(const Vec2& acceleration);
		[[nodiscard]] Vec2 GetAcceleration() const {return m_ConstantAccelerations;}
	public:
		void SetPosition(const Vec2& position);
		[[nodiscard]] Vec2 GetPosition() const;

		void SetVelocity(const Vec2& velocity);
		[[nodiscard]] Vec2 GetVelocity() const;

	public:
		void swap(Particle& other) noexcept;
	public:
		std::any Data;
	private:
		Vec2 m_Position;
		Vec2 m_Velocity;
		Vec2 m_ConstantAccelerations;
		Vec2 m_SummedAccelerations;
	};
} // FYC
