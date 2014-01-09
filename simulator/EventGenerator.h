#pragma once
#include "SimulatorArgument.h"
#include "Measurement.h"
#include "DistanceSimulator.h"
#include "EventLog.h"
#include "PlaneGenerator.h"
#include "BeaconDeploy.h"



class EventGenerator
{
public:
	EventGenerator();
	~EventGenerator();

	EventLogList events;
	void generateEvent(SimulatorArgument *args);

protected:

	MeasurementList *measurementList;

	SimulatorArgument* args;
	std::vector<DistanceSimulator> distances;

	void setBeacon();
	void setBeacons();
	void setDistanceSimulator();

};
