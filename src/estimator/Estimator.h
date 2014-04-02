#pragma once
#include "Solver.h"
#include "FilterManager.h"
#include "EstimatorArgument.h"




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
	int receptionLevel; 	// 0 : enough, 1 : smaller than N_V, 2 : unable to estimate
};


	
class Estimator
{
public:

	Estimator();
	~Estimator();

	void reset();
	void destructor();

	void setEstimator(EstimatorArgument *estimatorArgument);
	void measure(unsigned long timestamp, int userBid, double distance);
	EstimatorResult solve(long currentTime);



private:
	Solver solver;
	EstimatorArgument *args;
	MeasurementList *measurementList;
	FilterManager *filterManager;
	Analyzer *analyzer;

	SolverInput *input;
	
	Vector prevLocation;

	void setMeasurementList();
	void setSolver();
	void setInput();
	void setFilterManager();
	
	void optimize1(SolverResultList *results);



	

};
