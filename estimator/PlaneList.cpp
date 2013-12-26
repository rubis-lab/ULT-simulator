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

	planes.push_back(newPlane);
}
