#pragma once
#include "SimulatorArgument.h"
#include "PlaneList.h"

class PlaneGenerator
{
public:
	void generatePlane(SimulatorArgument *args);

protected:
	SimulatorArgument *args;
	void generateCube(SimulatorArgument *args);
};



