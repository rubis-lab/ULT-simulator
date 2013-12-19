#pragma once
#include <vector>
#include "KFTracker.h"
#include "Solver.h"


class FilterManager
{
public:
	FilterManager(KFArgument args);
	~FilterManager();

	void reset();
	void getCorrectedResult(SolverResult *results);


private:
	KFTracker *currentTracker;
	KFArgument args;
	bool blInit;

	void destructor();
	SolverResult::Result* findMostAccurateResult(SolverResult results);
	void correctResult(SolverResult::Result *result);
	bool initFilterInfo(SolverResult *result);
	SolverResult::Result* processNextStep(SolverResult *results);




};
