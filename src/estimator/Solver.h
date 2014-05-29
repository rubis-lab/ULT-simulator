#pragma once
#include <vector>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_linalg.h>
#include "Vector.h"
#include "Measurement.h"
#include "Analyzer.h"

//#define REFLECTION_LIMIT 1 
// REFLECTOIN_LIMIT is defined in HeardList.h
#define LSQ_THRESHOLD 100
#define MAX_HISTORY 1024
#define MAX_VAL 99999


class SolverResultList;


class SolverCondition
{
public:
	SolverCondition()
	{
		gatherData = false;
		cutBranch1 = true;		// this value will be ignored in naive mode
		cutBranch2 = false;
		solveNaive = true;
		maxMeasError = 50.0;
		minBeaconSize = 3;
	}
	~SolverCondition(){}

	bool cutBranch1;
	bool cutBranch2;
	bool gatherData;		// if true, solver doesn't cut branch. It calculates how much leaves would be cut.
	bool solveNaive;
	double maxMeasError;

	int minBeaconSize;

	Analyzer *analyzer;
};


#define MAX_FLAG_NUM 10
class SolverInput
{
public:
	SolverInput(MeasurementList *measurementList);
	~SolverInput();
	MeasurementList *measurementList;
	std::vector<Measurement*> measurements;
	Vector initLocation;

	void setup(unsigned long currentTime, Vector previousLocation);
	void setInvalidButGather(int flag);
	void resetGatherFlags();
	double getError(Vector location);
private:
	bool invalidButGather[MAX_FLAG_NUM];
	
};

class SolverResult
{
public:	
	SolverResult();
	SolverResult(Vector location, std::vector<Measurement*> &measurements);
	SolverResult(Vector location, std::vector<MeasurementInstance> &snapshot);
	
	Vector location;
	double error;		//squared error
	std::vector<MeasurementInstance> snapshot;
	bool overThreshold;
	bool isEmpty;
	bool isInside;


	bool isValid();
	void setCorrectedLocation(Vector vCorrect);
	void setPredictedLocation(Vector vPredict);
	Vector getCorrectedLocation();
	Vector getPredictedLocation();


	double getPredictedDistanceError();
	double getError(Vector location);
	double getError();
	double getCorrectedError();
	double getPredictedError();
	

private:
	Vector correctedLocation;
	Vector predictedLocation;
	double distErrorCorrected;
	double distErrorPredicted;
};

class SolverResultList
{
public:
	SolverResultList();
	~SolverResultList();

	void addResult(Vector location, SolverInput *input);
	void addResult(Vector location, std::vector<MeasurementInstance> &snapshot);
	void addResult(SolverResult &result);
	size_t size();
	size_t validSize();
	bool isValid(int idx);
	Vector getLocation(int idx);
	SolverResult* at(int idx);
	void setFail(SolverInput* input);
	bool isFail();
	
	SolverResult getFilteredResult();
	SolverResult getFirstResult();
	void setFilteredResult(SolverResult result);

public:
	int nOverThreshold;
	int nOutside;

private:
	std::vector<SolverResult> results;

	SolverResult filteredResult;
	bool fail;

};

class Solver
{
public:
	Solver();
	~Solver();

	void solve(SolverInput *input, SolverResultList *results);

	void setSolverCondition(SolverCondition condition);

	
private:
	SolverCondition condition;


	void solveNaive(SolverInput *input, SolverResultList *results);
	void solveWithPlanes(SolverInput *input, SolverResultList *results, int currentIdx = 0);
	Vector NLLeastSquareSolver(SolverInput *input);
	Vector LeastSquareSolver2D(SolverInput *input);

	// optimization method 1. JH's method using distance of two beacons
	bool checkCutBranch1(SolverInput *input, int currentIdx);
	bool checkBranchCutoff(Measurement* meas1, Measurement* meas2);

	// optimization method 2. Using distances of three beacons	
	bool checkCutBranch2(SolverInput *input, int currentIdx);


	
};

