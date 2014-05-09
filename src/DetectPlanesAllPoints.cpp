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
		EventLogList *eventsDirect, 
		Vector *bestLocation, 
		Vector *bestLocationSimulated,
		double *pMinError)
{
	EventGenerator reflectedEventGenerator;
	// normal vector of each plane faces outside of cube
	Vector vFacing = plane->vNormal;
	
	printf("\nGenerating events for Plane %d...", planeIdx);
	fflush(stdout);
	reflectedEventGenerator.generateEventForPlaneDetection(
			simArgs, 
			listenerInterval, 
			minMargin, 
			listenerZ, 
			vFacing);
	EventLogList *eventsReflected = &reflectedEventGenerator.events;
	printf("done\n");

	double minError = -1.0;
	// meaningless plane
	Plane bestPlane(Vector(0, 0, 0), Vector(0, 0, 1));
	
	char filename[255];
	sprintf(filename, "pd_%02d.dat.%02d", planeIdx, simArgs->randomSeed);
	FILE* fp = fopen(filename, "w");

	double prev_x = 0.0;	
	// in here, assume that order of location of each eventDirect is identical to eventReflected 
	for (size_t i = 0; i < eventsDirect->size(); i++)
	{
		printf("Detecting Plane %d...%3d%% (%7d / %7d) \r", 
				planeIdx,
				(int)((i + 1) / (double)eventsDirect->size() * 100),
				(int)(i + 1), 
				(int) eventsDirect->size());
		fflush(stdout);
		EventLog eventD = eventsDirect->events[i];
		EventLog eventR = eventsReflected->events[i];
		Vector vRealListener = eventD.location;

		Vector vListener = getSimulatedListener(&eventD);
		double estimationError = vListener.getDistance(vRealListener);
		
		PlaneDetector planeDetector(vListener);

		if (vRealListener.x != prev_x) fprintf(fp, "\n");
		prev_x = vRealListener.x;
		//log real listener location
		fprintf(fp, "%f\t%f\t%f\t", vRealListener.x, vRealListener.y, vRealListener.z);

		// not enough measurements. can not detect plane. continue
		if (eventR.measurements.size() < 4)
		{
			fprintf(fp, "-100.0\t-100.0\t%f\n", estimationError);
			continue;
		}


		//do plane detection
		for (size_t j = 0; j < eventR.measurements.size(); j++)
		{
			int bid = eventR.measurements[j].userBid;
			Vector vBeacon = (simArgs->beacons.findByUserBid(bid))->getLocation();
			double distance = eventR.measurements[j].distance;
			
			//insert reflected distance data into plane detector
			planeDetector.addReflectedDistance(vBeacon, distance);
		}

		Plane detectedPlane = planeDetector.getPlane();
		double error = planeDetector.getDistanceError(*plane);
		double errorAngle = planeDetector.getAngleError(*plane);

		if (minError < 0 || error < minError)
		{
			minError = error;
			bestPlane = detectedPlane;
			*bestLocation = vRealListener;
			*bestLocationSimulated = vListener;
		}

		fprintf(fp, "%f\t%f\t%f\n", error, errorAngle, estimationError);
	}

	fclose(fp);

	*pMinError = minError;

	printf("\n");
	return bestPlane;
}


int main(int argc, char** argv)
{
	// this version detecting only one plane for performance.
	// only calculates detection error at each point.
	// thus, detected plane information is not logged.
	// plane index should be 0~3
	int planeIdx = 1;

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

	
	Plane detectedPlane = detectPlane(
			&simArgs, 
			realPlanes.at(planeIdx), 
			planeIdx/*pid*/, 
			&directEventGenerator.events,
			&bestLocation[planeIdx],
			&bestLocationSimulated[planeIdx],
			&error[planeIdx]);
	printf("min error : %f\n", error[planeIdx]);
}










