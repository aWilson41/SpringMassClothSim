#pragma once

class Particle;

class Spring
{
public:
	Spring() { }
	Spring(Particle* p1, Particle* p2, float restDist);

	void applySpringForce();

	Particle* getParticle(Particle* p) { return p == p1 ? p2 : p1; }

public:
	Particle* p1 = nullptr;
	Particle* p2 = nullptr;
	float ks = 40.0f;
	float kd = 0.5f;
	float restingLength = 1.0f;
};