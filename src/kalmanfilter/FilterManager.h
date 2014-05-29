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
	void getCorrectedResult(SolverResultList *results);


private:
	KFTracker *currentTracker;
	KFArgument args;
	bool blInit;

	void destructor();
	SolverResult* findMostAccurateResult(SolverResultList results);
	void correctResult(SolverResult *result);
	bool initFilterInfo(SolverResultList *result);
	SolverResult* processNextStep(SolverResultList *results);




};
