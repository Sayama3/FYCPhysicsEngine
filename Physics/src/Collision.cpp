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
		const Vec2 maxSize = a.GetHalfSize() + b.GetHalfSize();
		const Vec2 aToB = b.GetCenter() - a.GetCenter();
		const Vec2 absAToB = {std::abs(aToB.x), std::abs(aToB.y)};

		if (absAToB.x > maxSize.x || absAToB.y > maxSize.y) return {{0,0}, {0,0}, 0, false};

		Vec2 distanceToMove = maxSize - absAToB;
		Real interpenetration = std::min(distanceToMove.x, distanceToMove.y);
		Vec2 point = a.GetCenter();
		Vec2 normal = {};

		if (distanceToMove.x < distanceToMove.y) {
			normal = {-Math::Sign(aToB.x), 0};
			point.x += aToB.x - (interpenetration * 0.5);
		} else {
			normal = {0, -Math::Sign(aToB.y)};
			point.y += aToB.y - (interpenetration * 0.5);
		}

		return {point, normal, interpenetration, true};
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