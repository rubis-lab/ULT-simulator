#pragma once
#include "SimulatorArgument.h"
#include "Measurement.h"



class EventGenerator
{
public:
	EventGenerator();
	~EventGenerator();

	EventLogList events;
	void generateEvent(SimulatorArgument *args);

protected:

	BeaconList *beacons;
	PlaneList *planes;
	MeasurementList *measurementList;

	SimulatorArgument* args;

	void setBeacon();


};
