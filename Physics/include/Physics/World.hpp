//
// Created by ianpo on 24/12/2024.
//

#pragma once

#include "Physics/Math.hpp"
#include "Physics/AABB.hpp"
#include "Physics/Particle.hpp"

namespace FYC {

	class World {
	public:
		using ID = uint64_t;
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
	public:
		WorldIterator AddParticle();
		WorldIterator AddParticle(const Vec2& position);
		WorldIterator AddParticle(const Vec2& position, const Vec2& velocity);
		WorldIterator AddParticle(const Vec2& position, const Vec2& velocity, const Vec2& constantAcceleration);
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
		ID m_IDGenerator{0ull};
	public:
		std::variant<std::monostate, AABB> Bounds;
	};

	static_assert(std::forward_iterator<World::WorldIterator>);
} // FYC
