#pragma once
#include <vector>
#include "Vector.h"
#include "Plane.h"


// one beacon can have several children beacons according to the reflectable planes.
// thus beacons consists tree, the depth of it means reflection count.
// depth one is for non-reflect beacons which reflectionCount is 0.
// AddReflectionBeacon find reflected Beacon, and then make child node

class Beacon
{
public:
	Beacon(int bid, Vector location);
	Beacon (Beacon *beacon);
	~Beacon();

	void addReflectedBeacon(Plane *plane);
	Vector getLocation();
	size_t size();
	Beacon* at(size_t idx);

	
private:
	int bid;
	int reflectionCount;
	std::vector<Vector> vLocations;			// track reflected location of beacons
	std::vector<Plane*> pPlanes;			// track which planes are reflected to
	Beacon *parent;
	std::vector<Beacon *> children;
	
	void addChild(Beacon *beacon);
	Beacon* newReflectedBeacon(Plane *plane);
};
		

