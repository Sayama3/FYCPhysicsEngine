//
// Created by ianpo on 09/12/2024.
//

#pragma once

#include <raylib.h>
#include "Physics/Math.hpp"

namespace FYC::Application {
	class Camera
	{
	public:
		Camera(Real viewportWidth = 800, Real viewportHeight = 450);
		Camera(Real viewportWidth, Real viewportHeight, Real zoom);
		~Camera() = default;
	public:
		void Move(const Vec2& movement);
		void MultiplyZoom(Real multiplicator);

		void SetZoom(Real zoom);
		void SetPosition(const Vec2& position);
	public:
		[[nodiscard]] const Camera2D& GetRaylibCamera() const;
	private:
		Camera2D m_Camera;
	};
}
