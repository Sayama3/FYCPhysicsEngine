//
// Created by ianpo on 24/12/2024.
//

#pragma once

#include "Physics/Math.hpp"
#include "Physics/AABB.hpp"
#include "Physics/Particle.hpp"
#include "Physics/Collision.hpp"

namespace FYC {

	struct PairHasher {
	public:
		template <typename T, typename U>
		std::size_t operator()(const std::pair<T, U> &x) const
		{
			return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
		}
	};

	class World {
	public:
		using ID = uint64_t;
		using PairID = std::pair<ID, ID>;
		inline static constexpr ID NULL_ID = ~0ull;
		class WorldIterator {
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = Particle;
			using difference_type = std::ptrdiff_t;
			using pointer = Particle*;
			using reference = Particle&;
		public:
			WorldIterator();
			~WorldIterator();
			WorldIterator(World& world, ID particleId);
			WorldIterator(World* world, ID particleId);

			WorldIterator& operator++();
			WorldIterator operator++(int);

			bool operator==(const WorldIterator&) const;
			bool operator!=(const WorldIterator& other) const;

		public:
			reference operator*();
			const reference operator*() const;

			pointer operator->();
			const pointer operator->() const;
		public:
			[[nodiscard]] ID GetID() const { return m_ParticleId; }
			[[nodiscard]] World* GetWorld() const { return m_World; }
		private:
			World* m_World = nullptr;
			ID m_ParticleId = NULL_ID;
		};
	public:
		World();
		explicit World(uint64_t reserveParticleCount);
		~World();
		World(const World&) = default;
		World& operator=(const World&) = default;
	public:
		WorldIterator AddParticle();
		WorldIterator AddParticle(const Particle::Shape& shape);
		WorldIterator AddParticle(const Particle::Shape& shape, const Vec2& velocity);
		WorldIterator AddParticle(const Particle::Shape& shape, const Vec2& velocity, const Vec2& constantAcceleration);
		WorldIterator AddParticle(const Particle& particle);
		WorldIterator AddParticle(Particle&& particle);

		[[nodiscard]] Particle* GetParticle(ID id);
		[[nodiscard]] const Particle* GetParticle(ID id) const;

		void RemoveParticle(ID id);
	public:
		void Step(Real stepTime);
	public:
		[[nodiscard]] WorldIterator begin() {return WorldIterator{*this, m_Particles.empty() ? NULL_ID : m_Particles.begin()->first};}
		[[nodiscard]] WorldIterator end() {return WorldIterator{*this, NULL_ID};}
	private:
		std::unordered_map<ID, Particle> m_Particles;
		std::unordered_map<PairID, Collision, PairHasher> m_Collisions;
		ID m_IDGenerator{0ull};
	public:
		std::variant<std::monostate, AABB> Bounds;
	};

	static_assert(std::forward_iterator<World::WorldIterator>);
} // FYC
