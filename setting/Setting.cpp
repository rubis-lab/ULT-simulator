#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "Setting.h"

#ifdef ENV_LINUX
#define _strdup strdup
#endif


double read_value(char* str)
{
	return atof(strtok(str, ";"));
}
long read_value_long(char* str)
{
	return atol(strtok(str, ";"));
}
char* read_header(char* str)
{
	return strtok(str, ";");
}

bool read_vector(char* str, Vector &v)
{
	char *buf;
	buf = strtok(str, ";#\r\n");
	if (buf == NULL) 
	{
		v = Vector(false);
		return false;
	}
	double x, y, z;

	x = atof(buf);
	y = atof(strtok(NULL, ";"));
	z = atof(strtok(NULL, ";"));
	
	v = Vector(x, y, z);

	return true;
}


long read_listener_info(FILE* fp, Vector &vPosition, Vector &vFace)
{
	const int buf_size = 1024;
	char buf[buf_size + 1];
	char *buf_header;

	if (fgets(buf, buf_size, fp) == NULL)
	{
		printf("incorrect file contents (exit codee :15)\n");
		getchar();
		exit(15);
	}
	buf_header = read_header(buf);
	
//	if (fgets(buf, buf_size, fp) == NULL) return -1;
	if (strcmp(buf_header, "endlog") == 0) return -1;
	if (strcmp(buf_header, "time") != 0)
	{
		printf("incorrect file contents (exit code : 10)\n");
		getchar();
		exit(10);
	}
	
	long timestamp;
	bool ret1, ret2;

	timestamp = read_value_long(NULL);
	
	ret1 = read_vector(NULL, vPosition);
	ret2 = read_vector(NULL, vFace);

	if (ret1 && ret2 == false)
	{
		printf("incorrect file contents (exit code : 11\n");
		getchar();
		exit(11);
	}

	return timestamp;
}



int read_reflection_cnt(FILE *fp, int* ref_cnt1, int *ref_cnt2)
{
	const int buf_size = 1024;
	char buf[buf_size + 1];

	fgets(buf, buf_size, fp);
	if (strcmp(read_header(buf), "ref1") != 0)
	{
		printf("incorrect file contents : no reflection1 info\n");
		getchar();
		exit(12);
	}
	*ref_cnt1 = (int)read_value(NULL);
	fgets(buf, buf_size, fp);
	if (strcmp(read_header(buf), "ref2") != 0)
	{
		printf("incorrect file contents : no reflection2 info\n");
		getchar();
		exit(13);
	}
	*ref_cnt2 = (int)read_value(NULL);

	return true;

	
}



Setting::Setting(void)
{
	defaultArgs();
}


Setting::~Setting(void)
{
}


void Setting::loadEstimatorArgument(const char *filename, EstimatorArgument *estimatorArgument)
{
	FILE* fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("loadEstimatorArgument: cannot open configuration file %s\n", filename);
		getchar();
		exit(1);
	}

	char buf[255];
	char arg[255];
	char val[255];
	char* tok;


	estimatorArgument->setDefault();

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
			estimatorArgument->timeSlot = atoi(val);
		else if (strcmp(arg, "env_n_cut") == 0)
			estimatorArgument->validSize = atoi(val);
		else if (strcmp(arg, "env_max_meas_err") == 0)
			estimatorArgument->maxMeasError = atoi(val);
		else if (strcmp(arg, "env_min_distance") == 0)
			estimatorArgument->minValidDistance = atoi(val);
		else if (strcmp(arg, "env_max_distance") == 0)
			estimatorArgument->maxValidDistance = atoi(val);
		else if (strcmp(arg, "env_timewindow") == 0)
			estimatorArgument->timeWindow = atoi(val);
		else if (strcmp(arg, "env_optimization") == 0)
			estimatorArgument->optimization = atoi(val);
		else if (strcmp(arg, "env_beacon_config") == 0)
			strcpy(estimatorArgument->beaconConfigFilename, val);
		else if (strcmp(arg, "env_plane_config") == 0)
			strcpy(estimatorArgument->planeConfigFilename, val);		
		else if (strcmp(arg, "env_mode") == 0)
		{
			if (atoi(val) == 0)
				estimatorArgument->estimatorMode = EST::TRADITIONAL;
			else
				estimatorArgument->estimatorMode = EST::PROPOSED1;
		}
		else if (strncmp(arg, "env_", 4) == 0)
			printf("loadEnvironment: unkown argumnet \"%s\"\n", arg);
	
//kf settings
		if (strcmp(arg, "kf_meas_err") == 0)
			estimatorArgument->kfMeasError = atof(val);
		else if (strcmp(arg, "kf_system_err") == 0)
			estimatorArgument->kfSystemError = atof(val);
		else if (strcmp(arg, "kf_mode") == 0)
		{
			if (strncmp(val, "PVA",3) == 0) estimatorArgument->kfMode = KF::PVA;
			else if (strncmp(val, "PV", 2) == 0) estimatorArgument->kfMode = KF::PV;
			else if (strncmp(val, "P", 1) == 0) estimatorArgument->kfMode = KF::P;
			else printf("unknown KF Mode : %s\n", val);		
		}
		else if (strncmp(arg, "kf_", 3) == 0)
			printf("loadEnvironment: unkown argumnet \"%s\"\n", arg);
	}
	fclose(fp);
}

