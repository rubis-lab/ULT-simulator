#pragma once
#include "SimulatorArgument.h"


class DistanceScenario
{
public:
	
	DistanceScenario (SimulatorArgument *args, Beacon *beacon);

	bool setListener(Vector location, Vector facing);
	double getNoisyDistance();
	double getExactDistance();
	double getDistance();
	bool isValid();
	int getBid();

	std::vector<Vector> reflectedPoints;

protected:
	double distance;
	double randomNoise;
	double applicationError;

	Vector listenerLocation;
	Vector listenerFacing;

	Beacon *virtualBeacon;

	SimulatorArgument *args;

	bool calculateDistance();
	double getApplicationError();
	double getRandomNoise(double baseError);
	bool checkValid();
	bool valid;
};

