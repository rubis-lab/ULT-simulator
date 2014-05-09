#include <stdlib.h>
#include <stdio.h>
#include "PlaneList.h"

PlaneList::PlaneList()
{
	constructor();
}

PlaneList::~PlaneList()
{
	destructor();
}

void PlaneList::reset()
{
	destructor();
	constructor();
}

void PlaneList::constructor()
{
}

void PlaneList::destructor()
{
	for (size_t i = 0; i < planes.size(); i++)
	{
		delete planes[i];
	}
	planes.clear();
}

size_t PlaneList::size()
{
	return planes.size();
}

Plane* PlaneList::at(int idx)
{
	return planes[idx];
}

void PlaneList::addPlane(Plane plane)
{
	Plane *newPlane = new Plane(plane);
	newPlane->setId(planes.size());

	planes.push_back(newPlane);
}

bool PlaneList::checkInside(Vector location)
{
	for (size_t i = 0; i < planes.size(); i++)
	{
		if (!planes[i]->isBoundary()) continue;
		if (planes[i]->getSign(location) > 0)
			return false;
	}
	return true;
}

void PlaneList::load(const char *filename)
{
	reset();

	FILE *fp = fopen(filename, "r");

	if (fp == NULL)
	{
		printf("can't open plane list file %s\n", filename);
		exit(20);
	}

	const int bufSize = 1024;
	char buf[bufSize + 1];

	while(fgets(buf, bufSize, fp) != NULL)
	{
		addPlane(Plane(buf));
	}
	fclose(fp);


}

void PlaneList::makeCube(const int width, const int length, const int height)
{
	reset();

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


	Plane p1 = Plane(v1, v2, v6, v5);		// front plane
	Plane p2 = Plane(v2, v4, v8, v6);		// right side plane
	Plane p3 = Plane(v4, v3, v7, v8);		// back plane
	Plane p4 = Plane(v3, v1, v5, v7);		// left plane
	Plane p5 = Plane(v3, v4, v2, v1);		// ceiling
	Plane p6 = Plane(v5, v6, v8, v7);		// floor

	p1.setBoundary(true);
	p2.setBoundary(true);
	p3.setBoundary(true);
	p4.setBoundary(true);
	p5.setBoundary(true);
	p6.setBoundary(true);
	
	
	addPlane(p1);
	addPlane(p2);
	addPlane(p3);
	addPlane(p4);
	addPlane(p5);
	addPlane(p6);
}

void PlaneList::save(const char *filename)
{
	FILE *fp = fopen(filename, "w");

	if (fp == NULL)
	{
		printf("PlaneList::save, can't open file %s\n", filename);
		exit(21);
	}

	for (size_t i = 0; i < planes.size(); i++)
	{
		Plane* plane = planes[i];
		char buf[1024];

		plane->getString(buf);

		if (plane->isBoundary())
			fprintf(fp, "O");

		fprintf(fp, "%s\n", buf);
	}
	fclose(fp);
}
