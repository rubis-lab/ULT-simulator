#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "EstimatorArgument.h"


EstimatorArgument::EstimatorArgument()
{
	setDefault();
	planeApplied = false;
	random.setSeed(0);
}

EstimatorArgument::~EstimatorArgument()
{
}

void EstimatorArgument::setDefault()
{
	timeSlot = 50;
	validSize = 4;
	maxMeasError = 50;
	minValidDistance = 1;
	maxValidDistance = 700;
	minBeaconSize = 3;
	strictValidSize = false;
	timeWindow = 1000;
	optimization = OPT::NONE;
	estimatorMode = EST::TRADITIONAL;
	gatherData = false;

	kfMode = KF::PV;
	kfMeasError = 0.001;
	kfSystemError = 0.0008;

	strcpy(beaconConfigFilename, "beacon.conf");
	strcpy(planeConfigFilename, "plane.conf");
}


void EstimatorArgument::setCutThreshold()
{
	cutThreshold = sqrt(pow((double)maxMeasError, 2) / validSize);
}

void EstimatorArgument::applyPlanes()
{
	if (planeApplied) return;
	beacons.applyPlanes(&planes);
	planeApplied = true;
}

void EstimatorArgument::load(const char* filename)
{
	FILE* fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("EstimatorArgument::load, cannot open configuration file %s\n", filename);
		getchar();
		exit(15);
	}

	char buf[255];
	char arg[255];
	char val[255];
	char* tok;


	setDefault();

	while (fgets(buf, 255, fp) != NULL)
	{
		tok = strtok(buf, " =");
		if (tok == NULL) continue;
		strcpy(arg, tok);
		tok = strtok(NULL, " =#\t\n\r");
		if (tok == NULL) continue;
		strcpy(val, tok);
		
//env settings
		if (strcmp(arg, "env_timeslot") == 0)
			timeSlot = atoi(val);
		else if (strcmp(arg, "env_n_cut") == 0)
			validSize = atoi(val);
		else if (strcmp(arg, "env_max_meas_err") == 0)
			maxMeasError = atoi(val);
		else if (strcmp(arg, "env_min_distance") == 0)
			minValidDistance = atoi(val);
		else if (strcmp(arg, "env_max_distance") == 0)
			maxValidDistance = atoi(val);
		else if (strcmp(arg, "env_timewindow") == 0)
			timeWindow = atoi(val);
		else if (strcmp(arg, "env_optimization") == 0)
			optimization = atoi(val);
		else if (strcmp(arg, "env_beacon_config") == 0)
		{
			strcpy(beaconConfigFilename, val);
			beacons.load(val);
		}
		else if (strcmp(arg, "env_plane_config") == 0)
		{
			strcpy(planeConfigFilename, val);
			planes.load(val);
		}
		else if (strcmp(arg, "env_mode") == 0)
		{
			if (atoi(val) == 0)
				estimatorMode = EST::TRADITIONAL;
			else
				estimatorMode = EST::PROPOSED1;
		}
		else if (strncmp(arg, "env_", 4) == 0)
			printf("loadEnvironment: unkown argumnet \"%s\"\n", arg);
	
//kf settings
		if (strcmp(arg, "kf_meas_err") == 0)
			kfMeasError = atof(val);
		else if (strcmp(arg, "kf_system_err") == 0)
			kfSystemError = atof(val);
		else if (strcmp(arg, "kf_mode") == 0)
		{
			if (strncmp(val, "PVA",3) == 0) kfMode = KF::PVA;
			else if (strncmp(val, "PV", 2) == 0) kfMode = KF::PV;
			else if (strncmp(val, "P", 1) == 0) kfMode = KF::P;
			else printf("unknown KF Mode : %s\n", val);		
		}
		else if (strncmp(arg, "kf_", 3) == 0)
			printf("loadEstimatorArgument: unkown argumnet \"%s\"\n", arg);
	}
	fclose(fp);
}

void EstimatorArgument::save(const char *filename)
{
	FILE* fp = fopen(filename, "w");

	if (fp == NULL)	
	{
		printf("EstimatorArgument::save, can't open file %s\n", filename);
		exit(16);
	}

	fprintf(fp, "env_timeslot = %d \t# in ms\n", (int)timeSlot);
	fprintf(fp, "env_n_cut = %d \t# N_V value\n", validSize);
	fprintf(fp, "env_max_meas_err = %d \t# maximum measurement error\n", maxMeasError);
	fprintf(fp, "env_min_distance = %d \t# minimum valid distance\n", minValidDistance);	
	fprintf(fp, "env_max_distance = %d \t# maximum valid distance\n", minValidDistance);	
	fprintf(fp, "env_timewindow = %d \t# measurements time window in ms \n", (int)timeWindow);	
	fprintf(fp, "env_optimization = %d \t# bitwise 'or' value of : selection opt (%d), threshold opt (%d), branch cut opt (%d), branch cut opt 2 (%d)\n", optimization, OPT::SELECTION, OPT::THRESHOLD, OPT::BRANCHCUT, OPT::BRANCHCUT_2);
	fprintf(fp, "env_beacon_config = %s \t# beacon config file\n", beaconConfigFilename);
	fprintf(fp, "env_plane_config = %s \t# plane config file\n", planeConfigFilename);
	fprintf(fp, "env_mode = ");
	if (estimatorMode == EST::TRADITIONAL)
		fprintf(fp, "0");
	if (estimatorMode == EST::PROPOSED1)
		fprintf(fp, "1");
	fprintf(fp, " \t# estimator mode. traditional(0), proposed(1)\n");

	fprintf(fp, "\n");

	fprintf(fp, "kf_meas_err = %f \t#\n", kfMeasError);
	fprintf(fp, "kf_system_err = %f \t#\n", kfSystemError);
	fprintf(fp, "kf_mode = ");
	switch(kfMode)
	{
	case KF::PVA : fprintf(fp, "PVA"); break;
	case KF::PV : fprintf(fp, "PV"); break;
	case KF::P : fprintf(fp, "P");break;
	}
	fprintf(fp, "\t# P, PV, PVA \n");
	fprintf(fp,"\n");

	fclose(fp);
}
