#include "DistanceSimulator.h"

DistanceSimulator::DistanceSimulator()
{
}

DistanceSimulator::~DistanceSimulator()
{
	destructor();
}

void DistanceSimulator::destructor()
{
	for (size_t i = 0; i < scenarios.size(); i++)
	{
		delete scenarios[i];
	}
	scenarios.clear();
}

void DistanceSimulator::reset()
{
	destructor();
}


void DistanceSimulator::setup(SimulatorArgument *args, Beacon* beacon)
{
	beacon->getIterator()->reset();
	Beacon *curVBeacon = beacon;
	while((curVBeacon = curVBeacon->next()) != NULL)
	{
		scenarios.push_back(new DistanceScenario(args, curVBeacon));
	}
}


DistanceScenario* DistanceSimulator::findScenario(Vector listenerLocation, Vector listenerFacing)
{
	double minDistance = -1;
	bool valid;
	double distance;
	DistanceScenario *bestScenario = NULL;
	for (size_t i = 0; i < scenarios.size(); i++)
	{
		valid = scenarios[i]->setListener(listenerLocation, listenerFacing);
		if (!valid) continue;


		distance = scenarios[i]->getDistance();
		if (minDistance < 0 || distance < minDistance)
		{
			minDistance = distance;
			bestScenario = scenarios[i];
		}
	}

	return bestScenario;
}
