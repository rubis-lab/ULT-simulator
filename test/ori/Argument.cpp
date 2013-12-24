#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "Argument.h"

#ifdef ENV_LINUX
#define _strdup strdup
#endif

Argument::Argument(void)
{
	defaultArgs();
	analyzer = NULL;
	printf("analyzer is created\n");
}


Argument::~Argument(void)
{
//	delete analyzer;
}

void Argument::LoadArgument(const char *filename)
{
	defaultArgs();

	if (checkConfigFile(filename))
	{	
		loadEnvironmentArgs(filename);
		loadKalmanFilterArgs(filename);
		loadSimulatorArgs(filename);
		setBaseName();
	}
	SaveArgument("current_conf.txt");
}

bool Argument::checkConfigFile(const char *filename)
{
	FILE* fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("configuration file %s not exist, default config file %s.default is created.\n", filename, filename);
		char buf[255];
		sprintf(buf, "%s.default", filename);
		SaveArgument(buf);
		return false;
	}
	fclose(fp);
	return true;
}
	

void Argument::SaveArgument(const char *filename)
{
	FILE *fp = fopen(filename, "w");
	if (fp == NULL)
	{
		printf("SaveArgument : cannot open file %s\n", filename);
		getchar();
		exit(3);
	}

	saveEnvironmentArgs(fp);
	saveKalmanFilterArgs(fp);
	saveSimulatorArgs(fp);
	
	fclose(fp);
}



void Argument::defaultArgs()
{  
	RoomWidth = 1000;
	RoomLength = 1000;
	RoomHeight = 300;
	TimeSlot = 50;	
	NCut = 4;
	MaxMeasError = 50;
	Optimization = OPT::SELECTION | OPT::THRESHOLD;
	strcpy(BeaconConfigFilename,"beacon.conf");
	strcpy(PlaneConfigFilename, "plane.conf");
	setCutThreshold();
	MinValidDistance = 10;
	MaxValidDistance = 1000;
	
	KF_MeasError = 0.001;
	KF_SystemError = 0.0008;
	KF_SpeedError = 1.5;
	KF_AccError = 1200;
	KF_Mode = KF::PV;

	SIM_SpeedAvg = 0.5;
	SIM_SpeedDev = 0.025;
	SIM_AngularAvg = 0.0;
	SIM_AngularDev = 1.0;
	SIM_EstimationErr = 0.0;
	SIM_EstimationDev = 1.5;
	SIM_TimeLimit = 50 * 2000;
	SIM_Iter = 1;
	SIM_AngleRange = 160;
	SIM_MoveType = SIM::RND;
	SIM_ActiveEstimators = SIM::PREV_EST | SIM::PROP_EST;
	SIM_BeaconDeploy = SIM::AUTO;
	SIM_NBeacon = 9;
	SIM_BeaconConfFile = NULL;
	SIM_MinMeasurement = 20;
	SIM_MaxMeasurement = 600;

	setBaseName();
}

void Argument::loadEnvironmentArgs(const char* filename)
{
	FILE* fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("LoadEnvironment: cannot open configuration file %s\n", filename);
		getchar();
		exit(1);
	}

	char buf[255];
	char arg[255];
	char val[255];
	char* tok;

	while (fgets(buf, 255, fp) != NULL)
	{
		tok = strtok(buf, " =");
		if (tok == NULL) continue;
		strcpy(arg, tok);
		tok = strtok(NULL, " =#\t\n\r");
		if (tok == NULL) continue;
		strcpy(val, tok);
		
		if (strcmp(arg, "env_width") == 0)
			RoomWidth = atoi(val);
		else if (strcmp(arg, "env_length") == 0)
			RoomLength = atoi(val);
		else if (strcmp(arg, "env_height") == 0)
			RoomHeight = atoi(val);
		else if (strcmp(arg, "env_timeslot") == 0)
			TimeSlot = atoi(val);
		else if (strcmp(arg, "env_n_cut") == 0)
			NCut = atoi(val);
		else if (strcmp(arg, "env_max_meas_err") == 0)
			MaxMeasError = atoi(val);
		else if (strcmp(arg, "env_optimization") == 0)
			Optimization = atoi(val);
		else if (strcmp(arg, "env_beacon_config") == 0)
			strcpy(BeaconConfigFilename, val);
		else if (strcmp(arg, "env_plane_config") == 0)
			strcpy(PlaneConfigFilename, val);		
		else if (strcmp(arg, "env_min_distance") == 0)
			MinValidDistance = atoi(val);

		else if (strncmp(arg, "env_", 4) == 0)
			printf("LoadEnvironment: unkown argumnet \"%s\"\n", arg);
	}
	setCutThreshold();
	
	fclose(fp);
}

