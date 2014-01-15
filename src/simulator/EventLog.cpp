#include <stdio.h>
#include <stdlib.h>
#include "EventLog.h"
#include "IOUtil.h"

void EventLog::addMeasurement(int bid, double distance)
{
	measurements.push_back(MeasurementLog(bid, distance));
}

//TODO: manage reflectedPoint as vector
void EventLog::setReflectedPoints(Vector point1, Vector point2)
{
	if (measurements.size() == 0)
	{
		printf("EventLog::setReflectedPoints, measurement is not defined.\n");
		exit(122);
	}
	measurements.back().reflectedPoint1 = point1;
	measurements.back().reflectedPoint2 = point2;
}



/////////////////////////////////////
/////////////EventLogList////////////

EventLogList::EventLogList()
{
	events.clear();
}

EventLogList::~EventLogList()
{
}

void EventLogList::reset()
{
	events.clear();
	reflectionCount1 = 0;
	reflectionCount2 = 0;
}

void EventLogList::setNewEvent(unsigned long timestamp, Vector location, Vector facing)
{
	events.push_back(EventLog(timestamp, location, facing));
}
void EventLogList::setNewEvent(ListenerInfo listener)
{
	setNewEvent(listener.timestamp, listener.location, listener.facing);
}

void EventLogList::addMeasurement(int bid, double distance)
{
	if (events.size() == 0)
	{
		printf("EventLogList::addMeasurement, evnet is not defined. call setNewEvent first\n");
		exit(120);
	}
	
	events.back().addMeasurement(bid, distance);
}

void EventLogList::setMeasurementReflectedPoints(Vector point1, Vector point2)
{
	if (events.size() == 0)
	{
		printf("EvnetLogList::setMeasurementReflectedPoints, evnet is not defined. call setNewEvent first\n");
		exit(121);
	}
	
	events.back().setReflectedPoints(point1, point2);

	if (!point1.isNull)
		reflectionCount1 ++;
	if (!point2.isNull)
		reflectionCount2 ++;
}

void EventLogList::addScenario(DistanceScenario *scenario)
{
	addMeasurement(scenario->getUserBid(), scenario->getDistance());

	//TODO: change below codes to cover reflectedPoint vector
	Vector rPoint1 = Vector(false);
	Vector rPoint2 = Vector(false);
	
	if (scenario->reflectedPoints.size() > 0)
		rPoint1 = scenario->reflectedPoints[0];
	if (scenario->reflectedPoints.size() > 1)
		rPoint2 = scenario->reflectedPoints[1];

	setMeasurementReflectedPoints(rPoint1, rPoint2);
	
}

size_t EventLogList::size()
{
	return events.size();
}


void EventLogList::load(const char *filename)
{
	FILE *fp;

	fp = fopen(filename, "r");

	if (fp == NULL)
	{
		printf("EventLogList::load, can't open file %s\n", filename);
		exit(21);
	}

	reset();

	const int bufSize = 4095;
	char buf[bufSize + 1];
	
	unsigned long timestamp;
	Vector location;
	Vector facing;

	int bid;
	double distance;
	Vector reflectedPoint1, reflectedPoint2;

	while (read_listener_info(fp, location, facing, &timestamp))
	{
		setNewEvent(timestamp, location, facing);

		while(fgets(buf, bufSize, fp), strcmp(read_header(buf), "dist") == 0)
		{
			bid = (int) read_value(NULL);				
			distance = read_value(NULL);
			
			read_vector(NULL, reflectedPoint1);
			read_vector(NULL, reflectedPoint2);

			addMeasurement(bid, distance);
			setMeasurementReflectedPoints(reflectedPoint1, reflectedPoint2);
		}
	}	
}

void EventLogList::save(const char *filename)
{
	FILE *fp;

	fp = fopen(filename, "w");

	if (fp == NULL)
	{
		printf("EventLogList::save, can't open file %s\n", filename);
		exit(22);
	}
	for (size_t i = 0; i < events.size(); i++)
	{
		fprintf(fp, "time;%10lu;%4d;%4d;%4d;", 
				events[i].timestamp, 
				(int) events[i].location.x, 
				(int) events[i].location.y,
				(int) events[i].location.z);
	
		fprintf(fp, "%.4f;%.4f;%.4f;\n", 
				events[i].facing.x, 
				events[i].facing.y,
				events[i].facing.z);

		for (size_t j = 0; j < events[i].measurements.size(); j++)
		{
			EventLog::MeasurementLog measurement = events[i].measurements[j];

			fprintf(fp, "dist;%4d;%4.2f;", measurement.userBid, measurement.distance);

			Vector point1 = measurement.reflectedPoint1;
			Vector point2 = measurement.reflectedPoint2;
			if (!point1.isNull)
			{
				fprintf(fp, "%4d;%4d;%4d;", (int)point1.x, (int)point1.y, (int)point1.z);
			}
			if (!point2.isNull)
			{
				fprintf(fp, "%4d;%4d;%4d;", (int)point2.x, (int)point2.y, (int)point2.z);
			}
			fprintf(fp, "\n");
		}
		fprintf(fp, "end;\n");
	}
	fprintf(fp, "endlog;\n");
	fclose(fp);
}
