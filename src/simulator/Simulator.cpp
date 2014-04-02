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
	char bd[10];
	switch(simArgs->moveType)
	{
		case SIM_WALKER::CLOCKWISE : 		strcpy(mt, "C"); break;
		case SIM_WALKER::ANTI_CLOCKWISE : 	strcpy(mt, "A"); break;
		case SIM_WALKER::RANDOM : 			strcpy(mt, "R"); break;
		case SIM_WALKER::PLANE_DETECTION : 	strcpy(mt, "P"); break;
	}
	switch(simArgs->deployType)
	{
		case SIM_BEACON::BYFILE :		strcpy(bd, "FILE"); break;
		case SIM_BEACON::CIRCULAR :		strcpy(bd, "CIR1"); break;
		case SIM_BEACON::CIRCULAR2 :	strcpy(bd, "CIR2"); break;
		case SIM_BEACON::COULOMB :		strcpy(bd, "CLM1"); break;
		case SIM_BEACON::COULOMB2 :		strcpy(bd, "CLM2"); break;
		case SIM_BEACON::RANDOM :		strcpy(bd, "RAND"); break;

	}
	int opt = estArgs->optimization;
	int meas = simArgs->maxMeasurableDistance / 100;
	int angleRange = simArgs->validAngleRange / 10;
	sprintf(baseName, "nb%02d_nv%02d_bd%s_mt%s_opt%02d_md%02d_ar%02d", nb, nv, bd, mt, opt, meas, angleRange);
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


	// get reflection count
	estArgs.analyzer.N_reflection.addValue(events->reflectionCount1);
	estArgs.analyzer.N_reflection2.addValue(events->reflectionCount2);


//	eventGenerator.save(dataname);

	double err_sum1 = 0;
	double err_sum2 = 0;

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
		EstimatorResult result, result2;

		result = estimator.solve(timestamp);
		Vector ori_point = event.location;
		Vector slv_point = result.location;
		

		result2 = estimatorNaive.solve(timestamp);
		Vector slv_point2 = result2.location;

		double err1 = slv_point.getDistance(ori_point);
		double err2 = slv_point2.getDistance(ori_point);

		estArgs.analyzer.estimationError.addValue(err1);
		// Reception Level
		// 0 : recept over N_V          (R >= N_V)
		// 1 : recept over 3 under N_V  (3 <= R < N_V)
		// 2 : recept under 3			(R < 3 )
		// estimation error except fail : include error when level is 0 and 1
		// estimation error except fail strictly : include error when level is only 0
		
		switch(result.receptionLevel)
		{
		case 0 :	// R >= N_V		(success)
			//estimation error
			estArgs.analyzer.estimationErrorExcFail.addValue(err1);
			estArgs.analyzer.estimationErrorExcStrictFail.addValue(err1);
			//no fail count
			break;

		case 1 : 	// 3 <= R < N_V 	(fail only in strict mode)
			//estimation error
			estArgs.analyzer.estimationErrorExcFail.addValue(err1);
			//fail count
			estArgs.analyzer.N_receptionFailStrict.addValue(1.0);
			break;

		
		case 2 : 	// R < 3     		(fail both case)
			//no estimation error
			//fail count
			estArgs.analyzer.N_receptionFail.addValue(1.0);
			estArgs.analyzer.N_receptionFailStrict.addValue(1.0);
			break;
			
		default : 
			printf("Simulator::main, undefined reception level\n");
			exit(0);
		}


		estArgs.analyzer.estimationError2.addValue(err2);

		switch(result2.receptionLevel)
		{
		case 0 :	// R >= N_V		(success)
			//estimation error
			estArgs.analyzer.estimationErrorExcFail2.addValue(err2);
			estArgs.analyzer.estimationErrorExcStrictFail2.addValue(err2);
			//no fail count
			break;

		case 1 : 	// 3 <= R < N_V 	(fail only in strict mode)
			//estimation error
			estArgs.analyzer.estimationErrorExcFail2.addValue(err2);
			//fail count
			estArgs.analyzer.N_receptionFailStrict2.addValue(1.0);
			break;

		
		case 2 : 	// R < 3     		(fail both case)
			//no estimation error
			//fail count
			estArgs.analyzer.N_receptionFail2.addValue(1.0);
			estArgs.analyzer.N_receptionFailStrict2.addValue(1.0);
			break;
			
		default : 
			printf("Simulator::main, undefined reception level\n");
			exit(0);
		}

		err_sum1 += err1;
		err_sum2 += err2;

		fprintf(stderr, "%d\t%5.1f\t%5.1f\r", (int) i, err_sum1 / (i + 1), err_sum2 / (i + 1));
	}
	FILE *fp = fopen(outname, "w");
	if (fp == NULL)
	{
		printf("can't open %s\n", outname);
		exit(1);
	}

	double total = (double)events->size();

	fprintf(fp, "%f\t%f\n", err_sum1 / total, err_sum2 / total);

	fprintf(fp, "N_steps             : %d\n", (int)total);
	fprintf(fp, "error(proposed)     : %f\n", err_sum1 / (double)total);
	fprintf(fp, "error(original)     : %f\n", err_sum2 / (double)total);

	estArgs.analyzer.printAVG(fp);


	fclose(fp);
}


