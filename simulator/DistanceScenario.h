#pragma once
#include "SimulatorArgument.h"


class DistanceScenario
{
public:
	
	DistanceScenario (SimulatorArgument *args, Beacon *beacon);

	bool setListener(Vector location, Vector facing);
	double getNoisyDistance();
	double getExactDistance();
	bool isValid();

	std::vector<Vector> reflectionPoints;

protected:
	double distance;
	double randomNoise;
	double applicationError;

	Vector listenerLocation;
	Vector listenerFacing;

	Beacon *virtualBeacon;
	Vector reflectionPoint1;
	Vector reflectionPoint2;

	SimulatorArgument *args;

	bool calculateDistance();
	double getApplicationError();
	double getRandomNoise(double baseError);
	bool checkValid();
	bool valid;
};

