//
// Created by ianpo on 23/12/2024.
//

#include "Physics/Particle.hpp"

#include "Physics/AABB.hpp"

namespace FYC {
	struct AABB;

	Particle::~Particle() = default;

	Particle::Particle() : m_Shape(Circle{{0,0}, 1})
	{
	}

	Particle::Particle(const Shape& shape) : m_Shape(shape)
	{
	}

	Particle::Particle(const Shape& shape, const Vec2 &velocity) : m_Shape(shape), m_Velocity(velocity)
	{
	}

	Particle::Particle(const Shape& shape, const Vec2 &velocity, const Vec2 &constantAcceleration) : m_Shape(shape), m_Velocity(velocity), m_ConstantAccelerations(constantAcceleration)
	{
	}

	Particle Particle::CreateCircle(const Vec2& position, const Real radius)
	{
		return Particle{Circle{position, radius}};
	}

	Particle Particle::CreateCircle(const Vec2& position, const Real radius, const Vec2 &velocity)
	{
		return Particle{Circle{position, radius}, velocity};
	}

	Particle Particle::CreateCircle(const Vec2& position, const Real radius, const Vec2 &velocity, const Vec2 &constantAcceleration)
	{
		return Particle{Circle{position, radius}, velocity, constantAcceleration};
	}

	Particle Particle::CreateRectangle(const Vec2& position, const Vec2& size)
	{
		return Particle{AABB::FromCenterSize(position, size)};
	}

	Particle Particle::CreateRectangle(const Vec2& position, const Vec2& size, const Vec2& velocity)
	{
		return Particle{AABB::FromCenterSize(position, size), velocity};
	}

	Particle Particle::CreateRectangle(const Vec2& position, const Vec2& size, const Vec2& velocity, const Vec2& constantAcceleration)
	{
		return Particle{AABB::FromCenterSize(position, size), velocity, constantAcceleration};
	}

	Particle::Particle(Particle&& other) noexcept
		: Data(std::move(other.Data)),
		  m_Shape(std::move(other.m_Shape)),
		  m_Velocity(std::move(other.m_Velocity)),
		  m_ConstantAccelerations(std::move(other.m_ConstantAccelerations)),
		  m_SummedAccelerations(std::move(other.m_SummedAccelerations)) {
	}

	Particle &Particle::operator=(Particle &&other) noexcept {
		swap(other);
		return *this;
	}

	void Particle::AddConstantAcceleration(const Vec2 &constantAcceleration) {
		m_ConstantAccelerations += constantAcceleration;
	}

	void Particle::SubConstantAcceleration(const Vec2 &constantAcceleration) {
		m_ConstantAccelerations -= constantAcceleration;
	}

	void Particle::SetConstantAcceleration(const Vec2 &constantAcceleration) {
		m_ConstantAccelerations = constantAcceleration;
	}

	void Particle::AddAcceleration(const Vec2 &acceleration) {
		m_SummedAccelerations += acceleration;
	}

	void Particle::SubAcceleration(const Vec2 &acceleration) {
		m_SummedAccelerations -= acceleration;
	}

	void Particle::SetAcceleration(const Vec2 &acceleration) {
		m_SummedAccelerations = acceleration;
	}

	void Particle::SetPosition(const Vec2 &position) {
		static_assert(std::is_same<Shape, std::variant<Circle, AABB>>());
		if (Circle *circle = std::get_if<Circle>(&m_Shape)) {
			circle->Position = position;
		}
		else if(AABB* aabb = std::get_if<AABB>(&m_Shape)) {
			*aabb = AABB::FromCenterSize(position, aabb->GetSize());
		}
	}

	Vec2 Particle::GetPosition() const {
		static_assert(std::is_same<Shape, std::variant<Circle, AABB>>());
		if (const Circle *circle = std::get_if<Circle>(&m_Shape)) {
			return circle->Position;
		}
		else if(const AABB* aabb = std::get_if<AABB>(&m_Shape)) {
			return aabb->GetCenter();
		}
		return {NAN, NAN};
	}

	void Particle::SetVelocity(const Vec2 &velocity) { m_Velocity = velocity; }
	Vec2 Particle::GetVelocity() const { return m_Velocity; }

	void Particle::SetKinematic(const bool isKinematic) { m_IsKinematic = isKinematic; }
	bool Particle::IsKinematic() const { return m_IsKinematic; }

	void Particle::SetRebound(const Real rebound) { m_Rebound = rebound; }
	Real Particle::GetRebound() const { return m_Rebound; }

	Real Particle::GetInverseMass() const {
		return m_IsKinematic ? 1 : 0;
	}

	void Particle::swap(Particle &other) noexcept {
		std::swap(Data, other.Data);
		std::swap(m_Shape, other.m_Shape);
		std::swap(m_Velocity, other.m_Velocity);
		std::swap(m_ConstantAccelerations, other.m_ConstantAccelerations);
		std::swap(m_SummedAccelerations, other.m_SummedAccelerations);
	}

	std::optional<Real> Particle::GetCircleRadius() const {
		if (const Circle *circle = std::get_if<Circle>(&m_Shape)) {
			return circle->Radius;
		}
		return std::nullopt;
	}

	void Particle::SetCircleRadius(const Real radius) {
		if (Circle *circle = std::get_if<Circle>(&m_Shape)) {
			circle->Radius = radius;
		} else {
			Vec2 particlePosition = GetPosition();
			m_Shape = Circle{particlePosition, radius};
		}
	}

	bool Particle::TrySetCircleRadius(Real radius)
	{
		if (Circle *circle = std::get_if<Circle>(&m_Shape)) {
			circle->Radius = radius;
			return true;
		}
		return false;
	}

	std::optional<Vec2> Particle::GetRectangleSize() const {
		if (const AABB* aabb = std::get_if<AABB>(&m_Shape)) {
			return aabb->GetSize();
		}
		return std::nullopt;
	}

	void Particle::SetRectangleSize(const Vec2 &size) {
		if (AABB* aabb = std::get_if<AABB>(&m_Shape)) {
			*aabb = AABB::FromCenterSize(aabb->GetCenter(), size);
		} else {
			const Vec2 particlePosition = GetPosition();
			m_Shape = AABB::FromCenterSize(particlePosition, size);
		}

	}

	bool Particle::TrySetRectangleSize(const Vec2 &size) {
		if (AABB* aabb = std::get_if<AABB>(&m_Shape)) {
			*aabb = AABB::FromCenterSize(aabb->GetCenter(), size);
			return true;
		}
		return false;
	}
} // FYC
