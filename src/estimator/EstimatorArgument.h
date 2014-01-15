#pragma once
#include "BeaconList.h"
#include "PlaneList.h"
#include "KFArgument.h"
#include "Random.h"
#include "Analyzer.h"

namespace OPT
{
	enum options
	{
		NONE = 			0x00,
		SELECTION = 	0x01,
		THRESHOLD = 	0x02,
		BRANCHCUT = 	0x04,
		BRANCHCUT_2 =	0x08,
	};
}

namespace EST
{
	enum mode
	{
		TRADITIONAL,
		KFONLY,
		PROPOSED1,
	};
}

class EstimatorArgument
{
public:
	EstimatorArgument();
	~EstimatorArgument();
	void setDefault();
	int lid;			// each listener has its own estimator
	long timeSlot;		// in ms
	int validSize;		// N_V
	int maxMeasError;	// in cm
	int minValidDistance;
	int maxValidDistance;
	int minBeaconSize;
	bool strictValidSize;
	long timeWindow;	// in ms. measurements later than current time - timeWindow will be valid

	bool gatherData;
	double cutThreshold;

		
	int optimization;
	EST::mode estimatorMode;

	KF::mode kfMode;
	double kfMeasError;
	double kfSystemError;
	

	char beaconConfigFilename[255];
	char planeConfigFilename[255];

	BeaconList beacons;
	PlaneList planes;

	Random random;

	Analyzer analyzer;

	void setCutThreshold();
	void applyPlanes();
	void load(const char* filename);
	void save(const char* filename);


private:
	bool planeApplied;


};
////////////////////////////
