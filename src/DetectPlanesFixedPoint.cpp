#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SimulatorArgument.h"
#include "PlaneDetector.h"
#include "EventGenerator.h"
#include "Estimator.h"

const int listenerInterval = 5;
const int minMargin = 5;
const int listenerZ = 130;

Estimator estimator;
EstimatorArgument estArgs;

Vector getSimulatedListener(EventLog *event)
{
#if 0
	return event->location;
#else
	unsigned long timestamp = event->timestamp;
	for (size_t i = 0; i < event->measurements.size(); i++)
	{
		estimator.measure(timestamp, event->measurements[i].userBid, event->measurements[i].distance);
	}
	EstimatorResult result = estimator.solve(timestamp);
	
//	printf("%f\n", event->location.getDistance(result.location));

	return result.location;
#endif
}

Plane detectPlane(
		SimulatorArgument *simArgs, 
		Plane *plane, 
		int planeIdx,
		EventLog *eventD,
		EventLog *eventR,
		Vector *bestLocation, 
		Vector *bestLocationSimulated,
		double *pMinError)
{
	double minError = -1.0;
	// meaningless plane
	Plane bestPlane(Vector(0, 0, 0), Vector(0, 0, 1));
	


	// in here, assume that order of location of each eventDirect is identical to eventReflected 
	Vector vRealListener = eventD->location;

	Vector vListener = getSimulatedListener(eventD);

	PlaneDetector planeDetector(vListener);


	// not enough measurements. can not detect plane. continue
	if (eventR->measurements.size() < 4)
	{
		fprintf(stderr, "invalid point!! planeIdx = %d\n", planeIdx);
		exit(1);
	}

	//do plane detection
	for (size_t j = 0; j < eventR->measurements.size(); j++)
	{
		int bid = eventR->measurements[j].userBid;
		Vector vBeacon = (simArgs->beacons.findByUserBid(bid))->getLocation();
		double distance = eventR->measurements[j].distance;

		//insert reflected distance data into plane detector
		planeDetector.addReflectedDistance(vBeacon, distance);
	}

	Plane detectedPlane = planeDetector.getPlane();
	double error = planeDetector.getDistanceError(*plane);

	if (minError < 0 || error < minError)
	{
		minError = error;
		bestPlane = detectedPlane;
		*bestLocation = vRealListener;
		*bestLocationSimulated = vListener;
	}

	*pMinError = minError;
	return bestPlane;
}

Vector getFixedPoint(Plane *plane, double gap)
{
	// plane should have four vertexes.
	
	Vector vMid = (plane->getVertex(0) + plane->getVertex(1) + 
			plane->getVertex(2) + plane->getVertex(3)) * 0.25;
	
	vMid.z = listenerZ;

	vMid = vMid - (plane->vNormal * gap);
//	vMid.x += 50;

	return vMid;

}

int getFixedPointIndex(Vector fixedPoint, EventLogList *eventsDirect)
{
	//find nearest point from fixedPoint;
	double minDistance = -1.0;
	int fixedPointIdx = -1;
	for (size_t i = 0; i < eventsDirect->size(); i++)
	{
		Vector vLocation = eventsDirect->events[i].location;
		double distance = vLocation.getDistance(fixedPoint);

		if (minDistance < 0 || distance < minDistance)
		{
			fixedPointIdx = i;
			minDistance = distance;
		}
	}
	return fixedPointIdx;
}


bool checkReflection(EventLog *event)
{
	for (size_t i = 0; i < event->measurements.size(); i++)
	{
		if (!event->measurements[i].reflectedPoint1.isNull)
		{
			return false;
		}
	}
	return true;
}

