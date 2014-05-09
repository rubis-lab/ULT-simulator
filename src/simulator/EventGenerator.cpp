#include <stdio.h>
#include "EventGenerator.h"


EventGenerator::EventGenerator()
{

}


EventGenerator::~EventGenerator()
{
	clearDistanceSimulator();
}


void EventGenerator::generateEvent(SimulatorArgument *args)
{
	this->args = args;

	events.reset();
	setDistanceSimulator();
	setPath();

	for (size_t i = 0; i < path.size(); i++)
	{
		printf("Event Generating .... %3d\r", (int)((double)i / (double)path.size() * 100));
		fflush(stdout);
		events.setNewEvent(path[i]);
		
		for (size_t j = 0; j < distances.size(); j++)
		{
			DistanceScenario* scenario;
			scenario = distances[j]->findScenario(path[i].location, path[i].facing);
			if (scenario == NULL || !scenario->isValid()) continue;

			events.addScenario(scenario);
		}

	}
	printf("Event Generating .... done\n");
}

void EventGenerator::generateEventForPlaneDetection(
		SimulatorArgument *args, 
		int listenerInterval, 
		int minMargin, 
		int z, 
		Vector vFacing)
{
	this->args = args;

	events.reset();
	setDistanceSimulator();

	int xStart = -((int)(args->width / 2 - minMargin) / listenerInterval) * listenerInterval;
	int yStart = -((int)(args->length / 2 - minMargin) / listenerInterval) * listenerInterval;
	int xEnd = -xStart;
	int yEnd = -yStart;

	unsigned long timestamp = 0;

	for (int x = xStart; x <= xEnd; x += listenerInterval)
	{
		for (int y = yStart; y <= yEnd; y += listenerInterval)
		{
			Vector vRealLocation = Vector(x, y, z);
			events.setNewEvent(timestamp, vRealLocation, vFacing);
			for (size_t i = 0; i < distances.size(); i++)
			{
				DistanceScenario* scenario;
				scenario = distances[i]->findScenario(vRealLocation, vFacing);
				if (scenario == NULL || !scenario->isValid()) continue;
				
				events.addScenario(scenario);	
			}
			timestamp += 10000;
		}
	}
}

void EventGenerator::save(const char *filename)
{
	events.save(filename);
}



void EventGenerator::setDistanceSimulator()
{
	clearDistanceSimulator();
	for (size_t i = 0; i < args->beacons.size(); i++)
	{
		DistanceSimulator *distance = new DistanceSimulator();
		distance->setup(args, args->beacons.at(i));
		
		distances.push_back(distance);
	}
}

void EventGenerator::clearDistanceSimulator()
{
	for (size_t i = 0; i < distances.size(); i++)
	{
		delete distances[i];
	}
	distances.clear();
}

void EventGenerator::setPath()
{
	PathGenerator pathGenerator;
	pathGenerator.generatePath(args, path);
}
