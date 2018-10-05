#include "Face.h"
#include "Particle.h"
#include "Spring.h"

void Particle::integrate(float dt)
{
	if (!isFixed)
	{
		integrateAccel(dt);
		integrateVelocity(dt);
		force = vmath::vec3(0.0f);
	}
}

void Particle::updateNormal()
{
	vmath::vec3 sum = vmath::vec3(0.0f);
	// For every face
	for (int i = 0; i < faces.size(); i++)
	{
		Face& f = *faces[i];
		sum += vmath::normalize(vmath::cross(f.p2->pos - f.p1->pos, f.p3->pos - f.p1->pos));
	}
	normal = vmath::normalize(sum);
}