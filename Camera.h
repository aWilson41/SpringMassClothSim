#pragma once

#include "vmath.h"

class Camera
{
public:
	Camera();

	void reset();

	void setPerspective(float fov, float aspectRatio, float nearZ, float farZ)
	{
		Camera::fov = fov;
		Camera::aspectRatio = aspectRatio;
		Camera::nearZ = nearZ;
		Camera::farZ = farZ;
		proj = vmath::perspective(fov, aspectRatio, nearZ, farZ);
	}
	void setEyePos(vmath::vec3 pos) { eyePos = pos; }
	void setEyePos(float x, float y, float z) { setEyePos(vmath::vec3(x, y, z)); }

	void setFocalPt(vmath::vec3 pt)
	{
		focalPt = pt;
		view = vmath::lookat(eyePos, focalPt, up);
	}
	void setFocalPt(float x, float y, float z) { setFocalPt(vmath::vec3(x, y, z)); }

public:
	// Camera view matrix
	vmath::mat4 view;
	// Camera projection matrix
	vmath::mat4 proj;
	// Eye position
	vmath::vec3 eyePos;
	// Focal point
	vmath::vec3 focalPt;
	// Up
	vmath::vec3 up;
	// Camera specs
	float fov;
	float aspectRatio;
	float nearZ;
	float farZ;
};