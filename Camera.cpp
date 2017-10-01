#include "Camera.h"

Camera::Camera()
{
	reset();
}

void Camera::reset()
{
	fov = 0.0f;
	eyePos = vmath::vec3(0.0f);
	up = vmath::vec3(0.0f, 1.0f, 0.0f);
	view = vmath::mat4::identity();

	// 1, 0, 0, 0
	// 0, 1, 0, 0
	// 0, 0, 1, 0
	// 0, 0, 1, 0 default proj matrix. Causes a z divide
	proj = vmath::mat4::identity();
	proj[2][3] = 1.0f;
	proj[3][3] = 0.0f;
}