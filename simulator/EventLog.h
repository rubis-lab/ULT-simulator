#pragma once
#include <vector>
#include "Vector.h"
#include "ListenerMover.h"
#include "DistanceScenario.h"

class EventLog
{
public:

	class MeasurementLog
	{
	public:
		MeasurementLog(int bid, double distance) : bid(bid), distance(distance) {}
		int bid;
		double distance;
		Vector reflectedPoint1;
		Vector reflectedPoint2;
	};

	EventLog() {}
	EventLog(unsigned long timestamp, Vector location, Vector facing) :
			timestamp(timestamp), location(location), facing(facing) {}
	~EventLog() {}


	unsigned long timestamp;
	Vector location;
	Vector facing;
	std::vector<MeasurementLog> measurements;

	void addMeasurement(int bid, double distance);
	void setReflectedPoints(Vector point1, Vector point2);

};

class EventLogList
{
public:
	EventLogList();
	~EventLogList();
	
	void reset();
	
	void setNewEvent(unsigned long timestamp, Vector location, Vector facing);
	void setNewEvent(ListenerInfo listener);
	void addMeasurement(int bid, double distance);
	void addScenario(DistanceScenario *scenario);
	void setMeasurementReflectedPoints(Vector point1, Vector point2);
	//this function set current event's last measurement's value
	size_t size();
	void load(const char *filename);
	void save(const char *filename);
	

	int reflectionCount1;
	int reflectionCount2;

	std::vector<EventLog> events;

};

