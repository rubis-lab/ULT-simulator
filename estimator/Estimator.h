#pragma once
#include "Solver.h"
#include "BeaconList.h"
#include "PlaneList.h"
#include "FilterManager.h"

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



class EstimatorResult
{
public:
	EstimatorResult() 	{/**/}
	EstimatorResult(Vector location, double error):
		location(location), 
		error(error) 	{/**/}
	~EstimatorResult()	{/**/}
	
	Vector location;
	double error;
};


////////////////////////////
//EstimatorArgument 
class EstimatorArgument
{
public:
	EstimatorArgument()
	{
		setDefault();
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

	BeaconList *beacons;
	PlaneList *planes;

	void setCutThreshold()
	{
		cutThreshold = sqrt(pow((double)maxMeasError, 2) / validSize);
	}


};
////////////////////////////
	
class Estimator
{
public:

	Estimator();
	~Estimator();

	void reset();
	void destructor();

	void setEstimator(EstimatorArgument estimatorArgument);
	void measure(unsigned long timestamp, int userBid, double distance);
	EstimatorResult solve(long currentTime);



private:
	Solver solver;
	EstimatorArgument args;
	MeasurementList *measurementList;
	FilterManager *filterManager;

	SolverInput *input;
	
	
	Vector prevLocation;

	void setMeasurementList();
	void setSolver();
	void setInput();
	void setFilterManager();
	
	void optimize1(SolverResultList *results);



	

};
