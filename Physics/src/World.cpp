//
// Created by ianpo on 24/12/2024.
//

#include "Physics/World.hpp"

using namespace FYC::Literal;

namespace FYC {

	static constexpr Real NumberOfFrameToRemove{2.5};

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
		static_assert(std::is_same<Particle::Shape, std::variant<Circle, AABB>>());
		m_Collisions.clear();
		for (auto it = begin(); it != end(); ++it) {
			auto nextIt = it;
			++nextIt;
			for ( ;nextIt != end(); ++nextIt) {
				Collision collision;
				auto a = it.GetID() < nextIt.GetID() ? it : nextIt;
				auto b = it.GetID() < nextIt.GetID() ? nextIt : it;

				if (!a->IsKinematic() && !b->IsKinematic()) continue;

				{
					Circle ca, cb;
					AABB ra, rb;
					if (a->HasShape<Circle>(ca) && b->HasShape<Circle>(cb)) collision = CollisionDetector::Collide(ca,cb);
					else if (a->HasShape<AABB>(ra) && b->HasShape<AABB>(rb)) collision = CollisionDetector::Collide(ra,rb);
					else if (a->HasShape<AABB>(ra) && b->HasShape<Circle>(cb)) collision = CollisionDetector::Collide(ra,cb);
					else if (a->HasShape<Circle>(ca) && b->HasShape<AABB>(rb)) collision = CollisionDetector::Collide(ca,rb);
				}

				if (collision) {
					m_Collisions[{a.GetID(), b.GetID()}] = collision;
				}
			}
		}
	}

	void World::ResolveParticleCollisions(Real stepTime) {
		for (const auto& [pair, collision] : m_Collisions)
		{
			Particle* particleA = GetParticle(pair.first);
			Particle* particleB = GetParticle(pair.second);

			if (!particleA || !particleB) continue;

			const Real inverseMassA = particleA->GetInverseMass();
			const Real inverseMassB = particleB->GetInverseMass();
			const Real totalInverseMass = inverseMassA + inverseMassB;

			if (totalInverseMass <= REAL_EPSILON) continue;

			const Real reboundA = particleA->GetRebound();
			const Real reboundB = particleB->GetRebound();

			const Vec2 velA = particleA->GetVelocity();
			const Vec2 velB = particleB->GetVelocity();
			const Real contactVelocity = Math::Dot(collision.CollisionNormal, velA - velB);

			const Vec2 accA = particleA->GetConstantAccelerations();
			const Vec2 accB = particleB->GetConstantAccelerations();
			Real accCausedSepVelocity = (Math::Dot(collision.CollisionNormal, accA - accB)) * stepTime * NumberOfFrameToRemove;
			if (accCausedSepVelocity > 0) {
				accCausedSepVelocity = 0;
			}

			// Resolving velocity
			if (contactVelocity <= 0) {
				Real separatingVelocity = -contactVelocity + accCausedSepVelocity;
				if(separatingVelocity < 0) separatingVelocity = 0;

				const Real deltaVelocityA = separatingVelocity * reboundA - contactVelocity;
				const Real separatingImpulseA = deltaVelocityA / totalInverseMass;
				const Vec2 directedSeparatedImpulseA = collision.CollisionNormal * separatingImpulseA;

				const Real deltaVelocityB = separatingVelocity * reboundB - contactVelocity;
				const Real separatingImpulseB = deltaVelocityB / totalInverseMass;
				const Vec2 directedSeparatedImpulseB = collision.CollisionNormal * separatingImpulseB;

				particleA->SetVelocity(velA + directedSeparatedImpulseA * inverseMassA);
				particleB->SetVelocity(velB - directedSeparatedImpulseB * inverseMassB);
			}

			// Resolving Interpenetration
			if (collision.Interpenetration > 0) {
				const Vec2 posA = particleA->GetPosition();
				const Vec2 posB = particleB->GetPosition();

				const Vec2 separatingMovement = collision.CollisionNormal * (collision.Interpenetration / totalInverseMass);

				particleA->SetPosition(posA + separatingMovement * inverseMassA);
				particleB->SetPosition(posB - separatingMovement * inverseMassB);
			}
		}
	}

	void World::FindAndResolveBoundsCollisions(Real stepTime) {
		static_assert(std::is_same<Particle::Shape, std::variant<Circle, AABB>>());

		if (const AABB* boundsAABB = std::get_if<AABB>(&Bounds))
		{
			for (auto& particle : *this)
			{
				if (!particle.IsKinematic()) continue;

				bool changed = false;
				Vec2 position = particle.GetPosition();
				const Vec2 velocity = particle.GetVelocity();
				const Real rebound = particle.GetRebound();

				Vec2 impulse{};

				Vec2 halfSize{0};
				AABB particleAABB = AABB::FromCenterHalfSize(position, halfSize);

				Vec2 contactNormal{};

				if (const Circle* circle = std::get_if<Circle>(&particle.m_Shape)) {
					halfSize = Vec2{circle->Radius};
					particleAABB = AABB::FromCenterHalfSize(position, halfSize);
				} else if (const AABB* aabb = std::get_if<AABB>(&particle.m_Shape)) {
					halfSize = aabb->GetHalfSize();
					particleAABB = *aabb;
				}

				if (particleAABB.Min.x < boundsAABB->Min.x) {
					position.x = boundsAABB->Min.x + halfSize.x;
					if(velocity.x < 0) impulse += {-(velocity.x), 0};
					contactNormal += {1,0};
					changed = true;
				} else if (particleAABB.Max.x > boundsAABB->Max.x) {
					position.x = boundsAABB->Max.x - halfSize.x;
					if(velocity.x > 0) impulse += {-(velocity.x), 0};;
					contactNormal += {-1,0};
					changed = true;
				}

				if (particleAABB.Min.y < boundsAABB->Min.y) {
					position.y = boundsAABB->Min.y + halfSize.y;
					if(velocity.y < 0) impulse += {0, -(velocity.y)};;
					contactNormal += {0, 1};
					changed = true;
				} else if (particleAABB.Max.y > boundsAABB->Max.y) {
					position.y = boundsAABB->Max.y - halfSize.y;
					if(velocity.y > 0) impulse += {0, -(velocity.y)};;
					contactNormal += {0, -1};
					changed = true;
				}

				if (changed) {
					particle.SetPosition(position);

					if (impulse.x != 0 || impulse.y != 0)
					{
						Math::NormalizeInPlace(contactNormal);
						const Vec2 contactVelocity = contactNormal * Math::Dot(contactNormal, velocity);
						const Real accCausedSepVelocity = (Math::Dot(contactNormal, particle.GetConstantAccelerations())) * stepTime * NumberOfFrameToRemove;
						Real impulseValue = Math::Dot(contactNormal, impulse);
						if (accCausedSepVelocity < 0) {
							impulseValue += accCausedSepVelocity;
							if (impulseValue < 0) impulseValue = 0;
						}
						particle.SetVelocity(velocity - contactVelocity + contactNormal * (impulseValue * rebound));
					}
				}
			}
		}
	}

	void World::Integrate(Real stepTime) {
		for (auto& particle : *this)
		{
			if (!particle.IsKinematic()) continue;
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
			ResolveParticleCollisions(stepTime);
			FindAndResolveBoundsCollisions(stepTime);
			FindParticlesCollisions();
			if (m_Collisions.size() == 0) break;
		}

		for (auto& particle : *this) {
			if (!particle.IsKinematic()) continue;
			particle.SetVelocity(particle.GetVelocity() * particle.GetDrag());
		}
	}
} // FYC