#pragma once

#include "BeaconList.h"
#include "PlaneList.h"

namespace SIM_WALKER
{
	enum type
	{
		RANDOM,				// in configuration file, RND
		CLOCKWISE,			// CW
		ANTI_CLOCKWISE,		// ACW
		PLANE_DETECTION,	// PD
	};
}

namespace SIM_BEACON
{
	enum deploy
	{
		BYFILE,				// in configuration file, FILE
		CIRCULAR,			// CIR
		CIRCULAR2,			// CIR2
		COULOMB,			// CLMB
		COULOMB2,			// CLMB2
		RANDOM,				// RND
	};
}

namespace SIM_PLANE
{
	enum type
	{
		BYFILE,				// in configuration file, FILE
		CUBE,				// CUBE
	};
}


class SimulatorArgument
{
public:
	SimulatorArgument();

	int width;
	int length;
	int height;
	int timeslot;
	double speedAvg;
	double speedDev;
	double angleAvg;
	double angleDev;
	int validAngleRange;
	double distanceNoiseAvg;
	double distanceNoiseDev;
	int maxMeasurableDistance;
	int minMeasurableDistance;
	
	SIM_WALKER::type moveType;
	SIM_BEACON::deploy deployType;
	char beaconDeployFilename[256];
	int beaconSize;

	SIM_PLANE::type planeType;
	char planeFilename[256];


	int eventSize;
	int randomSeed;
	
	BeaconList beacons;
	PlaneList planes;

	void setDefault();
	void load(const char *filename);
	void save(const char *filename);
};