int main(int argc, char** argv)
{
	char configFilename[256];
	
	if (argc > 1)
		strncpy(configFilename, argv[1], 255);
	else
		strcpy(configFilename, "conf.txt");

	SimulatorArgument simArgs;
	simArgs.load(configFilename);

	// real plane should be saved as plane.auto.conf, but for convenice, save plane.detected.conf
	// this will be rewrite after detecting plane process
	simArgs.planes.save("plane.detected.conf");
	simArgs.beacons.save("beacon.auto.conf");

	estArgs.load(configFilename);
	estArgs.estimatorMode = EST::TRADITIONAL;
	estArgs.optimization = 0;
	estimator.setEstimator(&estArgs);

	

	PlaneList realPlanes;

	PlaneList detectedPlanes;

	realPlanes.makeCube(simArgs.width, simArgs.length, simArgs.height);
	// there is 6 planes in realPlanes.
	// index 0~3 are side wall, 4 and 5 are ceiling and floor
	

	printf("Generating direct signal events...");
	fflush(stdout);
	// events for directly receiving signals
	EventGenerator directEventGenerator;
	directEventGenerator.generateEventForPlaneDetection(
			&simArgs, listenerInterval, minMargin, listenerZ, Vector(0, 0, 1));
	printf("done\n");

	Vector bestLocation[4];
	Vector bestLocationSimulated[4];
	double error[4];

	
	for (int i = 0; i < 4; i++)
	{
		EventGenerator reflectedEventGenerator;
		// normal vector of each plane faces outside of cube
		Vector vFacing = realPlanes.at(i)->vNormal;

		printf("\nGenerating events for Plane %d...", i);
		fflush(stdout);
		reflectedEventGenerator.generateEventForPlaneDetection(
				&simArgs, listenerInterval, minMargin, listenerZ, vFacing);
		printf("done\n");

		double gap = 100;

		Vector fixedPoint;
		int fixedPointIdx;
		EventLog *eventD;
		EventLog *eventR;
		
		for (gap = 100; gap > 0; gap -= 5)
		{

			fixedPoint = getFixedPoint(realPlanes.at(i), gap);
			fixedPointIdx = getFixedPointIndex(fixedPoint, &directEventGenerator.events);
			eventD = &directEventGenerator.events.events[fixedPointIdx];
			eventR = &reflectedEventGenerator.events.events[fixedPointIdx];

			if (!checkReflection(eventD)) continue;
			if (eventR->measurements.size() >= 5) break;
		};
		if (gap <= 0)
		{
			fprintf(stderr, "can not find valid point for plane %d\n", i);
			exit(1);
		}

		printf("fixed point for plane %d  (gap : %d) = ", i, (int)gap);
		fixedPoint.println();

		Plane detectedPlane = detectPlane(
				&simArgs, 
				realPlanes.at(i), 
				i/*pid*/, 				
				eventD,
				eventR,
				&bestLocation[i],
				&bestLocationSimulated[i],
				&error[i]);
		detectedPlane.setBoundary(true);
		detectedPlanes.addPlane(detectedPlane);
		printf("min error : %f\n", error[i]);
	}

	for (int i = 0; i < 5; i++)
	{
		// make test plane list. this list has mixed set of real plane and detected plane
		PlaneList testPlanes;
		for (int j = 0; j < 4; j++)
		{
			if (i > j)			
				testPlanes.addPlane(*detectedPlanes.at(j));
			else
				testPlanes.addPlane(*realPlanes.at(j));
		}

		testPlanes.addPlane(*realPlanes.at(4));
		testPlanes.addPlane(*realPlanes.at(5));

		char filename[255];
		sprintf(filename, "plane.detected.%d.conf", i);
		
		testPlanes.save(filename);	
	}

		

	printf("\nResults");

	for (int i = 0; i < 4; i++)
	{
		printf("\n");
		printf("detected plane            : ");
		detectedPlanes.at(i)->println();

		
		printf("best location             : ");
		bestLocation[i].println();

		printf("best location (simulated) : ");
		bestLocationSimulated[i].println();

		printf("error                     : %f\n", error[i]);
		
	}

	// add ceiling and floor
	detectedPlanes.addPlane(*realPlanes.at(4));
	detectedPlanes.addPlane(*realPlanes.at(5));

	// save detected planes
	detectedPlanes.save("plane.detected.conf");
}










