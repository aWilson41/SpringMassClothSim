#pragma once

class Particle;

class Face
{
public:
	Face() { }
	Face(Particle* p1, Particle* p2, Particle* p3)
	{
		Face::p1 = p1;
		Face::p2 = p2;
		Face::p3 = p3;
	}

public:
	Particle* p1 = nullptr;
	Particle* p2 = nullptr;
	Particle* p3 = nullptr;
};