//
// Created by ianpo on 26/12/2024.
//

#include "../include/Physics/Collision.hpp"

namespace FYC {
	Collision Collider::Collide(const Circle &a, const Circle &b)
	{
		const Real sumRadii = a.Radius + b.Radius;
		const Vec2 aToB = b.Position - a.Position;
		const Real lenAToB = Math::Magnitude(aToB);
		if (lenAToB > sumRadii) return {false, {0,0}, {0,0}, 0};

		if (lenAToB <= REAL_EPSILON) {
			return {true, a.Position, {0,1}, sumRadii};
		}

		const Vec2 collisionNormal = -(aToB / lenAToB);
		return {true, a.Position + aToB + (collisionNormal * (sumRadii * 0.5f)), collisionNormal, sumRadii - lenAToB};
	}
} // FYC