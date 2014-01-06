#pragma once
#include <stdio.h>
#include "BeaconList.h"
#include "PlaneList.h"
#include "Estimator.h"
#include "EventLog.h"


namespace SIM
{
	enum moveType
	{
		CW,
		CCW,
		RND,
		PD_CCW,	//Plane Detection mode. CCW
	};

	enum estimators
	{
		PREV_EST = 1 << 0,
		PROP_EST = 1 << 1,
	};
	typedef int estimator;
	
	enum deploy
	{
		AUTO,
		MANUAL,
	};
}

class Setting
{
public:
	Setting(void);
	~Setting(void);

	void LoadArgument(const char *filename);
	void SaveArgument(const char *filename);


	static void loadEstimatorArgument(const char *filename, EstimatorArgument *estimatorArgument);
	static void loadBeaconList(const char *filename, BeaconList *beacons);
	static void loadPlaneList(const char* filename, PlaneList *planes);
	static void loadEventLogList(const char* filename, EventLogList *events);

	void saveEstimatorArgument(const char *filename, EstimatorArgument *args);

	// arguments of environment
	int RoomWidth;		// in cm
	int RoomLength;		// in cm
	int RoomHeight;		// in cm



	// arguemnts for simulator
	double SIM_SpeedAvg;
	double SIM_SpeedDev;
	double SIM_AngularAvg;	
	double SIM_AngularDev;
	double SIM_EstimationErr;
	double SIM_EstimationDev;
	int SIM_TimeLimit;				// in ms
	int SIM_Iter;
	int SIM_AngleRange;				// in degree
	SIM::moveType SIM_MoveType;
	SIM::estimator SIM_ActiveEstimators;
	SIM::deploy SIM_BeaconDeploy;
	int SIM_NBeacon;				// number of beacons in auto deploy
	char* SIM_BeaconConfFile;
	int SIM_MaxMeasurement;			// maximum possible measurement value in cm
	int SIM_MinMeasurement;			// minimum possible measurement value in cm

	char Basename[255];

private:
	void defaultArgs();

	void loadSimulatorArgs(const char*filename);

	void saveSimulatorArgs(FILE *fp);

	bool checkConfigFile(const char *filename);

//	void setBaseName();
};

