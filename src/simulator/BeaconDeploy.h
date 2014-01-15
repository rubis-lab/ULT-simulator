#pragma once

#include "SimulatorArgument.h"
#include "BeaconList.h"


class BeaconDeploy
{
public:
	void deployBeacons(SimulatorArgument *args);

private:
	SimulatorArgument *args;
	void deployByFile();
	void deployCircular();
	void deployCircular2();
	void deployCoulomb();
	void deployCoulomb2();
	void deployRandom(Random *random);
	
	
};

