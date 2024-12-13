//
// Created by ianpo on 09/12/2024.
//

#include "Camera.hpp"

namespace FYC::Application {

	static constexpr Real c_CenterOffsetMultiplier{0.5};
	static constexpr Real c_DefaultZoomMultiplier{0.2};

	Camera::Camera(Real viewportWidth, Real viewportHeight)
	: m_Camera({{static_cast<float>(viewportWidth)*c_CenterOffsetMultiplier,static_cast<float>(viewportHeight)*c_CenterOffsetMultiplier},{0,0}, 0, static_cast<float>(viewportHeight) * c_DefaultZoomMultiplier})
	{
	}

	Camera::Camera(Real viewportWidth, Real viewportHeight, Real zoom)
	: m_Camera({{static_cast<float>(viewportWidth)*c_CenterOffsetMultiplier,static_cast<float>(viewportHeight)*c_CenterOffsetMultiplier},{0,0}, 0, zoom})
	{
	}

	void Camera::Move(const Vec2& movement)
	{
		m_Camera.target.x += movement.x;
		m_Camera.target.y += movement.y;
	}

	void Camera::SetZoom(Real zoom)
	{
		m_Camera.zoom = zoom;
	}

	void Camera::MultiplyZoom(Real multiplicator)
	{
		m_Camera.zoom *= multiplicator;
	}

	void Camera::SetPosition(const Vec2 &position)
	{
		m_Camera.target.x = position.x;
		m_Camera.target.y = position.y;
	}

	const Camera2D & Camera::GetRaylibCamera() const
	{
		return m_Camera;
	}
}
