#include <stdio.h>
#include <stdlib.h>
#include "SimulatorArgument.h"
#include "PlaneGenerator.h"
#include "BeaconDeploy.h"

#define SIM_REFLECTION_LIMIT 2

SimulatorArgument::SimulatorArgument()
{
	setDefault();
}

void SimulatorArgument::setDefault()
{
	width = 1000;
	length = 1000;
	height = 300;
	timeslot = 50;
	speedAvg = 0.5;
	speedDev = 0.025;
	angleAvg = 0.00;
	angleDev = 1.00;
	validAngleRange = 170;
	distanceNoiseAvg = 0.0;
	distanceNoiseDev = 1.5;
	maxMeasurableDistance = 600;
	minMeasurableDistance = 20;

	moveType = SIM_WALKER::RANDOM;
	deployType = SIM_BEACON::COULOMB;
	strcpy(beaconDeployFilename, "beacon.conf");
	beaconSize = 20;

	planeType = SIM_PLANE::CUBE;
	strcpy(planeFilename, "plane.conf");


	eventSize = 10000;
	randomSeed = 0;
	random.setSeed(randomSeed);
}

void SimulatorArgument::setPlane()
{
	PlaneGenerator planeGenerator;
	planeGenerator.generatePlane(this);
}

void SimulatorArgument::setBeacon()
{
	BeaconDeploy beaconDeploy;
	beaconDeploy.deployBeacons(this);
	beacons.applyPlanes(&planes, SIM_REFLECTION_LIMIT);
}

void SimulatorArgument::load(const char *filename)
{
	loadArgument(filename);
	//plane should be defined before calling setBeacon
	setPlane();
	setBeacon();
}

void SimulatorArgument::save(const char *filename)
{
	saveArgument(filename);
}

void SimulatorArgument::loadArgument(const char *filename)
{
	FILE* fp = fopen(filename, "r");
	if (fp == NULL)
	{
		printf("SimulatorArgument::load, cannot open configuration file %s\n", filename);
		getchar();
		exit(17);
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
		
		if (strcmp(arg, "sim_room_width") == 0)
			width = atoi(val);
		else if (strcmp(arg, "sim_room_length") == 0)
			length = atoi(val);
		else if (strcmp(arg, "sim_room_height") == 0)
			height = atoi(val);
		else if (strcmp(arg, "sim_timeslot") == 0)
			timeslot = atoi(val);
		else if (strcmp(arg, "sim_speed_avg") == 0)
			speedAvg = atof(val);
		else if (strcmp(arg, "sim_speed_dev") == 0)
			speedDev = atof(val);
		else if (strcmp(arg, "sim_angle_avg") == 0)
			angleAvg = atof(val);
		else if (strcmp(arg, "sim_angle_dev") == 0)
			angleDev = atof(val);
		else if (strcmp(arg, "sim_angle_range") == 0)
			validAngleRange = atoi(val);
		else if (strcmp(arg, "sim_distance_noise_avg") == 0)
			distanceNoiseAvg = atof(val);
		else if (strcmp(arg, "sim_distance_noise_dev") == 0)
			distanceNoiseDev = atof(val);
		else if (strcmp(arg, "sim_min_dist") == 0)
			minMeasurableDistance = atoi(val);
		else if (strcmp(arg, "sim_max_dist") == 0)
			maxMeasurableDistance = atoi(val);
		else if (strcmp(arg, "sim_movetype") == 0)
		{
			if (strcmp(val, "CW") == 0) moveType = SIM_WALKER::CLOCKWISE;
			else if (strcmp(val, "ACW") == 0) moveType = SIM_WALKER::ANTI_CLOCKWISE;
			else if (strcmp(val, "RND") == 0) moveType = SIM_WALKER::RANDOM;
			else if (strcmp(val, "PD") == 0) moveType = SIM_WALKER::PLANE_DETECTION;
			else printf("unknown sim_movetype : %s\n", val);
		}
		else if (strcmp(arg, "sim_beacon_deploy") == 0)
		{
			if (strcmp(val, "FILE") == 0) deployType = SIM_BEACON::BYFILE;
			else if (strcmp(val, "CIR") == 0) deployType = SIM_BEACON::CIRCULAR;
			else if (strcmp(val, "CIR2") == 0) deployType = SIM_BEACON::CIRCULAR2;
			else if (strcmp(val, "CLMB") == 0) deployType = SIM_BEACON::COULOMB;
			else if (strcmp(val, "CLMB2") == 0) deployType = SIM_BEACON::COULOMB2;
			else printf("unknow sim_beacon_deploy : %s\n", val);
		}
		else if (strcmp(arg, "sim_beacon_deploy_file") == 0)
			strncpy(beaconDeployFilename, val, 255);
		else if (strcmp(arg, "sim_n_beacon") == 0)
			beaconSize = atoi(val);
		else if (strcmp(arg, "sim_plane_type") == 0)
		{
			if (strcmp(val, "FILE") == 0) planeType = SIM_PLANE::BYFILE;
			if (strcmp(val, "CUBE") == 0) planeType = SIM_PLANE::CUBE;
		}
		else if (strcmp(arg, "sim_event_size") == 0)
			eventSize = atoi(val);
		else if (strcmp(arg, "sim_random_seed") == 0)
		{
			randomSeed = atoi(val);
			random.setSeed(randomSeed);
		}

		// exceptions
		else if (strncmp(arg, "sim_", 4) == 0)
			printf("SimulatorArgument::load, unkown argumnet \"%s\"\n", arg);
		
	}
	fclose(fp);
}

