#include <stdio.h>
#include <stdlib.h>
#include "EventGenerator.h"
#include "Estimator.h"

int main(int argc, char** argv)
{
	SimulatorArgument simArgs;
	EventGenerator eventGenerator;

	printf("load...");
	simArgs.load("sim_conf.txt");
	printf("done\n");

	
	

	printf("generating event ...\n");
	eventGenerator.generateEvent(&simArgs);
	printf("done\n");
	printf("save event ...");
	eventGenerator.save("event_test.dat");
	printf("done\n");

}