void Argument::saveEnvironmentArgs(FILE *fp)
{
	fprintf(fp, "env_width = %d \t# in cm\n", RoomWidth);
	fprintf(fp, "env_length = %d \t# in cm\n", RoomLength);
	fprintf(fp, "env_height = %d \t# in cm\n", RoomHeight);
	fprintf(fp, "env_timeslot = %d \t# in ms\n", (int)TimeSlot);
	fprintf(fp, "env_n_cut = %d \t# N_V value\n", NCut);
	fprintf(fp, "env_max_meas_err = %d \t# maximum measurement error\n", MaxMeasError);
	fprintf(fp, "env_optimization = %d \t# bitwise 'or' value of : selection opt (%d), threshold opt (%d), branch cut opt (%d), branch cut opt 2 (%d)\n", Optimization, OPT::SELECTION, OPT::THRESHOLD, OPT::BRANCHCUT, OPT::BRANCHCUT_2);
	fprintf(fp, "env_beacon_config = %s \t# beacon config file\n", BeaconConfigFilename);
	fprintf(fp, "env_plane_config = %s \t# plane config file\n", PlaneConfigFilename);
	fprintf(fp, "env_min_distance = %d \t# minimum valid distance\n", MinValidDistance);	

	fprintf(fp, "\n");
}
	


void Argument::loadKalmanFilterArgs(const char* filename)
{
	FILE* fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("LoadKF: cannot open configuration file %s\n", filename);
		getchar();
		exit(1);
	}

	char buf[255];
	char arg[255];
	char val[255];
	char* tok;

	while (fgets(buf, 255, fp) != NULL)
	{
		tok = strtok(buf, " =");
		if (tok == NULL) continue;
		strcpy(arg, tok);
		tok = strtok(NULL, " =#\t\n\r");
		if (tok == NULL) continue;
		strcpy(val, tok);
		
		if (strcmp(arg, "kf_meas_err") == 0)
			KF_MeasError = atof(val);
		else if (strcmp(arg, "kf_system_err") == 0)
			KF_SystemError = atof(val);
		else if (strcmp(arg, "kf_speed_err") == 0)
			KF_SpeedError = atof(val);
		else if (strcmp(arg, "kf_acc_err") == 0)
			KF_AccError = atof(val);
		else if (strcmp(arg, "kf_mode") == 0)
		{
			if (strncmp(val, "PVA",3) == 0) KF_Mode = KF::PVA;
			else if (strncmp(val, "PV", 2) == 0) KF_Mode = KF::PV;
			else if (strncmp(val, "P", 1) == 0) KF_Mode = KF::P;
			else printf("unknown KF Mode : %s\n", val);		
		}
		else if (strncmp(arg, "kf_", 3) == 0)
			printf("LoadKF: unkown argumnet \"%s\"\n", arg);
	}
	fclose(fp);
}

void Argument::saveKalmanFilterArgs(FILE *fp)
{
	fprintf(fp, "kf_meas_err = %f \t#\n", KF_MeasError);
	fprintf(fp, "kf_system_err = %f \t#\n", KF_SystemError);
	fprintf(fp, "kf_speed_err = %f \t#\n", KF_SpeedError);
	fprintf(fp, "kf_acc_err = %f \t#\n", KF_AccError);
	fprintf(fp, "kf_mode = ");
	switch(KF_Mode)
	{
	case KF::PVA : fprintf(fp, "PVA"); break;
	case KF::PV : fprintf(fp, "PV"); break;
	case KF::P : fprintf(fp, "P");break;
	}
	fprintf(fp, "\t# P, PV, PVA \n");
	fprintf(fp,"\n");
}

