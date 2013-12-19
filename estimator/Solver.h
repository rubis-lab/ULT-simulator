#pragma once
#include "Vector.h"
#include "Measurement.h"
#include <vector>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_linalg.h>

//#define REFLECTION_LIMIT 1 
// REFLECTOIN_LIMIT is defined in HeardList.h
#define LSQ_THRESHOLD 100
#define MAX_HISTORY 1024
#define MAX_VAL 99999


class SolverResult;


class SolverCondition
{
public:
	SolverCondition()
	{
		gatherData = false;
		cutBranch1 = true;
		cutBranch2 = false;
		solveNaive = true;
		maxMeasError = 50.0;
	}
	~SolverCondition();

	bool cutBranch1;
	bool cutBranch2;
	bool gatherData;		// if true, solver doesn't cut branch. It calculates how much leaves would be cut.kkk
	bool solveNaive;
	double maxMeasError;
};


#define MAX_FLAG_NUM 10
class SolverInput
{
public:
	SolverInput(MeasurementList *measurementList, unsigned long timeWindow = 1000);
	~SolverInput();
	MeasurementList *measurementList;
	std::vector<Measurement*> measurements;
	Vector initLocation;

	void setup(unsigned long currentTime, Vector previousLocation);
	void setInvalidButGather(int flag);
	void resetGatherFlags();
private:
	unsigned long timeWindow;
	bool invalidButGather[MAX_FLAG_NUM];
};

class SolverResult
{
public:
	SolverResult();
	~SolverResult();

	class Result
	{
	public:	
		Result() : isEmpty(true) {}
		Result(Vector location, double error, std::vector<Measurement*> snapshot) :
				location(location), 
				error(error), 
				snapshot(snapshot), 
				cutThreshold(false), 
				isEmpty(false)
		{
		}
		Vector location;
		double error;		//squared error
		std::vector<Measurement*> snapshot;
		bool cutThreshold;
		bool isEmpty;
		bool isValid()
		{
			if (cutThreshold || isEmpty) return false;
			return true;
		}

		void setCorrectedLocation(Vector vCorrect)
		{
			correctedLocation = vCorrect;
			distErrorCorrected = vCorrect.getDistance(location);
		}
		void setPredictedLocation(Vector vPredict)
		{
			predictedLocation = vPredict;
			distErrorPredicted = vPredict.getDistance(location);
		}
		double getPredictedDistanceError()
		{
			return distErrorPredicted;
		}
	private:
		Vector correctedLocation;
		Vector predictedLocation;
		double distErrorCorrected;
		double distErrorPredicted;

	};


	void addResult(SolverInput *input, Vector location);
	void cutThreshold(double thresholdErrorSquare);
	size_t size();
	bool isValid(int idx);
	Vector getLocation(int idx);
	Result* at(int idx);

	Result getFilteredResult();
	void setFilteredResult(Result result);

private:
	double getError(SolverInput *input, Vector location);
	std::vector<Result> results;

	Result filteredResult;

};

class Solver
{
public:
	Solver();
	~Solver();

	void solve(SolverInput *input, SolverResult *result);

	void setSolverCondition(SolverCondition condition);

	
private:
	SolverCondition condition;
	PlaneList *planes;
	BeaconList *beacons;


	void solveNaive(SolverInput *input, SolverResult *result);
	void solveWithPlanes(SolverInput *input, SolverResult *result, int currentIdx = 0);
	Vector NLLeastSquareSolver(SolverInput *input);
	Vector LeastSquareSolver2D(SolverInput *input);

	// optimization method 1. JH's method using distance of two beacons
	bool checkCutBranch1(SolverInput *input, int currentIdx);
	bool checkBranchCutoff(Measurement* meas1, Measurement* meas2);

	// optimization method 2. Using distances of three beacons	
	bool checkCutBranch2(SolverInput *input, int currentIdx);


	
};

