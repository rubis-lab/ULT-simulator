#include "Beacon.h"

Beacon::Beacon(int bid, Vector vLocation) : bid(bid)
{
	reflectionCount = 0;
	distanceToPlane = 0;
	vLocations.clear();
	vLocations.push_back(vLocation);
	pPlanes.clear();
	children.clear();
	parent = NULL;
	iterator = new BeaconIterator();
}
Beacon::Beacon(Beacon *beacon) : bid(beacon->bid)
{
	this->reflectionCount = beacon->reflectionCount;
	this->vLocations = beacon->vLocations;
	this->pPlanes = beacon->pPlanes;
	this->parent = beacon;
//	this->children = beacon->children;
	this->iterator = beacon->iterator;
	this->userBid = beacon->userBid;
	this->distanceToPlane = beacon->distanceToPlane;

}
Beacon::~Beacon()
{
	if (parent == NULL)
		delete iterator;
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
}

Beacon* Beacon::newReflectedBeacon(Plane *plane)
{
	Beacon *newBeacon = new Beacon(this);
	Vector vReflected = plane->getReflectedPoint(vLocations.back());

	newBeacon->reflectionCount ++;
	newBeacon->vLocations.push_back(vReflected);
	newBeacon->pPlanes.push_back(plane);
	newBeacon->distanceToPlane += plane->getDistanceToPoint(vReflected);
	
	return newBeacon;
}


size_t Beacon::childrenSize()
{
	return children.size();
}

Beacon* Beacon::childAt(size_t idx)
{
	if (idx >= children.size()) return NULL;
	return children[idx];
}

double Beacon::getDistanceToPlane()
{
	return distanceToPlane;
}

int Beacon::getReflectionCount()
{
	return reflectionCount;
}

int Beacon::getLastPlaneId()
{
	if (pPlanes.size() == 0) return -1;
	return pPlanes.back()->getPid();
}

void Beacon::setUserBid(int bid)
{
	userBid = bid;
}

int Beacon::getUserBid()
{
	return userBid;
}

int Beacon::getBid()
{
	return bid;
}

Beacon* Beacon::getRoot()
{
	if (parent == NULL) return this;
	return parent->getRoot();
}

void Beacon::setIterator(bool isDFS)
{
	iterator->makeIndex(getRoot(), isDFS);
}
/*
void Beacon::setIterator(BeaconIterator *iterator)
{
	this->iterator = iterator;
}
*/
BeaconIterator* Beacon::getIterator()
{
	return iterator;
}

Beacon* Beacon::next()
{
	return iterator->next();
}


///////////////////////////////////////////////////////////////////////
void BeaconIterator::makeIndex(Beacon *beacon, bool isDFS, bool isFirst)
{
	if (isFirst)
	{
		beaconIndex.clear();
		curIdx = -1;
	}

	beaconIndex.push_back(beacon);
	if (isDFS)
	{
		// DFS. recursive
		for (size_t i = 0; i < beacon->childrenSize(); i++)
		{
			makeIndex(beacon->childAt(i), isDFS, false/*isFirst*/);
		}
	}
	else
	{
		// BFS
		int cur = 0;
		Beacon *curBeacon;
		while(cur < (int)beaconIndex.size())
		{
			curBeacon = beaconIndex[cur];
			
			for (size_t i = 0; i < curBeacon->childrenSize(); i++)
			{
				beaconIndex.push_back(beacon->childAt(i));				
			}
			cur++;
		}
	}
}

Beacon* BeaconIterator::reset()
{
	curIdx = -1;
	return current();
}

Beacon* BeaconIterator::next()
{
	curIdx++;
	return current();
}

Beacon* BeaconIterator::current()
{
	if (curIdx < 0) 
		return beaconIndex[0];
	if (curIdx >= (int)beaconIndex.size())
		return NULL;
	return beaconIndex[curIdx];
}


