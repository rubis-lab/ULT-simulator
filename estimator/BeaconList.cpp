#include "BeaconList.h"


BeaconList::BeaconList()
{
	Clear();
}

BeaconList::~BeaconList()
{
	for (size_t i; i < baseBeaconList.size(); i++)
		delete baseBeaconList[i];
}

void BeaconList::Clear()
{
	beacons.clear();
	baseBeaconList.clear();
	planeList.clear();
}

void BeaconList::AddBeacon(int bid, Vector location)
{
	Beacon beacon = new Beacon(bid, location);
	AddBeacon(beacon);
}

void BeaconList::AddBeacon(Beacon *beacon)
{
	baseBeaconList.push_back(beacon);
}

void BeaconList::ApplyPlanes(PlaneList *planeList, int maxReflectionCount)
{
	this->planeList = planeList;
	this->maxReflectionCount = maxReflectionCount;

	for (int i = 0; i < maxReflectionCount; i++)
	{
		baseBeaconList
	}
	
}

void BeaconList::Setup()
{
	beacon.clear();
	beacon = baseBeaconList;
}


void 
