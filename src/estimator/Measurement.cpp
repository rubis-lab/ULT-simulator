#include <stdio.h>
#include <stdlib.h>
#include "Measurement.h"




///////////////////////////////////////////////////////
Measurement::Measurement(Beacon *beacon) : beacon(beacon)
{
	distance = 0;
//	beacon->bindMeasurementContainer(this);
}

void Measurement::measure(unsigned long timestamp, double distance)
{
	this->timestamp = timestamp;
	this->distance = distance;
}

int Measurement::getUserBid()
{
	return beacon->getUserBid();
}

Vector Measurement::getLocation()
{
	return beacon->getLocation();
}
double Measurement::getDistance()
{
	return distance;
}
unsigned long Measurement::getTimestamp()
{
	return timestamp;
}

void Measurement::clearNVSS()
{
	NVSS = 1;		// default NVSS is 1. the first one is VSS when there is no reflection
}
void Measurement::incNVSS()
{
	NVSS++;
}
int Measurement::getNVSS()
{
	return NVSS;
}

void Measurement::resetIterator()
{
	beacon = beacon->getRoot();
	beacon->getIterator()->reset();
}

bool Measurement::nextVBeacon()
{
	Beacon *nextBeacon = beacon->next();
	if (nextBeacon == NULL) return false;
	beacon = nextBeacon;
	return true;
}

bool Measurement::isValidDistance()
{	
	if (distance < beacon->getDistanceToPlane()) return false;
	return true;
}

Beacon* Measurement::getBeacon()
{
	return beacon;
}


// simulated measurement
void Measurement::simulate(unsigned long timestamp, Vector location, double noise)
{
	this->timestamp = timestamp;
	this->distance = location.getDistance(beacon->getLocation());
	this->simulatedNoise = noise;
}

void Measurement::addNoise(double noise)
{
	this->simulatedNoise = noise;
}

double Measurement::getSimulatedDistance()
{
	return distance + simulatedNoise;
}

/////////////////////////////////////////////////////////////////////////

MeasurementList::MeasurementList(int lid, BeaconList *beacons, PlaneList* planes) : 
		lid(lid), planes(planes)
{
	allMeasurements.clear();
	for (size_t i = 0; i < beacons->size(); i++)
	{
		allMeasurements.push_back(Measurement(beacons->at(i)));
	}
}
MeasurementList::~MeasurementList()
{
}

void MeasurementList::measure(int userBid, unsigned long timestamp, double distance)
{
	if (distance <= condition.minValidDistance) return;
	int idx = findBeaconIndexByUserId(userBid);
	if (idx < 0) 
	{
		printf("cannot find bid %d\n", userBid);
		exit(0);
	}
	Measurement *measurement = &allMeasurements[idx];

	measurement->measure(timestamp, distance);

	// set NVSS
	measurement->clearNVSS();
	if (planes == NULL) return;
	for (size_t i = 0; i < planes->size(); i++)
	{
		Plane *plane = planes->at(i);

		if (plane->getDistanceToPoint(measurement->getLocation()) <= measurement->getDistance())
		{
			measurement->incNVSS();
		}
	}
}

std::vector<Measurement*> MeasurementList::getFilteredMeasurements(unsigned long currentTime, unsigned long timeWindow)
{
	std::vector<Measurement*> filteredMeasurements;
	filteredMeasurements.clear();
	for (size_t i = 0; i < allMeasurements.size(); i++)
	{
		if (allMeasurements[i].getDistance() < condition.minValidDistance) continue;

		unsigned long timestamp = allMeasurements[i].getTimestamp();
		if (timestamp <= currentTime && currentTime - timestamp <= timeWindow)
		{
			filteredMeasurements.push_back(&allMeasurements[i]);
		}
	}
	return filteredMeasurements;
}

//std::vector<Measurement*> MeasurementList::getMeasurements(unsigned long currentTime, unsigned long timeWindow)
void MeasurementList::makeSnapshot(unsigned long currentTime)
{
	unsigned long timeWindow = condition.timeWindow;
	std::vector<Measurement*> filteredMeasurements = getFilteredMeasurements(currentTime, timeWindow);

	measurements.clear();

	if (!condition.canCalculate(filteredMeasurements.size()))
	{
		// TODO : ANLZ
		
		return;
	}
	

	if (condition.shortDistanceFirst)
	{
		getSortedList(filteredMeasurements);
	}
	else
	{
//		getSortedListInverse(filteredMeasurements);
		getRandomList(filteredMeasurements);
	}
}

