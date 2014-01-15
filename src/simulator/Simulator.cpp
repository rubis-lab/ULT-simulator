#include <stdio.h>
#include <stdlib.h>
#include "EventGenerator.h"
#include "Estimator.h"
#include "SimulatorArgument.h"



void setBaseName(SimulatorArgument *simArgs, EstimatorArgument *estArgs, char* baseName)
{
	int nb = simArgs->beacons.size();
	int nv = estArgs->validSize;
	char mt[10];
	switch(simArgs->moveType)
	{
		case SIM_WALKER::CLOCKWISE : 		strcpy(mt, "C"); break;
		case SIM_WALKER::ANTI_CLOCKWISE : 	strcpy(mt, "A"); break;
		case SIM_WALKER::RANDOM : 			strcpy(mt, "R"); break;
		case SIM_WALKER::PLANE_DETECTION : 	strcpy(mt, "P"); break;
	}
	int opt = estArgs->optimization;
	sprintf(baseName, "nb%02d_nv%02d_mt%s_opt%02d", nb, nv, mt, opt);
}

int main(int argc, char** argv)
{
	char configFilename[256];
	char baseName[256];
	char outname[256];
	char dataname[256];

	if (argc > 1)
		strncpy(configFilename, argv[1], 255);
	else
		strcpy(configFilename, "conf.txt");

	SimulatorArgument simArgs;
	simArgs.load(configFilename);

	EventGenerator eventGenerator;
	eventGenerator.generateEvent(&simArgs);

	//during generating event, beacons and planes are also auto generated
	simArgs.beacons.save("beacon.auto.conf");
	simArgs.planes.save("plane.auto.conf");


	Estimator estimator, estimatorNaive;
	EstimatorArgument estArgs, estArgs2;

	estArgs.load(configFilename);
	estimator.setEstimator(&estArgs);

	estArgs2.load(configFilename);
	estArgs2.estimatorMode = EST::TRADITIONAL;
	estimatorNaive.setEstimator(&estArgs2);


	EventLogList *events = &eventGenerator.events;	

	setBaseName(&simArgs, &estArgs, baseName);
	sprintf(outname, "%s.log", baseName);
	sprintf(dataname, "%s.dat", baseName);

	//eventGenerator.save(dataname);

	double err1 = 0;
	double err2 = 0;

	for (size_t i = 0; i < events->size(); i++)
	{
		EventLog event = events->events[i];
		unsigned long timestamp = event.timestamp;

		for (size_t j = 0; j < event.measurements.size(); j++)
		{
			estimator.measure(timestamp,
					event.measurements[j].userBid,
					event.measurements[j].distance);

			estimatorNaive.measure(timestamp,
					event.measurements[j].userBid,
					event.measurements[j].distance);
		}
		EstimatorResult result;

		result = estimator.solve(timestamp);
		Vector ori_point = event.location;
		Vector slv_point = result.location;

		result = estimatorNaive.solve(timestamp);
		Vector slv_point2 = result.location;

		err1 += slv_point.getDistance(ori_point);
		err2 += slv_point2.getDistance(ori_point);

		fprintf(stderr, "%d\t%5.1f\t%5.1f\r", (int) i, err1 / (i + 1), err2 / (i + 1));
	}
	FILE *fp = fopen(outname, "w");
	if (fp == NULL)
	{
		printf("can't open %s\n", outname);
		exit(1);
	}

	double total = (double)events->size();

	fprintf(fp, "%f\t%f\n", err1 / total, err2 / total);

	fprintf(fp, "N_steps             : %d\n", (int)total);
	fprintf(fp, "error(proposed)     : %f\n", err1 / (double)total);
	fprintf(fp, "error(original)     : %f\n", err2 / (double)total);

	estArgs.analyzer.printAVG(fp);
	estArgs.analyzer.printAVG(stdout);

	fclose(fp);
}


