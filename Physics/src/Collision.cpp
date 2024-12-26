//
// Created by ianpo on 26/12/2024.
//

#include "Physics/Collision.hpp"

namespace FYC {
	Collision CollisionDetector::Collide(const Circle &a, const Circle &b)
	{
		const Real sumRadii = a.Radius + b.Radius;
		const Vec2 aToB = b.Position - a.Position;
		const Real lenAToB = Math::Magnitude(aToB);

		if (lenAToB > sumRadii) return {{0,0}, {0,0}, 0, false};

		if (lenAToB <= REAL_EPSILON) {
			return {a.Position, {0,1}, sumRadii, true};
		}

		const Vec2 collisionNormal = -(aToB / lenAToB);
		return {a.Position + aToB + (collisionNormal * (sumRadii * 0.5f)), collisionNormal, sumRadii - lenAToB, true};
	}

	Collision CollisionDetector::Collide(const AABB &a, const AABB &b) {
		return {{0,0}, {0,0}, 0, false};
	}

	Collision CollisionDetector::Collide(const Circle &a, const AABB &b) {
		return {{0,0}, {0,0}, 0, false};
	}

	Collision CollisionDetector::Collide(const AABB &a, const Circle &b) {
		Collision invCol = Collide(b,a);
		invCol.CollisionNormal *= -1;
		return invCol;
	}
} // FYC