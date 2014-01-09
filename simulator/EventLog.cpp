#include <stdio.h>
#include <stdlib.h>
#include "EventLog.h"
#include "IOUtil.h"

void EventLog::addMeasurement(int bid, double distance)
{
	measurements.push_back(MeasurementLog(bid, distance));
}

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

void EventLogList::setNewEvent(unsigned long timestamp, Vector location, Vector direction)
{
	events.push_back(EventLog(timestamp, location, direction));
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
	Vector direction;

	int bid;
	double distance;
	Vector reflectedPoint1, reflectedPoint2;

	while (read_listener_info(fp, location, direction, &timestamp))
	{
		setNewEvent(timestamp, location, direction);

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
