#pragma once
#include "BeaconList.h"
#include "PlaneList.h"
#include "Measurement.h"



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
	enum autoDeploy
	{
		NONE,
		CIRCULAR,
		CIRCULAR2,
		MAGNETIC,
		RANDOM,
	}
}


class EventGeneratorArgument
{
	int width;
	int length;
	int height;
	double speedAvg;
	double speedDev;
	double angleAvg;
	double angleDev;
	WALKER::type moveType;
	int validAngleRange;
	int eventSize;
	int randomSeed;


	void setDefault()
	{
		width = 1000;
		length = 1000;
		height = 300;
		speeedAvg = 0.5
		speedDev = 0.025;
		angleAvg = 0.00;
		angleDev = 1.00;
		validAngleRange = 170;
		moveType = WALKER::RANDOM;

	}
}


class EventGenerator
{
public:
	EventGenerator();
	~EventGenerator();

protected:

BeaconList *beacons;
PlaneList *planes;
MeasurementList *measurementList;

};
