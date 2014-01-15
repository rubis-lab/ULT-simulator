#pragma once
#include <stdio.h>
#include "Analyzer.h"

namespace OPT
{
	enum mode
	{
		SELECTION = 1 << 0,
		THRESHOLD = 1 << 1,
		BRANCHCUT = 1 << 2,
		BRANCHCUT_2 = 1 << 3,
		ANGLELIMIT = 1 << 4,
	};
	typedef int options;
}

namespace KF
{
	enum mode
	{
		P,
		PV,
		PVA,
	};
}

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

class Argument
{
public:
	Argument(void);
	~Argument(void);

	void LoadArgument(const char *filename);
	void SaveArgument(const char *filename);
	void SetAnalyzer(Analyzer* analyzer);
	void SetAnalyzer();

	// arguments of environment
	int RoomWidth;		// in cm
	int RoomLength;		// in cm
	int RoomHeight;		// in cm
	long TimeSlot;		// in ms
	int NCut;			//NV
	int MaxMeasError;	// in cm
	double CutThreshold;
	OPT::options Optimization;	// OPT_SELECTION, OPT_THRESHOLD, OPT_BRANCHCUT
	char BeaconConfigFilename[255];
	char PlaneConfigFilename[255];
	int MinValidDistance;
	int MaxValidDistance;


	// arguments of Kalman Filter
	double KF_MeasError;
	double KF_SystemError;
	double KF_SpeedError;
	double KF_AccError;
	KF::mode KF_Mode;

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

	Analyzer *analyzer;
	
private:
	void defaultArgs();

	void loadEnvironmentArgs(const char* filename);
	void loadKalmanFilterArgs(const char* filename);
	void loadSimulatorArgs(const char*filename);

	void saveEnvironmentArgs(FILE *fp);
	void saveKalmanFilterArgs(FILE *fp);
	void saveSimulatorArgs(FILE *fp);

	bool checkConfigFile(const char *filename);

	void setBaseName();
	void setCutThreshold();
};

