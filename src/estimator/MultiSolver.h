#pragma once
#include <vector>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_linalg.h>
#include "Vector.h"
#include "Measurement.h"
#include "Analyzer.h"
#include "Solver.h"


class MultiSolverInput
{
public:
	MultiSolverInput(SolverInput *input);
	~MultiSolverInput();

	Vector getLocation(int idx);
	double getDistance(int idx);
	size_t size();

	std::vector<MeasurementInstance> data;
};

class MultiSolver
{
public:
	MultiSolver();
	~MultiSolver();

	void solve(SolverInput *input, SolverResultList *results);
	void setSolverCondition(SolverCondition condition);

private:
	SolverCondition condition;

	SolverResult solveNaive(MultiSolverInput &input);
	void solveWithPlanes(SolverInput *input, int currentIdx = 0);
	Vector NLLeastSquareSolver(MultiSolverInput *input);
	void solveInputList(SolverResultList *results);

	// optimization method 1. JH's method using distance of two beacons
	bool checkCutBranch1(SolverInput *input, int currentIdx);
	bool checkBranchCutoff(Measurement* meas1, Measurement* meas2);

	// optimization method 2. Using distances of three beacons	
	bool checkCutBranch2(SolverInput *input, int currentIdx);

	std::vector<MultiSolverInput> inputList;



	
};