void Setting::saveEstimatorArgument(const char *filename, EstimatorArgument *args)
{
	FILE* fp = fopen(filename, "w");

	if (fp == NULL)	
	{
		printf("Setting::saveEstimatorArgument, can't open file %s\n", filename);
		exit(16);
	}

	fprintf(fp, "env_timeslot = %d \t# in ms\n", (int)args->timeSlot);
	fprintf(fp, "env_n_cut = %d \t# N_V value\n", args->validSize);
	fprintf(fp, "env_max_meas_err = %d \t# maximum measurement error\n", args->maxMeasError);
	fprintf(fp, "env_min_distance = %d \t# minimum valid distance\n", args->minValidDistance);	
	fprintf(fp, "env_max_distance = %d \t# maximum valid distance\n", args->minValidDistance);	
	fprintf(fp, "env_timewindow = %d \t# measurements time window in ms \n", (int)args->timeWindow);	
	fprintf(fp, "env_optimization = %d \t# bitwise 'or' value of : selection opt (%d), threshold opt (%d), branch cut opt (%d), branch cut opt 2 (%d)\n", args->optimization, OPT::SELECTION, OPT::THRESHOLD, OPT::BRANCHCUT, OPT::BRANCHCUT_2);
	fprintf(fp, "env_beacon_config = %s \t# beacon config file\n", args->beaconConfigFilename);
	fprintf(fp, "env_plane_config = %s \t# plane config file\n", args->planeConfigFilename);

	fprintf(fp, "\n");

	fprintf(fp, "kf_meas_err = %f \t#\n", args->kfMeasError);
	fprintf(fp, "kf_system_err = %f \t#\n", args->kfSystemError);
	fprintf(fp, "kf_mode = ");
	switch(args->kfMode)
	{
	case KF::PVA : fprintf(fp, "PVA"); break;
	case KF::PV : fprintf(fp, "PV"); break;
	case KF::P : fprintf(fp, "P");break;
	}
	fprintf(fp, "\t# P, PV, PVA \n");
	fprintf(fp,"\n");
}


void Setting::loadBeaconList(const char* filename, BeaconList *beacons)
{
	beacons->reset();

	FILE *fp = fopen(filename, "r");

	if (fp == NULL)
	{
		printf("can't open beacon list file %s\n", filename);
		exit(1);
	}

	const int bufSize = 1024;
	char buf[bufSize + 1];


	while(fgets(buf, bufSize, fp) != NULL)
	{
		int bid;
		Vector location;

		bid = atoi(strtok(buf, ";"));

		location.x = atof(strtok(NULL, ";"));
		location.y = atof(strtok(NULL, ";"));
		location.z = atof(strtok(NULL, ";\r\n#"));

		beacons->addBeacon(bid, location);
	}
}


void Setting::loadPlaneList(const char* filename, PlaneList *planes)
{
	planes->reset();

	FILE *fp = fopen(filename, "r");

	if (fp == NULL)
	{
		printf("can't open plane list file %s\n", filename);
		exit(2);
	}

	const int bufSize = 1024;
	char buf[bufSize + 1];

	while(fgets(buf, bufSize, fp) != NULL)
	{
		planes->addPlane(Plane(buf));
	}


}

void Setting::loadEventLogList(const char* filename, EventLogList *events)
{
	FILE *fp;

	fp = fopen(filename, "r");

	if (fp == NULL)
	{
		printf("Setting::loadEventLogList, can't open file %s\n", filename);
		exit(3);
	}

	events->reset();

	const int bufSize = 4095;
	char buf[bufSize + 1];
	
	unsigned long timestamp;
	Vector location;
	Vector direction;

	int bid;
	double distance;
	Vector reflectedPoint1, reflectedPoint2;

	while ((timestamp = read_listener_info(fp, location, direction)) > 0)
	{
		events->setNewEvent(timestamp, location, direction);

		while(fgets(buf, bufSize, fp), strcmp(read_header(buf), "dist") == 0)
		{
			bid = (int) read_value(NULL);				
			distance = read_value(NULL);
			
			read_vector(NULL, reflectedPoint1);
			read_vector(NULL, reflectedPoint2);

			events->addMeasurement(bid, distance);
			events->setMeasurementReflectedPoints(reflectedPoint1, reflectedPoint2);
		}
	}	

}



void Setting::LoadArgument(const char *filename)
{
	defaultArgs();

	if (checkConfigFile(filename))
	{	
		loadSimulatorArgs(filename);
//		setBaseName();
	}
	SaveArgument("current_conf.txt");
}

bool Setting::checkConfigFile(const char *filename)
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
	

void Setting::SaveArgument(const char *filename)
{
	FILE *fp = fopen(filename, "w");
	if (fp == NULL)
	{
		printf("SaveArgument : cannot open file %s\n", filename);
		getchar();
		exit(3);
	}

	saveSimulatorArgs(fp);
	
	fclose(fp);
}



void Setting::defaultArgs()
{  
	RoomWidth = 1000;
	RoomLength = 1000;
	RoomHeight = 300;
	
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

//	setBaseName();
}


void Setting::loadSimulatorArgs(const char* filename)
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

void Setting::saveSimulatorArgs(FILE *fp)
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
/*
void Setting::setBaseName()
{
	sprintf(Basename, "room%dx%dx%d_s%dx%d_nb%02d_nc%02d_i%d_cth%d_mt%d_opt%d", 
			RoomWidth, RoomLength, RoomHeight,
			(int)(SIM_SpeedAvg * 10), (int)(SIM_EstimationDev * 10), 
			SIM_NBeacon, NCut, 
			SIM_Iter, (int)(CutThreshold*10), SIM_MoveType,
			Optimization);
}
*/

