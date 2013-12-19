#pragma once
#include "Solver.h"

#include "BeaconList.h"
#include "PlaneList.h"

namespace OPT
{
	enum options
	{
		NONE = 			0x00;
		SELECTION = 	0x01,
		THRESHOLD = 	0x02,
		BRANCHCUT = 	0x04;
		BRANCHCUT_2 =	0x08,
	};
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

namespace EST
{
	enum mode
	{
		TRADITIONAL,
		PROPOSED1,
	};
}


class EstimatorResult
{
public:
	EstimatorResult();
	~EsitmatorResult();
	
	Vector location;
	double error;
};


	
class Estimator
{
public:
////////////////////////////
//EstimatorArgument 
	class EstimatorArgument
	{
	public:
		EstimatorArguement()
		{
			setDefault();
		}
		~EstimatorArguement();
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
			kfMode = KF::PV;
			estimatorMode = EST::TRADITIONAL;

			gatherData = false;
			
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

		
		OPT::options optimization;
		KF::mode kfMode;
		EST::mode estimatorMode;

		BeaconList *beacons;
		PlaneList *planes;

		void setCutThreshold()
		{
			cutThreshold = sqrt(pow((double)maxMeasError, 2) / validSize);
		}


	};
////////////////////////////

	Estimator();
	~Estimator();

	void reset();

	void setEstimator(EstimatorArgument estimatorArgument);
	void setSolver();
	void measure(int userBid, unsigned long timestamp, double distance);
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



	

};
