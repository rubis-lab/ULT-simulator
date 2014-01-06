#include "BeaconList.h"

#define MIN_SEP 10


BeaconList::BeaconList()
{
	baseBeaconList.clear();
}

BeaconList::~BeaconList()
{
	reset();
}

void BeaconList::reset()
{
	for (size_t i = 0; i < baseBeaconList.size(); i++)
		delete baseBeaconList[i];
	baseBeaconList.clear();
	
}

void BeaconList::addBeacon(int userBid, Vector location)
{
	int bid = baseBeaconList.size();
	Beacon *beacon = new Beacon(bid, location);
	beacon->setUserBid(userBid);
	addBeacon(beacon);
}

void BeaconList::addBeacon(Beacon *beacon)
{
	baseBeaconList.push_back(beacon);
}

void BeaconList::applyPlanes(PlaneList *planeList, int maxReflectionCount)
{
	this->planeList = planeList;
	this->maxReflectionCount = maxReflectionCount;

	for (size_t i = 0; i < baseBeaconList.size(); i++)
	{
		applyPlanes(baseBeaconList[i]);

		// after planes are added, make iterators
		baseBeaconList[i]->setIterator(false/*isDFS*/);
	}

	
	
}

void BeaconList::applyPlanes(Beacon *beacon)
{
	if (beacon->getReflectionCount() == maxReflectionCount) return;

	for (size_t i = 0; i < planeList->size(); i++)
	{
		int pid = beacon->getLastPlaneId();

		// signal can not be reflected on same plane twice
		if (pid >= 0 && pid == planeList->at(i)->getPid()) 
			continue;

		// if beacon is too close to plane, do not reflect
		if (planeList->at(i)->getDistanceToPoint(beacon->getLocation()) < MIN_SEP)
			continue;

		beacon->addReflectedBeacon(planeList->at(i));
	}

	for (size_t i = 0; i < beacon->childrenSize(); i++)
	{
		applyPlanes(beacon->childAt(i));
	}
}


/*
void BeaconList::makeIterators()
{
	for (size_t i = 0; i < baseBeaconList.size(); i++)
	{
		BeaconIterator *iter = baseBeaconList[i]->getIterator();

		iter->makeIndex(baseBeaconList[i], false);

		beaconIterators.push_back(iter);
		baseBeaconList[i]->setIterator(&beaconIterators[i]);
	}
}

void BeaconList::setup()
{
	beacons = baseBeaconList;
	for (size_t i = 0; i < beaconIterators.size(); i++)
	{
		beacons[i] = beaconIterators.reset();
	}
}


bool BeaconList::moveNext(int bid)
{
}
*/

size_t BeaconList::size()
{
	return baseBeaconList.size();
}

Beacon* BeaconList::at(int idx)
{
	return baseBeaconList[idx];
}
