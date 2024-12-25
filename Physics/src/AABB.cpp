//
// Created by ianpo on 25/12/2024.
//

#include "Physics/AABB.hpp"

using namespace FYC::Literal;

namespace FYC {
	AABB::AABB(const Vec2& min, const Vec2& max) : Min(min), Max(max)
	{
		Validate();
	}

	AABB AABB::FromMinMax(const Vec2 &min, const Vec2 &max)
	{
		return {min, max};
	}

	AABB AABB::FromCenterSize(const Vec2& center, const Vec2& size)
	{
		return AABB::FromCenterHalfSize(center, size * 0.5_r);
	}

	AABB AABB::FromCenterHalfSize(const Vec2& center, const Vec2& halfSize)
	{
		return AABB{center - halfSize, center + halfSize};
	}

	Vec2 AABB::GetSize() const
	{
		return Max - Min;
	}

	Vec2 AABB::GetHalfSize() const
	{
		return GetSize() * 0.5_r;
	}

	Vec2 AABB::GetCenter() const
	{
		return (Min + Max) * 0.5_r;
	}

	void AABB::Validate()
	{
		if (Max.x < Min.x) std::swap(Max.x, Min.x);
		if (Max.y < Min.y) std::swap(Max.y, Min.y);
	}
} // FYC