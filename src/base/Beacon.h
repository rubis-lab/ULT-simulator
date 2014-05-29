#pragma once
#include <vector>
#include "Vector.h"
#include "Plane.h"


// one beacon can have several children beacons according to the reflectable planes.
// thus beacons consists tree, the depth of it means reflection count.
// depth one is for non-reflect beacons which reflectionCount is 0.
// AddReflectionBeacon find reflected Beacon, and then make child node

class BeaconIterator;
class Measurement;



class Beacon
{
public:
	Beacon(int bid, Vector location);
	Beacon(Beacon *beacon);
	~Beacon();

	void addReflectedBeacon(Plane *plane);
	Vector getLocation();
	size_t childrenSize();
	Beacon* childAt(size_t idx);
	int getReflectionCount();
	int getLastPlaneId();
	void setUserBid(int bid);
	int getUserBid();
	int getBid();
	BeaconIterator* getIterator();
//	void setIterator(BeaconIterator* iterator);
	void setIterator(bool isDFS = true);
	Beacon* getRoot();
	Beacon* next();
	double getDistanceToPlane();
	
	std::vector<Vector> vLocations;			// track reflected location of beacons
	std::vector<Plane*> pPlanes;			// track which planes are reflected to

//	void bindMeasurementContainer(Measurement *measurement);
//	Measurement *getMeasurement();
		
private:
	int bid;								// beacon id allocated by system
	int userBid;
	double distanceToPlane;

	int reflectionCount;
	Beacon *parent;
	Beacon *root;
	std::vector<Beacon *> children;
	
	void addChild(Beacon *beacon);
	Beacon* newReflectedBeacon(Plane *plane);
	BeaconIterator* iterator;
	Measurement* measurement;

};
		

class BeaconIterator
{
public:

	BeaconIterator() {}
	~BeaconIterator() {}
	void makeIndex(Beacon* beacon, bool isDFS = true, bool isFirst = true);
	Beacon* reset();
	Beacon* next();
	Beacon* current();

private:
	int curIdx;
	std::vector<Beacon*> beaconIndex;

};
