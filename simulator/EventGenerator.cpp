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
	this->beacons = &args.beacons;
	this->planes = &args.planes;

	PlaneGenerator planeGenerator;
	BeaconDeploy beaconDeploy;

	// load plane and beacons in args->planes, args->beacons
	// plane should be loaded prior to beacons
	planeGenerator.generatePlane(args);
	beaconDeploy.deployBeacons(args);

	args->beacon.applyPlanes(&args->plane, SIM_REFLECTION_LIMIT);
	


	
	
}


void EventGenerator::setBeacons()
{
	
}