void SimulatorArgument::saveArgument(const char *filename)
{
	FILE* fp = fopen(filename, "w");

	if (fp == NULL)	
	{
		printf("SimulatorArgument::save, can't open file %s\n", filename);
		exit(18);
	}
	
	fprintf(fp, "sim_room_width = %d \t#\n", width);
	fprintf(fp, "sim_room_length = %d \t#\n", length);
	fprintf(fp, "sim_room_height = %d \t#\n", height);
	fprintf(fp, "sim_timeslot = %d \t#time interval of simulator\n", timeslot);

	fprintf(fp, "sim_speed_avg = %f \t#\n", speedAvg);
	fprintf(fp, "sim_speed_dev = %f \t#\n", speedDev);
	fprintf(fp, "sim_angle_avg = %f \t#\n", angleAvg);
	fprintf(fp, "sim_angle_dev = %f \t#\n", angleDev);
	fprintf(fp, "sim_angle_range = %d \t#\n", validAngleRange);
	fprintf(fp, "sim_distance_noise_avg = %f \t#\n", distanceNoiseAvg);
	fprintf(fp, "sim_distance_noise_dev = %f \t#\n", distanceNoiseDev);
	fprintf(fp, "sim_min_dist = %d \t#\n", minMeasurableDistance);
	fprintf(fp, "sim_max_dist = %d \t#\n", maxMeasurableDistance);
	
	fprintf(fp, "sim_movetype = ");
	switch(moveType)
	{
		case SIM_WALKER::CLOCKWISE : fprintf(fp, "CW"); break;
		case SIM_WALKER::ANTI_CLOCKWISE : fprintf(fp, "ACW"); break;
		case SIM_WALKER::RANDOM : fprintf(fp, "RND"); break;
		case SIM_WALKER::PLANE_DETECTION: fprintf(fp, "PD"); break;
	}
	fprintf(fp, " \t# CW, ACW, RND, PD\n");

	fprintf(fp, "sim_beacon_deploy = ");
	switch(deployType)
	{
		case SIM_BEACON::BYFILE : fprintf(fp, "FILE"); break;
		case SIM_BEACON::CIRCULAR : fprintf(fp, "CIR"); break;
		case SIM_BEACON::CIRCULAR2 : fprintf(fp, "CIR2"); break;
		case SIM_BEACON::COULOMB : fprintf(fp, "CLMB"); break;
		case SIM_BEACON::COULOMB2 : fprintf(fp, "CLMB2"); break;
		case SIM_BEACON::RANDOM : fprintf(fp, "RND"); break;
	}
	fprintf(fp, " \t# FILE, CIR, CIR2, CLMB, CLMB2, RND\n");

	fprintf(fp, "sim_beacon_deploy_file = %s \t# if deploy type is FILE, load from this file. otherwise wirte to this file\n", beaconDeployFilename);
	fprintf(fp, "sim_n_beacon = %d \t#number of beacons. if deploy type is FILE, this value will be ignored\n", beaconSize);

	fprintf(fp, "sim_plane_type = ");
	switch(planeType)
	{
		case SIM_PLANE::BYFILE : fprintf(fp, "FILE"); break;
		case SIM_PLANE::CUBE : fprintf(fp, "CUBE"); break;
	}
	fprintf(fp, " \t# FILE, CUBE\n");
	
	fprintf(fp, "sim_event_size = %d \t#number of event size\n", eventSize);
	fprintf(fp, "sim_random_seed = %d \t#random seed\n", randomSeed);

	fclose(fp);


}
