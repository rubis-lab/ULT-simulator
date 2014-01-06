#pragma once
#include <vector>
#include "Vector.h"

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
	EventLog(unsigned long timestamp, Vector location, Vector direction) :
			timestamp(timestamp), location(location), direction(direction) {}
	~EventLog() {}


	unsigned long timestamp;
	Vector location;
	Vector direction;
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
	
	void setNewEvent(unsigned long timestamp, Vector location, Vector direction);
	void addMeasurement(int bid, double distance);
	void setMeasurementReflectedPoints(Vector point1, Vector point2);
	//this function set current event's last measurement's value
	

	int reflectionCount1;
	int reflectionCount2;

	size_t size();


	std::vector<EventLog> events;

};

