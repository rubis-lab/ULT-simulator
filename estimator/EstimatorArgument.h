#pragma once
#include "BeaconList.h"
#include "PlaneList.h"
#include "KFArgument.h"

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
	EstimatorArgument()
	{
		setDefault();
		planeApplied = false;
	}
	~EstimatorArgument(){}
	void setDefault()
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

	void setCutThreshold()
	{
		cutThreshold = sqrt(pow((double)maxMeasError, 2) / validSize);
	}

	void applyPlanes()
	{
		if (planeApplied) return;
		beacons.applyPlanes(&planes);
		planeApplied = true;
	}


private:
	bool planeApplied;


};
////////////////////////////
