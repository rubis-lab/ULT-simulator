#include <stdio.h>
#include "EventGenerator.h"

#define SIM_REFLECTION_LIMIT 2

EventGenerator::EventGenerator()
{

}


EventGenerator::~EventGenerator()
{

}


void EventGenerator::generateEvent(SimulatorArgument *args)
{
	this->args = args;

	events.reset();
	setBeacons();
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

void EventGenerator::save(const char *filename)
{
	events.save(filename);
}


void EventGenerator::setBeacons()
{
	PlaneGenerator planeGenerator;
	BeaconDeploy beaconDeploy;

	// load plane and beacons in args->planes, args->beacons
	// plane should be loaded prior to beacons
	planeGenerator.generatePlane(args);
	beaconDeploy.deployBeacons(args);

	args->beacons.applyPlanes(&args->planes, SIM_REFLECTION_LIMIT);
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
