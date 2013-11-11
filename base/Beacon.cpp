#include "Beacon.h"

Beacon::Beacon(int bid, Vector vLocation) : bid(bid)
{
	reflectionCount = 0;
	vLocations.clear();
	vLocations.push_back(vLocation);
	pPlanes.clear();
	children.clear();
	parent = NULL;
}
Beacon::Beacon(Beacon *beacon)
{
	this->bid = beacon->bid;
	this->reflectionCount = beacon->reflectionCount;
	this->vLocations = beacon->vLocations;
	this->pPlanes = beacon->pPlanes;
	this->parent = beacon->parent;
	this->children = beacon->children;

}
Beacon::~Beacon()
{
	for (size_t i = 0; i < children.size(); i++)
		delete children[i];
}

void Beacon::addReflectedBeacon(Plane *plane)
{
	Beacon *newBeacon;
	
	newBeacon = newReflectedBeacon(plane);
	addChild(newBeacon);
}

Vector Beacon::getLocation()
{
	// the last location is finally reflected location
	return vLocations.back();
}

void Beacon::addChild(Beacon* child)
{
	children.push_back(child);
	child->parent = this;
}

Beacon* Beacon::newReflectedBeacon(Plane *plane)
{
	Beacon *newBeacon = new Beacon(this);
	Vector vReflected = plane->getReflectedPoint(vLocations.back());

	newBeacon->reflectionCount ++;
	newBeacon->vLocations.push_back(vReflected);
	newBeacon->pPlanes.push_back(plane);
	
	return newBeacon;
}

size_t Beacon::size()
{
	return children.size();
}

Beacon* Beacon::at(size_t idx)
{
	if (idx >= children.size()) return NULL;
	return children[idx];
}
