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

	setBeacons();
	setDistanceSimulator();

	for (int i = 0; i < args->eventSize; i++)
	{





	}

	
	
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
	distances = std::vector<DistanceSimulator>(args->beacons.size());
	for (size_t i = 0; i < args->beacons.size(); i++)
	{
		distances[i].setup(args, args->beacons.at(i));
	}
}