void Argument::loadSimulatorArgs(const char* filename)
{
	FILE* fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("LoadKF: cannot open configuration file %s\n", filename);
		getchar();
		exit(1);
	}

	char buf[255];
	char arg[255];
	char val[255];
	char* tok;

	while (fgets(buf, 255, fp) != NULL)
	{
		tok = strtok(buf, " =");
		if (tok == NULL) continue;
		strcpy(arg, tok);
		tok = strtok(NULL, " =#\t\n\r");
		if (tok == NULL) continue;
		strcpy(val, tok);
		
		if (strcmp(arg, "sim_speed_avg") == 0)
			SIM_SpeedAvg = atof(val);
		else if (strcmp(arg, "sim_speed_dev") == 0)
			SIM_SpeedDev = atof(val);
		else if (strcmp(arg, "sim_angle_avg") == 0)
			SIM_AngularAvg = atof(val);
		else if (strcmp(arg, "sim_angle_dev") == 0)
			SIM_AngularDev = atof(val);
		else if (strcmp(arg, "sim_estimation_error") == 0)
			SIM_EstimationErr = atof(val);
		else if (strcmp(arg, "sim_estimation_dev") == 0)
			SIM_EstimationDev = atof(val);
		else if (strcmp(arg, "sim_time_limit") == 0)
			SIM_TimeLimit = atoi(val);
		else if (strcmp(arg, "sim_iteration") == 0)
			SIM_Iter = atoi(val);
		else if (strcmp(arg, "sim_angle_range") == 0)
			SIM_AngleRange = atoi(val);
		else if (strcmp(arg, "sim_movetype") == 0)
		{
			if (strcmp(val, "CW") == 0) SIM_MoveType = SIM::CW;
			else if (strcmp(val, "CCW") == 0) SIM_MoveType = SIM::CCW;
			else if (strcmp(val, "RND") == 0) SIM_MoveType = SIM::RND;
			else if (strcmp(val, "PD_CCW") == 0) SIM_MoveType = SIM::PD_CCW;
			else printf("unknown sim_movetype : %s\n", val);
		}
		else if (strcmp(arg, "sim_naive_only") == 0)		
			SIM_ActiveEstimators = SIM::PREV_EST;				
		else if (strcmp(arg, "sim_n_beacon") == 0)
			SIM_NBeacon = atoi(val);
		else if (strcmp(arg, "sim_beacon_deploy") == 0)
		{
			if (strcmp(val, "AUTO") == 0) 
			{
				SIM_BeaconDeploy = SIM::AUTO;
				SIM_BeaconConfFile = NULL;
			}
			else
			{
				SIM_BeaconDeploy = SIM::MANUAL;
				SIM_BeaconConfFile = _strdup(val);
				printf("%s\n", SIM_BeaconConfFile);
			}
		}
		else if (strcmp(arg, "sim_min_meas") == 0)
			SIM_MinMeasurement = atoi(val);
		else if (strcmp(arg, "sim_max_meas") == 0)
			SIM_MaxMeasurement = atoi(val);

			
		// exceptions
		else if (strncmp(arg, "sim_", 4) == 0)
			printf("LoadKF: unkown argumnet \"%s\"\n", arg);
		
	}
	printf("sim_max_meas = %d\n", SIM_MaxMeasurement);
	fclose(fp);
}

void Argument::saveSimulatorArgs(FILE *fp)
{
	fprintf(fp, "sim_speed_avg = %f\t#\n", SIM_SpeedAvg);
	fprintf(fp, "sim_speed_dev = %f\t#\n", SIM_SpeedDev);	
	fprintf(fp, "sim_angle_avg = %f\t#\n", SIM_AngularAvg);
	fprintf(fp, "sim_angle_dev = %f\t#\n", SIM_AngularDev);
	fprintf(fp, "sim_estimation_error = %f\t#\n", SIM_EstimationErr);
	fprintf(fp, "sim_estimation_dev = %f\t#\n", SIM_EstimationDev);
	fprintf(fp, "sim_time_limit = %d\t#\n", SIM_TimeLimit);
	fprintf(fp, "sim_iteration = %d\t#\n", SIM_Iter);
	fprintf(fp, "sim_angle_range = %d\t#\n", SIM_AngleRange);
	fprintf(fp, "sim_movetype = ");
	switch(SIM_MoveType)
	{
	case SIM::CW : fprintf(fp, "CW"); break;
	case SIM::CCW : fprintf(fp, "CCW"); break;
	case SIM::RND : fprintf(fp, "RND"); break;
	case SIM::PD_CCW : fprintf(fp, "PD_CCW"); break;
	}
	fprintf(fp, "\t#CW (clockwise), CCW (counter clockwise), RND (random), PD_CCW (plane detection CCW)\n");
	fprintf(fp, "sim_n_beacon = %d\t#\n", SIM_NBeacon);
	fprintf(fp, "sim_beacon_deploy = ");
	if (SIM_BeaconDeploy == SIM::AUTO)
		fprintf(fp, "AUTO");
	else
		fprintf(fp, "%s", SIM_BeaconConfFile);
	fprintf(fp, "\n");
	fprintf(fp, "sim_min_meas = %d\t#minimum valid measurement in cm\n", SIM_MinMeasurement);
	fprintf(fp, "sim_max_meas = %d\t#maximum valid measurement in cm\n", SIM_MaxMeasurement);

	fprintf(fp, "\n");
}


void Argument::setBaseName()
{
	sprintf(Basename, "room%dx%dx%d_s%dx%d_nb%02d_nc%02d_i%d_cth%d_mt%d_opt%d", 
			RoomWidth, RoomLength, RoomHeight,
			(int)(SIM_SpeedAvg * 10), (int)(SIM_EstimationDev * 10), 
			SIM_NBeacon, NCut, 
			SIM_Iter, (int)(CutThreshold*10), SIM_MoveType,
			Optimization);
}

void Argument::setCutThreshold()
{
	CutThreshold = sqrt(pow((double)MaxMeasError, 2) * NCut);
}

void Argument::SetAnalyzer()
{
	analyzer = new Analyzer();
}

void Argument::SetAnalyzer(Analyzer *analyzer)
{
	this->analyzer = analyzer;
}