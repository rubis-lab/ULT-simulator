#pragma once

#include "BeaconList.h"
#include "PlaneList.h"

namespace SIM_WALKER
{
	enum type
	{
		RANDOM,
		CLOCKWISE,
		ANTI_CLOCKWISE,
		PLANE_DETECTION,
	};
}

namespace SIM_BEACON
{
	enum deploy
	{
		BYFILE,
		CIRCULAR,
		CIRCULAR2,
		COULOMB,
		COULOMB2,
		RANDOM,
	};
}


class SimulatorArgument
{
public:
	SimulatorArgument()
	{
		setDefault();
	}

	int width;
	int length;
	int height;
	double speedAvg;
	double speedDev;
	double angleAvg;
	double angleDev;
	int validAngleRange;
	
	SIM_WALKER::type moveType;
	SIM_BEACON::deploy deployType;
	char* beaconDeployFilename;
	int beaconSize;;


	int eventSize;
	int randomSeed;
	
	BeaconList beacons;
	PlaneList planes;


	void setDefault()
	{
		width = 1000;
		length = 1000;
		height = 300;
		speedAvg = 0.5;
		speedDev = 0.025;
		angleAvg = 0.00;
		angleDev = 1.00;
		validAngleRange = 170;

		moveType = SIM_WALKER::RANDOM;
		deployType = SIM_BEACON::RANDOM;
		beaconDeployFilename = NULL;
		beaconSize = 10;

		eventSize = 10000;
		randomSeed = 0;

	}
};
