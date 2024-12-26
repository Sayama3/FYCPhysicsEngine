//
// Created by ianpo on 24/12/2024.
//

#include "Physics/World.hpp"

namespace FYC {
	// ========== WorldIterator ==========
	World::WorldIterator::WorldIterator(World &world, const uint64_t particleId) : m_World(&world), m_ParticleId(particleId) { }
	World::WorldIterator::WorldIterator(World *world, const uint64_t particleId) : m_World(world), m_ParticleId(particleId) { }
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
	World::World() {
		m_Particles.reserve(1024);
		m_Collisions.reserve(1024);
	}

	World::World(const uint64_t reserveParticleCount) {
		m_Particles.reserve(reserveParticleCount);
		m_Collisions.reserve(reserveParticleCount);
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

	void World::FindParticlesCollisions() {
		m_Collisions.clear();
		for (auto it = begin(); it != end(); ++it) {
			auto nextIt = it;
			++nextIt;
			for ( ;nextIt != end(); ++nextIt) {
				Collision collision;
				auto a = it.GetID() < nextIt.GetID() ? it : nextIt;
				auto b = it.GetID() < nextIt.GetID() ? nextIt : it;
				{
					Circle ca, cb;
					if (a->HasShape<Circle>(ca) && b->HasShape<Circle>(cb)) collision = CollisionDetector::Collide(ca,cb);
				}

				if (collision) {
					m_Collisions[{a.GetID(), b.GetID()}] = collision;
				}
			}
		}
	}

	void World::ResolveParticleCollisions() {
		for (const auto& [pair, collision] : m_Collisions)
		{
			Particle* particleA = GetParticle(pair.first);
			Particle* particleB = GetParticle(pair.second);

			if (!particleA || !particleB) continue;

			const Real inverseWeightA = 1; //TODO: Fetch the weight later on.
			const Real inverseWeightB = 1; //TODO: Fetch the weight later on.
			const Real totalInverseWeight = inverseWeightA + inverseWeightB;

			if (totalInverseWeight <= 0) continue;

			const Real rebound = 0.9;

			const Vec2 velA = particleA->GetVelocity();
			const Vec2 velB = particleB->GetVelocity();

			const Real contactVelocity = Math::Dot(collision.CollisionNormal, velA - velB);

			// Resolving velocity
			if (contactVelocity <= 0 && totalInverseWeight > 0) {
				const Real separatingVelocity = -contactVelocity * rebound;
				const Real deltaVelocity = separatingVelocity - contactVelocity;

				const Real separatingImpulse = deltaVelocity / totalInverseWeight;
				const Vec2 directedSeparatedImpulsePerInvWeight = collision.CollisionNormal * separatingImpulse;

				particleA->SetVelocity(velA + directedSeparatedImpulsePerInvWeight * inverseWeightA);
				particleB->SetVelocity(velB - directedSeparatedImpulsePerInvWeight * inverseWeightB);
			}

			// Resolving Interpenetration
			if (collision.Interpenetration > 0 && totalInverseWeight > 0) {
				const Vec2 posA = particleA->GetPosition();
				const Vec2 posB = particleB->GetPosition();

				const Vec2 separatingMovementPerInverseWeight = collision.CollisionNormal * (collision.Interpenetration / totalInverseWeight);

				particleA->SetPosition(posA + separatingMovementPerInverseWeight * inverseWeightA);
				particleB->SetPosition(posB - separatingMovementPerInverseWeight * inverseWeightB);
			}
		}
	}

	void World::FindAndResolveBoundsCollisions() {
		if (const AABB* boundsAABB = std::get_if<AABB>(&Bounds))
		{
			for (auto& particle : *this)
			{
				bool changed = false;
				Vec2 position = particle.GetPosition();
				Vec2 velocity = particle.GetVelocity();
				Real rebound = 0.9;

				Vec2 halfSize{0};
				AABB particleAABB = AABB::FromCenterHalfSize(position, halfSize);

				if (const Circle* cirlce = std::get_if<Circle>(&particle.m_Shape)) {
					halfSize = Vec2{cirlce->Radius};
					particleAABB = AABB::FromCenterHalfSize(position, halfSize);
				}

				if (particleAABB.Min.x <= boundsAABB->Min.x) {
					position.x = boundsAABB->Min.x + halfSize.x;
					if(velocity.x < 0) velocity.x *= -rebound;
					changed = true;
				} else if (particleAABB.Max.x >= boundsAABB->Max.x) {
					position.x = boundsAABB->Max.x - halfSize.x;
					if(velocity.x > 0) velocity.x *= -rebound;
					changed = true;
				}

				if (particleAABB.Min.y <= boundsAABB->Min.y) {
					position.y = boundsAABB->Min.y + halfSize.y;
					if(velocity.y < 0) velocity.y *= -rebound;
					changed = true;
				} else if (particleAABB.Max.y >= boundsAABB->Max.y) {
					position.y = boundsAABB->Max.y - halfSize.y;
					if(velocity.y > 0) velocity.y *= -rebound;
					changed = true;
				}

				if (changed) {
					particle.SetPosition(position);
					particle.SetVelocity(velocity);
				}
			}
		}
	}

	void World::Integrate(Real stepTime) {
		for (auto& particle : *this)
		{
			particle.SetPosition(particle.GetPosition() + particle.m_Velocity * stepTime);
			particle.SetVelocity(particle.GetVelocity() + particle.m_ConstantAccelerations * stepTime + particle.m_SummedAccelerations * stepTime);
			particle.m_SummedAccelerations = Vec2{};
		}
	}

	void World::Step(Real stepTime)
	{
		// Integration
		Integrate(stepTime);

		// Collision Detection
		FindParticlesCollisions();
		for (int iterations = 0; iterations < 10; ++iterations)
		{
			ResolveParticleCollisions();
			FindAndResolveBoundsCollisions();
			FindParticlesCollisions();
			if (m_Collisions.size() == 0) break;
		}
	}
} // FYC