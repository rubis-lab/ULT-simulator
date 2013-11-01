#pragma once
//#include "Position.h"
#include "Vector.h"
#include "HeardList.h"
#include "Argument.h"
#include "Cylinder.h"
#include <vector>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_linalg.h>

#define REFLECTION_LIMIT 1
#define LSQ_THRESHOLD 100
#define MAX_HISTORY 1024
#define MAX_VAL 99999
//#define SPEED_LIMIT 120		// cm per second
/*
#define FILTER1 0x0001
#define FILTER2 0x0002
#define FILTER3 0x0004
#define FILTER4 0x0008
#define FILTER5 0x0010
#define FILTER_LOG 0x8000
#define FILTER_FLAG (0x0 | FILTER2 \
	| FILTER1 \
	| FILTER2 \
	| FILTER3 \
	| FILTER4 \
	| FILTER5 \
	| FILTER_LOG \
	)
*/
const int mobilityLimit = 100;

class SolvedPosition
{
public:
	SolvedPosition();
	Vector position;
	double error;
	int history_size; 
	char b_hist[MAX_HISTORY];
	char p_hist[MAX_HISTORY];
	unsigned int filter;

	void AddHistory(int bid, int pid);	
	void SetPosition(Vector &position, double error);
	SolvedPosition Duplicate();
	bool valid;
};



		

class PositionSolver
{
private:
	double lsq_threshold;	
//	int measError;
	Argument args;
	

public:
	PositionSolver(Argument args);
	~PositionSolver(void);

	std::vector <SolvedPosition> positionInfo;	
	bool validPrevPosEx;
	Vector prevPositionExLSQ;
	Vector prevPositionLSQ;



public:
	bool ExLeastSquareSolver(HeardList &heardList, std::vector<Plane> &planeList, int **reflection_cnt, int bid, int ref_cnt, 
		SolvedPosition &solvedPosition);


	void ExLeastSquareSolver(HeardList &heardList, std::vector<Plane> &planeList);

	Vector InvalidPosition();
	bool IsInvalidPosition(Vector &pos);
	bool CheckReflectionIntegrity(int** reflection, int bsize, int psize);

	void SetLSQThreshold(double threshold);
	double GetError(HeardList &heardList, Vector &point);
	double FindMinimum(HeardList &heardList, Vector &ret);





// Non-Linear Least Square Sovler. GSL Nonlinear Curve-fitting

private:

	bool CheckBranchCutoff(DistanceInfo &info1, DistanceInfo &info2);

public:
	Vector NLLeastSquareSolver(HeardList &heardList, double *err);	
	Vector LeastSquareSolver(HeardList &heardList, double *err);
	Vector LeastSquareSolver2D(HeardList &heardList, double *err);

	int cnt;
};
