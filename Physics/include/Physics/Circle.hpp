//
// Created by ianpo on 25/12/2024.
//

#pragma once

#include "Math.hpp"

namespace FYC {
	struct Circle {
		Circle() = default;
		~Circle() = default;
		Circle(const Vec2& position, Real radius);
		Vec2 Position;
		Real Radius;
	};
} // FYC
