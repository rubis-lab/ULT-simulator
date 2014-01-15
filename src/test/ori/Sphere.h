#pragma once
#include "Vector.h"
class Sphere
{
public:
	Sphere(Vector center_, double radius_);
	~Sphere(void);

	Vector center;
	double radius;

	bool CheckInclusion(Vector point);
	bool CheckCollision(Vector p1, Vector p2);
};

