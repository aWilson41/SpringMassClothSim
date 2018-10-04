#include "Spring.h"
#include "Particle.h"

Spring::Spring(Particle* p1, Particle* p2, float restDist)
{
	Spring::p1 = p1;
	p1->springs.push_back(this);
	Spring::p2 = p2;
	p2->springs.push_back(this);
	restingLength = restDist;

	if (p1 == nullptr || p2 == nullptr)
		printf("shit\n");
}

void Spring::applySpringForce()
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