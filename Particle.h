#pragma once

#include "vmath.h"

class Particle
{
public:
	Particle()
	{
		pos = vmath::vec3(0.0f);
	}

	Particle(vmath::vec3 pos)
	{
		Particle::pos = pos;
	}

public:
	void integrate(float dt)
	{
		integrateAccel(dt);
		integrateVelocity(dt);
		force = vmath::vec3(0.0f);
	}

	void integrateAccel(float dt)
	{
		velocity += force * dt;
	}

	void integrateVelocity(float dt)
	{
		pos += velocity * dt;
	}

	void applyForce(vmath::vec3 force)
	{
		if (isForceApplied)
			Particle::force += force;
	}

public:
	bool isForceApplied = true;
	vmath::vec3 pos = vmath::vec3(0.0f);
	vmath::vec3 force = vmath::vec3(0.0f);
	vmath::vec3 velocity = vmath::vec3(0.0f);
};