//
// Created by ianpo on 25/12/2024.
//

#pragma once

#include "Math.hpp"

namespace FYC {

	struct AABB {
		AABB() = default;
		~AABB() = default;
		AABB(const AABB&) = default;
		AABB& operator=(const AABB&) = default;
		AABB(const Vec2& min, const Vec2& max);

		[[nodiscard]] static AABB FromMinMax(const Vec2& min, const Vec2& max);
		[[nodiscard]] static AABB FromCenterSize(const Vec2& center, const Vec2& size);
		[[nodiscard]] static AABB FromCenterHalfSize(const Vec2& center, const Vec2& halfSize);

		[[nodiscard]] Vec2 GetSize() const;
		[[nodiscard]] Vec2 GetHalfSize() const;
		[[nodiscard]] Vec2 GetCenter() const;

		void Validate();

		Vec2 Min, Max;
	};

} // FYC
