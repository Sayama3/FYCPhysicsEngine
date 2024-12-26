//
// Created by ianpo on 24/12/2024.
//

#include "Physics/World.hpp"

namespace FYC {
	// ========== WorldIterator ==========
	World::WorldIterator::WorldIterator(World &world, uint64_t particleId) : m_World(&world), m_ParticleId(particleId) { }
	World::WorldIterator::WorldIterator(World *world, uint64_t particleId) : m_World(world), m_ParticleId(particleId) { }
	World::WorldIterator::WorldIterator() = default;
	World::WorldIterator::~WorldIterator() = default;

	World::WorldIterator& World::WorldIterator::operator++()
	{
		auto it = m_World->m_Particles.find(m_ParticleId);
		if (it == m_World->m_Particles.end() || ++it == m_World->m_Particles.end()) m_ParticleId = World::NULL_ID;
		else m_ParticleId = it->first;
		return *this;
	}

	World::WorldIterator World::WorldIterator::operator++(int) {
		const auto tmp = *this;
		++*this;
		return tmp;
	}

	bool World::WorldIterator::operator==(const WorldIterator& other) const
	{
		return this->m_World == other.m_World && this->m_ParticleId == other.m_ParticleId;
	}

	bool World::WorldIterator::operator!=(const WorldIterator &other) const {return !(*this == other);}

	World::WorldIterator::reference World::WorldIterator::operator*() {
		return m_World->m_Particles[m_ParticleId];
	}

	const World::WorldIterator::reference World::WorldIterator::operator*() const {
		return m_World->m_Particles[m_ParticleId];
	}

	World::WorldIterator::pointer World::WorldIterator::operator->() {
		return &m_World->m_Particles[m_ParticleId];
	}

	const World::WorldIterator::pointer World::WorldIterator::operator->() const {
		return &m_World->m_Particles[m_ParticleId];
	}

	// ========== World ==========
	World::World()
	{
		m_Particles.reserve(1024);
	}

	World::World(uint64_t reserveParticleCount)
	{
		m_Particles.reserve(reserveParticleCount);
	}

	World::~World() = default;

	World::WorldIterator World::AddParticle() {
		auto id = m_IDGenerator++;
		m_Particles.insert({id, Particle{}});
		return {*this, id};
	}

	World::WorldIterator World::AddParticle(const Particle::Shape& shape) {
		auto id = m_IDGenerator++;
		m_Particles.insert({id, Particle{shape}});
		return {*this, id};
	}

	World::WorldIterator World::AddParticle(const Particle::Shape& shape, const Vec2 &velocity) {
		auto id = m_IDGenerator++;
		m_Particles.insert({id, Particle{shape, velocity}});
		return {*this, id};
	}

	World::WorldIterator World::AddParticle(const Particle::Shape& shape, const Vec2 &velocity, const Vec2 &constantAcceleration) {
		auto id = m_IDGenerator++;
		m_Particles.insert({id, Particle{shape, velocity, constantAcceleration}});
		return {*this, id};
	}

	World::WorldIterator World::AddParticle(const Particle &particle) {
		auto id = m_IDGenerator++;
		m_Particles.insert({id, particle});
		return {*this, id};
	}

	World::WorldIterator World::AddParticle(Particle &&particle) {
		auto id = m_IDGenerator++;
		m_Particles.insert({id, std::move(particle)});
		return {*this, id};
	}

	Particle* World::GetParticle(const ID id)
	{
		const auto it = m_Particles.find(id);
		if (it != m_Particles.end()) return &it->second;
		else return nullptr;
	}

	const Particle* World::GetParticle(const ID id) const
	{
		const auto it = m_Particles.find(id);
		if (it != m_Particles.end()) return &it->second;
		else return nullptr;
	}

	void World::RemoveParticle(const ID id)
	{
		m_Particles.erase(id);
	}

	void World::Step(Real stepTime)
	{
		// Integration
		for (auto& particle : *this)
		{
			particle.SetPosition(particle.GetPosition() + particle.m_Velocity * stepTime);
			particle.SetVelocity(particle.GetVelocity() + particle.m_ConstantAccelerations * stepTime + particle.m_SummedAccelerations * stepTime);
			particle.m_SummedAccelerations = Vec2{};
		}

		// Collision Detection
		if (const AABB* boundsAABB = std::get_if<AABB>(&Bounds))
		{
			for (auto& particle : *this)
			{
				bool changed = false;
				Vec2 position = particle.GetPosition();
				Vec2 velocity = particle.GetVelocity();

				Vec2 halfSize{0};
				AABB particleAABB = AABB::FromCenterHalfSize(position, halfSize);

				if (const Circle* cirlce = std::get_if<Circle>(&particle.m_Shape)) {
					halfSize = Vec2{cirlce->Radius};
					particleAABB = AABB::FromCenterHalfSize(position, halfSize);
				}

				if (particleAABB.Min.x <= boundsAABB->Min.x) {
					position.x = boundsAABB->Min.x + halfSize.x;
					velocity.x *= -1;
					changed = true;
				} else if (particleAABB.Max.x >= boundsAABB->Max.x) {
					position.x = boundsAABB->Max.x - halfSize.x;
					velocity.x *= -1;
					changed = true;
				}

				if (particleAABB.Min.y <= boundsAABB->Min.y) {
					position.y = boundsAABB->Min.y + halfSize.y;
					velocity.y *= -1;
					changed = true;
				} else if (particleAABB.Max.y >= boundsAABB->Max.y) {
					position.y = boundsAABB->Max.y - halfSize.y;
					velocity.y *= -1;
					changed = true;
				}

				if (changed) {
					particle.SetPosition(position);
					particle.SetVelocity(velocity);
				}
			}
		}
	}
} // FYC