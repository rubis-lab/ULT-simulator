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
		events.setNewEvent(path[i]);
		
		for (size_t i = 0; i < distances.size(); i++)
		{
			DistanceScenario* scenario;
			scenario = distances[i].findScenario(path[i].location, path[i].facing);
			if (!scenario->isValid()) continue;

			events.addScenario(scenario);
		}

	}
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
	distances.clear();
	for (size_t i = 0; i < args->beacons.size(); i++)
	{
		distances.push_back(DistanceSimulator());
		distances[i].setup(args, args->beacons.at(i));
	}
}

void EventGenerator::setPath()
{
	PathGenerator pathGenerator;
	pathGenerator.generatePath(args, path);
}
