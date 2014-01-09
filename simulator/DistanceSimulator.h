#pragma once
#include "SimulatorArgument.h"


class SimulatedDistance
{
public:
	
	SimulatedDistance (SimulatorArgument *args, Beacon *beacon);

	void setListener(Vector location, Vector facing);
	double getNoisyDistance();
	double getExactDistance();

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

	void calculateDistance();
	double getApplicationError();
	double getRandomNoise(double baseError, double noiseAvg, double noiseDev);
};

class DistanceSimulator
{
public:
	DistanceSimulator();



};
