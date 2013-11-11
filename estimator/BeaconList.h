#pragma once

#include <vector>
#include "Beacon.h"
#include "Plane.h"
#include "PlaneList.h"

#define REFLECTION_COUNT_LIMIT 1

class BeaconList
{
public:
	BeaconList();
	~BeaconList();

	void AddBeacon(int bie, Vector location);
	void AddBeacon(Beacon *beacon);
	void ApplyPlanes(PlaneList *planeList, int maxReflectionCount = REFLECTION_COUNT_LIMIT);
	void Clear();
	void Setup();
	bool 

public:
	std::vector<Beacon*> beacons;

private:
	std::vector<Beacon*> baseBeaconList;
	PlaneList *planeList;
	int maxReflectionCount;
};
