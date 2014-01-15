// SimulationTracer.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//
#include <stdio.h>
#include <string.h>
#include "LogRecorder.h"
#include "Argument.h"
#include "Analyzer.h"




//Argument args;

/*static LogRecorder logger;*/
/*static LocationEstimator estimator;*/

//int width, length, height;

void create_path(Argument& args,LogRecorder *logger,char* filename)
{
	printf("creating path ...");
	if (!logger->CreateLog(filename, args))
	{
		getchar();
		exit(0);
	}
	printf("done\n");
}

void set_estimator(Argument& args,LogRecorder *logger, LocationEstimator *estimator)
{
//	estimator->Reset();
//	estimator->SetPlanes(logger->l_width, logger->l_length, logger->l_height);
	for (int i = 0; i < logger->nBeacon; i++)
	{
		estimator->SetBeaconInfo(logger->logList[0].beaconList[i].beacon_id,
			logger->logList[0].beaconList[i].location);
	}
	estimator->filterManager = new FilterManager(args);
}

void load_path(Argument& args,LogRecorder *logger, LocationEstimator *estimator, char* filename, int *ref_cnt1, int *ref_cnt2)
{
	printf("loading path ...");
	if (!logger->LoadLog(filename, ref_cnt1, ref_cnt2))
	{
		getchar();
		exit(0);
	}
	set_estimator(args,logger, estimator);
	printf("done\n");
}

void write_map(LogRecorder *logger, char* mapname)
{
	printf("writing map ...");
	FILE *fp = fopen(mapname, "w");
	for (unsigned i = 0; i < logger->logList.size(); i++)
	{
		fprintf(fp, "%f\t%f\n", logger->logList[i].vPosition.x, 
								logger->logList[i].vPosition.y);
	}
	fclose(fp);
	printf("done\n");
}

LogRecorder* alloc_logger(Argument& args, LocationEstimator* estimator,int seed)
{
	char filename[256];
	char mapname[256];
	int ref_cnt1, ref_cnt2;

	sprintf(filename, "%s.dat", args.Basename);
	sprintf(mapname, "%s.map", args.Basename);

	LogRecorder *logger = new LogRecorder(seed);
	//debug
	create_path(args, logger, filename);
	load_path(args, logger, estimator, filename, &ref_cnt1, &ref_cnt2);
	write_map(logger, mapname);

	return logger;
}


void simulate(Argument& args, LocationEstimator* estimator, LogRecorder* logger)
{
	char tracename[256];	
	char outname[256];

	sprintf(tracename, "%s.trace", args.Basename);
	sprintf(outname, "%s.log", args.Basename);

	FILE *fp = fopen(outname, "w");
	fclose(fp);
	FILE *fptr = fopen(tracename, "w");
	fclose(fptr);

	double err1 = 0;
	double err2 = 0;
	size_t iterLogList;


	for (iterLogList = 0; iterLogList < logger->logList.size(); iterLogList++)
	{
		long timestamp = logger->logList[iterLogList].timestamp;

		for (int j = 0; j < logger->nBeacon; j++)
		{
			int lid = 0;		// not lid here, it is reflection count
			if (logger->logList[iterLogList].beaconList[j].vTemp.isValid) lid = 1;
			estimator->InsertHeardData(lid, timestamp, 
					logger->logList[iterLogList].beaconList[j].beacon_id,
					logger->logList[iterLogList].beaconList[j].distance);
		}
		
		Vector ori_point, slv_point1, slv_point2;
		ori_point = logger->logList[iterLogList].vPosition;

		if (args.SIM_ActiveEstimators & SIM::PROP_EST)
			slv_point1 = estimator->SolveEx();
		
//		if (args.SIM_ActiveEstimators & SIM::PREV_EST)
//			slv_point2 = estimator->Solve();				

		err1 += slv_point1.getDistance(ori_point);
		err2 += slv_point2.getDistance(ori_point);
		
		double instErr = slv_point1.getDistance(ori_point);
		fptr = fopen(tracename, "a");
		fprintf(fptr, "%d\t%f\t%d\n", (int)iterLogList, instErr,(int)slv_point1.temp);
		fclose(fptr);

		fprintf(stdout, "%d\t%f\t%f\n", (int)iterLogList, err1/(double)(iterLogList + 1), 0.0);
		slv_point1.Println();
//		getchar();
	}

	fp = fopen(outname, "a");
	fprintf(fp, "%f\t%f\t%f\n", err1/(double)iterLogList, err2/(double)iterLogList, (double)estimator->solver->cnt / (double)iterLogList);


	fprintf(fp, "N_steps             : %d\n", (int)iterLogList);
	fprintf(fp, "error(proposed)     : %f\n", err1 / (double)iterLogList);
	fprintf(fp, "error(original)     : %f\n", err2 / (double)iterLogList);
	fprintf(fp, "N_estimation(total) : %d\n", estimator->solver->cnt);
	fprintf(fp, "N_estimation(avg.)  : %f\n", estimator->solver->cnt / (double)iterLogList);

	args.analyzer->PrintAVG(fp);
	args.analyzer->PrintAVG(stdout);
//	args.analyzer->WriteCDF();
//	fprintf(fp, "Accepted_Distance_ratio : %f\n", args.analyzer->N_Selected.GetSum() / args.analyzer->
		
	fclose(fp);

	


}

void init_args(Argument& args, int argc, char** argv)
{
	char confname[256];

	if (argc > 1)
		strncpy(confname, argv[1], 255);
	else
		strcpy(confname, "conf.txt");

	args.LoadArgument(confname);	
	args.SetAnalyzer();

}

int main(int argc, char** argv)
{		
	Argument args;
	LocationEstimator *estimator;
	LogRecorder* logger;

	init_args(args, argc, argv);
	estimator = new LocationEstimator(args);
	logger = alloc_logger(args, estimator, 0);

	simulate(args, estimator, logger);

	delete logger;
	delete estimator;


	getchar();
	return 0;
}


