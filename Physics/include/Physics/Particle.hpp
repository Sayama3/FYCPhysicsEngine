//
// Created by ianpo on 23/12/2024.
//

#pragma once

#include "Physics/Math.hpp"
#include "Physics/Circle.hpp"
#include "Physics/AABB.hpp"

namespace FYC {
	class World;

	class Particle
	{
		friend class World;
	public:
		using Shape = std::variant<Circle, AABB>;
	public:
		Particle();
		~Particle();
		Particle(const Shape& shape);
		Particle(const Shape& shape, const Vec2& velocity);
		Particle(const Shape& shape, const Vec2& velocity, const Vec2& constantAcceleration);
	public:
		[[nodiscard]] static Particle CreateCircle(const Vec2& position, Real radius);
		[[nodiscard]] static Particle CreateCircle(const Vec2& position, Real radius, const Vec2& velocity);
		[[nodiscard]] static Particle CreateCircle(const Vec2& position, Real radius, const Vec2& velocity, const Vec2& constantAcceleration);
		[[nodiscard]] static Particle CreateRectangle(const Vec2& position, const Vec2& size);
		[[nodiscard]] static Particle CreateRectangle(const Vec2& position, const Vec2& size, const Vec2& velocity);
		[[nodiscard]] static Particle CreateRectangle(const Vec2& position, const Vec2& size, const Vec2& velocity, const Vec2& constantAcceleration);
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
		template<typename T>
		[[nodiscard]] bool HasShape() const { return std::holds_alternative<T>(m_Shape); }

		template<typename T>
		bool HasShape(T& value) const { const T* valuePtr = nullptr; if ((valuePtr = std::get_if<T>(&m_Shape))) value = *valuePtr; return valuePtr; }

		[[nodiscard]] std::optional<Real> GetCircleRadius() const;

		/**
		 * This function WILL set the shape to a Circle with a radius as mentioned in parameter.
		 * No matter what the shape was to begin with.
		 * @param radius The radius of the circle
		 */
		void SetCircleRadius(Real radius);

		/**
		 * This function will set the radius of the circle only if the shape is currently a circle.
		 * It will return whether it was a success or not.
		 * @param radius The radius of the circle
		 * @return Whether the shape was a Circle or not
		 */
		bool TrySetCircleRadius(Real radius);

		[[nodiscard]] std::optional<Vec2> GetRectangleSize() const;

		/**
		 * This function WILL set the shape to a Rectangle with a size as mentioned in parameter.
		 * No matter what the shape was to begin with.
		 * @param size The size of the rectangle
		 */
		void SetRectangleSize(const Vec2& size);

		/**
		 * This function will set the size of the rectangle only if the shape is currently a rectangle.
		 * It will return whether it was a success or not.
		 * @param size The size of the rectangle
		 * @return Whether the shape was a Rectangle or not
		 */
		bool TrySetRectangleSize(const Vec2& size);
	public:
		std::any Data;
	private:
		Shape m_Shape;
		Vec2 m_Velocity;
		Vec2 m_ConstantAccelerations;
		Vec2 m_SummedAccelerations;
	};
} // FYC
