#pragma once
#include "SimulatorArgument.h"
#include "Measurement.h"
#include "DistanceSimulator.h"
#include "EventLog.h"
#include "PlaneGenerator.h"
#include "BeaconDeploy.h"
#include "PathGenerator.h"



class EventGenerator
{
public:
	EventGenerator();
	~EventGenerator();

	EventLogList events;
	void generateEvent(SimulatorArgument *args);
	void save(const char *filename);

protected:

	MeasurementList *measurementList;

	SimulatorArgument* args;
	std::vector<DistanceSimulator*> distances;
	std::vector<ListenerInfo> path;

	void setBeacon();
	void setBeacons();
	void setDistanceSimulator();
	void setPath();

	void clearDistanceSimulator();

};
