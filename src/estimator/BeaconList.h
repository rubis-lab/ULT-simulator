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

	void addBeacon(int userBid, Vector location);
	void applyPlanes(PlaneList *planeList, int maxReflectionCount = REFLECTION_COUNT_LIMIT);
	void reset();
	size_t size();
	Beacon* at(int idx);
	Beacon* findByUserBid(int userBid);
	void load(const char *filename);
	void save(const char *filename);
	void modifyAndSave(const char *filename, int userBid, Vector location);
	void deleteAndSave(const char *filename, int userBid);

public:
//	std::vector<Beacon*> beacons;

private:
	std::vector<Beacon*> baseBeaconList;
//	std::vector<BeaconIterator> beaconIterators;
	
	PlaneList *planeList;
	int maxReflectionCount;
	void applyPlanes(Beacon* beacon);
	void addBeacon(Beacon *beacon);
};