std::vector<Measurement*> MeasurementList::getRandomList(std::vector<Measurement*> filteredMeasurements)
{
	size_t size = condition.validSize;
	if (size > filteredMeasurements.size()) 
	{
		// TODO : ANLZ
//		fprintf(stderr, "Warning. MeasurementList::getRandomList. validSize(%lu) is bigger than filteredMeasurements.size(%lu)\n",
//				size, filteredMeasurements.size());
		size = filteredMeasurements.size();
	}

	for (size_t i = 0; i < size; i++)
	{
		int idx;
		if (condition.random == NULL)
		{
			idx = rand() % filteredMeasurements.size();
		}
		else
		{
			idx = (int)condition.random->getUniformDist(0, filteredMeasurements.size());
		}
		measurements.push_back(filteredMeasurements[idx]);
		filteredMeasurements.erase(filteredMeasurements.begin() + idx);
	}

	return measurements;
}

std::vector<Measurement*> MeasurementList::getSortedList(std::vector<Measurement*> filteredMeasurements)
{
	size_t size = condition.validSize;
	if (size > filteredMeasurements.size()) 
	{
		// TODO : ANLZ
//		fprintf(stderr, "Warning. MeasurementList::getSortedList. size(%ld) is bigger than filteredMeasurements.size(%ld)\n",
//				size, filteredMeasurements.size());
		size = filteredMeasurements.size();
	}
#if 0
	//debug
	printf("\nmeasurement list\n");

	for (size_t i = 0; i < filteredMeasurements.size(); i++)
	{
		Vector location = filteredMeasurements[i]->getLocation();
		int nVSS = filteredMeasurements[i]->getNVSS();
		double distance = filteredMeasurements[i]->getDistance();

		printf("(%d, %d, %d) : %f\t%d\n", (int)location.x,(int)location.y, (int)location.z, distance, nVSS);

	}
#endif


	int bestIdx;
	int minNVSS;
	double minDistance;
	for (size_t i = 0; i < size; i++)
	{
		bestIdx = -1;
		for (size_t j = 0; j < filteredMeasurements.size(); j++)
		{
			if (condition.smallerNVSSFirst)
			{
				if (bestIdx < 0 || filteredMeasurements[j]->getNVSS() < minNVSS)
				{
					minNVSS = filteredMeasurements[j]->getNVSS();
					bestIdx = j;
					minDistance = filteredMeasurements[j]->getDistance();
				}
				else if (filteredMeasurements[j]->getNVSS() == minNVSS)
				{
					if (filteredMeasurements[j]->getDistance() < minDistance)
					{
						bestIdx = j;
						minDistance = filteredMeasurements[j]->getDistance();
					}
				}
			}
			else
			{
				if (bestIdx < 0 || filteredMeasurements[j]->getDistance() < minDistance)
				{
					minDistance = filteredMeasurements[j]->getDistance();
					bestIdx = j;
				}
			}
		}
		measurements.push_back(filteredMeasurements[bestIdx]);
		filteredMeasurements.erase(filteredMeasurements.begin() + bestIdx);
	}
#if 0
	printf("sorted list\n");
	for (size_t i = 0; i < measurements.size(); i++)
	{
		Vector location = measurements[i]->getLocation();
		int nVSS = measurements[i]->getNVSS();
		double distance = measurements[i]->getDistance();

		printf("(%d, %d, %d) : %f\t%d\n", (int)location.x,(int)location.y, (int)location.z, distance, nVSS);

	}
	printf("-------------------------------------------\n");
	getchar();
#endif

	return measurements;
}


std::vector<Measurement*> MeasurementList::getSortedListInverse(std::vector<Measurement*> filteredMeasurements)
{
	size_t size = condition.validSize;
	if (size > filteredMeasurements.size()) 
	{
		size = filteredMeasurements.size();
	}

	int bestIdx;
	double maxDistance;
	for (size_t i = 0; i < size; i++)
	{
		bestIdx = -1;
		for (size_t j = 0; j < filteredMeasurements.size(); j++)
		{
			if (bestIdx < 0 || filteredMeasurements[j]->getDistance() > maxDistance)
			{
				maxDistance = filteredMeasurements[j]->getDistance();
				bestIdx = j;
			}
		}
		measurements.push_back(filteredMeasurements[bestIdx]);
		filteredMeasurements.erase(filteredMeasurements.begin() + bestIdx);
	}

	return measurements;
}




int MeasurementList::findBeaconIndexByUserId(int userBid)
{
	for (size_t i = 0; i < allMeasurements.size(); i++)
	{
		if (allMeasurements[i].getUserBid() == userBid)
			return i;
	}
	return -1;
}


void MeasurementList::setMeasurementCondition(MeasurementCondition condition)
{
	this->condition = condition;
}


int MeasurementList::getLid()
{
	return lid;
}

size_t MeasurementList::size()
{
	return measurements.size();
}

void MeasurementList::resetVBeacon(int idx)
{
	measurements[idx]->resetIterator();
}

void MeasurementList::setupVBeacon()
{
	for (size_t i = 0; i < measurements.size(); i++)
	{
		resetVBeacon(i);
	}
}
bool MeasurementList::nextVBeacon(int idx)
{
	return measurements[idx]->nextVBeacon();
}
std::vector<Measurement*> MeasurementList::getMeasurements()
{
	return measurements;
}

