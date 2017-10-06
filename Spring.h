#pragma once

#include "Particle.h"

class Spring
{
public:
	Particle* p1 = nullptr;
	Particle* p2 = nullptr;
	float ks = 20.0f;
	float kd = 1.5f;
	float restingLength = 1.0f;

public:
	Spring()
	{

	}

	Spring(Particle* p1, Particle* p2, float restDist)
	{
		Spring::p1 = p1;
		Spring::p2 = p2;
		restDist = restingLength;
	}

	void applySpringForce()
	{
		if (p1 != nullptr && p2 != nullptr)
		{
			// Calculate distance between the two particles
			vmath::vec3 dist = p2->pos - p1->pos;
			float length = vmath::length(dist);
			// Normalize the distance for direciton
			vmath::vec3 nDist = vmath::normalize(dist);

			// Get the velocity along the direction
			float v1 = vmath::dot(p1->velocity, nDist);
			float v2 = vmath::dot(p2->velocity, nDist);

			// Calculate the force of the spring
			// -ks * (restingLength - actualLength) - kd * (v1 - v2)
			float fSD = -ks * (restingLength - length) - kd * (v1 - v2);
			vmath::vec3 force = fSD * nDist;

			p1->applyForce(force);
			p2->applyForce(-force);
		}
	}
};