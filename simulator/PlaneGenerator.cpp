#include "PlaneGenerator.h"


void PlaneGenerator::generatePlane(SimulatorArgument *args)
{
	this-> args = args;

	switch (args->planeTYpe)
	{
	case SIM_PLANE::CUBE: 
		generateCube(); 
		break;
	case SIM_PLANE::BYFILE: 
		args->planes.load(args->planeFilename); 
		break;
	}
}

void PlaneGenerator::generateCube()
{
	args->planes.reset();

	int width = args->width;
	int length = args->length;
	int height = args->height;

	/* 
	when making 8 points of cube (2 x 2 x 2), RH rules
		v1 = (-1,  1, 2)
		v2 = ( 1,  1, 2)
		v3 = (-1, -1, 2)
		v4 = ( 1, -1, 2)
		v5 = (-1,  1, 0)
		v6 = ( 1,  1, 0)
		v7 = (-1, -1, 0)
		v8 = ( 1, -1, 0)

		  1------2
		 /|     /|
		3------4 |
		| 5----|-6 
		|/     |/
		7------8
	*/

	double cx = width / 2.0;
	double cy = length / 2.0;
	double cz = height;

	Vector v1 = Vector(-cx,  cy, cz);
	Vector v2 = Vector( cx,  cy, cz);
	Vector v3 = Vector(-cx, -cy, cz);
	Vector v4 = Vector( cx, -cy, cz);
	Vector v5 = Vector(-cx,  cy, 0);
	Vector v6 = Vector( cx,  cy, 0);
	Vector v7 = Vector(-cx, -cy, 0);
	Vector v8 = Vector( cx, -cy, 0);


	args->planes.addPlane(Plane(v1, v2, v6, v5));		// front plane
	args->planes.addPlane(Plane(v2, v4, v8, v6));		// right side plane
	args->planes.addPlane(Plane(v4, v3, v7, v8));		// back plane
	args->planes.addPlane(Plane(v3, v1, v5, v7));		// left plane
	args->planes.addPlane(Plane(v3, v4, v2, v1));		// ceiling
	args->planes.addPlane(Plane(v5, v6, v8, v7));		// floor


	for (size_t i = 0; i < args->planes.size(); i++)
	{
		args->planes.at(i)->setBoundary(true);
	}

/*
	// original plane definition 
	Plane p1 = Plane(Vector(0, length / 2, 0), Vector(0, length / 2, height), Vector (width/ 2, length / 2, 0));
	Plane p2 = Plane(Vector(0, - length / 2, 0), Vector(0, - length /2, height), Vector (-width / 2, -length / 2, 0));
	Plane p3 = Plane(Vector(-width / 2, 0, 0), Vector (-width / 2, -length / 2 , 0), Vector(-width / 2, 0, height));
	Plane p4 = Plane(Vector(width / 2, 0, 0), Vector (width / 2, length /2 , 0), Vector(width / 2, 0, height));
	Plane p5 = Plane(Vector(0, 0, 0), Vector(0, length / 2, 0), Vector(width / 2, 0, 0));
	Plane p6 = Plane(Vector(0, 0, height), Vector(width / 2, 0, height), Vector(0, length / 2, height));
*/
/*
	// Jihyes's pentagon planes
	Plane p1 = Plane(Vector(0, length / 2, 0), Vector(0, length / 2, height), Vector (width/ 2, length / 2, 0));
	Plane p2 = Plane(Vector(0, - length / 2, 0), Vector(0, - length /2, height), Vector (-width / 2, -length / 2, 0));
	Plane p3 = Plane(Vector(-width / 2, 0, 0), Vector (-width / 2, -length / 2 , 0), Vector(-width / 2, 0, height));
	Plane p4 = Plane(Vector(width / 2, 0, 0), Vector (width / 2, length /2 , 0), Vector(width / 2, 0, height));
	Plane p5 = Plane(Vector(0, 0, 0), Vector(0, length / 2, 0), Vector(width / 2, 0, 0));
	Plane p6 = Plane(Vector(0, 0, height), Vector(width / 2, 0, height), Vector(0, length / 2, height));
	SetPlane(Vector(width / 6, length / 2, 0), Vector(width / 6, length / 2, height), Vector(width / 2, length / 6, 0));
*/

}
