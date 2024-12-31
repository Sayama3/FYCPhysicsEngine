//
// Created by ianpo on 31/12/2024.
//

#include "WorldSerializer.hpp"

namespace FYC::Application {

	// ========== SerializedParticle ==========
	std::pair<FYC::Particle, World::ID> SerializedParticle::ToParticle(const SerializedParticle& serialization)
	{
		Particle particle;
		particle.SetPosition(serialization.position);
		particle.SetVelocity(serialization.velocity);
		particle.AddConstantAcceleration(serialization.constantAccelerations);
		particle.SetRebound(serialization.rebound);
		particle.SetDrag(serialization.drag);
		switch (serialization.shapeType) {
			case CIRCLE:
				particle.SetCircleRadius(serialization.circle.Radius);
				break;
			case RECTANGLE:
				particle.SetRectangleSize(serialization.rectangle.GetSize());
				break;
		}
		particle.SetKinematic(serialization.isKinematic);
		particle.SetIsAwake(serialization.isAwake);
		particle.Data = serialization.color;
		return {particle, serialization.id};
	}

	SerializedParticle SerializedParticle::ToSerializedParticle(const FYC::Particle& particle, World::ID id)
	{
		SerializedParticle serialization;
		serialization.id = id;
		serialization.position = particle.GetPosition();
		serialization.velocity = particle.GetVelocity();
		serialization.constantAccelerations = particle.GetConstantAccelerations();
		serialization.rebound = particle.GetRebound();
		serialization.drag = particle.GetDrag();
		serialization.isKinematic = particle.IsKinematic();
		serialization.isAwake = particle.IsAwake();
		if (particle.HasShape<Circle>(serialization.circle)) serialization.shapeType = CIRCLE;
		if (particle.HasShape<AABB>(serialization.rectangle)) serialization.shapeType = RECTANGLE;
		else serialization.shapeType = CIRCLE;
		serialization.color = particle.Data.type() == typeid(Color) ? std::any_cast<Color>(particle.Data) : Color{255,255,255,255};
		return serialization;
	}

	static_assert(sizeof(char) == 1);
	std::vector<char> WorldSerializer::ToBinary(World world)
	{
		std::vector<char> rawBuffer(world.count() * sizeof(SerializedParticle) + sizeof(SerializedBounds));

		SerializedBounds* bounds = reinterpret_cast<SerializedBounds*>(rawBuffer.data());
		bounds->hasBounds = false;
		if (AABB* b = std::get_if<AABB>(&world.Bounds)) {
			bounds->hasBounds = true;
			bounds->bounds = *b;
		}

		SerializedParticle* buffer = reinterpret_cast<SerializedParticle*>(rawBuffer.data() + sizeof(SerializedBounds));
		uint64_t index{0};
		for (auto it = world.begin(); it != world.end(); ++it) {
			buffer[index++] = SerializedParticle::ToSerializedParticle(*it, it.GetID());
		}

		return std::move(rawBuffer);
	}

	World WorldSerializer::FromBinary(const std::vector<char> &binary) {
		World world;
		const uint64_t count = (binary.size() - sizeof(SerializedBounds)) / sizeof(SerializedParticle);

		const SerializedBounds* serializedBounds = reinterpret_cast<const SerializedBounds*>(binary.data());
		if (serializedBounds->hasBounds) {
			world.Bounds = serializedBounds->bounds;
		}

		const SerializedParticle* buffer = reinterpret_cast<const SerializedParticle*>(binary.data() + sizeof(SerializedBounds));
		uint64_t cursor{0};
		for (int i = 0; i < count; ++i) {
			auto&& [particle, id] = SerializedParticle::ToParticle(buffer[i]);
			world.SetParticle(std::move(particle), id);
		}

		return std::move(world);
	}
}
