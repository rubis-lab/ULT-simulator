#pragma once

#include "SimulatorArgument.h"
#include "DistanceScenario.h"

class DistanceSimulator
{
public:
	DistanceSimulator();
	~DistanceSimulator();

	void reset();
	void setup(SimulatorArgument *args, Beacon* beacon);
	DistanceScenario* findScenario(Vector listenerLocation, Vector listenerFacing);

private:
	std::vector<DistanceScenario*> scenarios;

	void destructor();


};
