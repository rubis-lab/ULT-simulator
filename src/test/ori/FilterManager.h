#pragma once

#include <vector>
#include "KFTracker.h"
#include "PositionSolver.h"
#include "Argument.h"





class FilterManager
{
public:
	FilterManager(Argument& args);
	~FilterManager(void);

	std::vector <KFTracker *> filterInfo;
	void Reset();	
	void ProcessNextBranch(std::vector <SolvedPosition> &positionInfo);
	Vector ProcessNext(Vector position);
	int cold_start;

	Argument args;

	Vector GetPredictedValue();
	Vector GetCorrectedValue();
	Vector GetInputValue();
	

private:
	int mode;

	KFTracker rootTracker;
	
	bool blInit;

	void clearFilterInfo();
	void initFilterInfo(std::vector <SolvedPosition> &positionInfo);
	void insertFilter(std::vector <KFTracker *> &filterInfo, KFTracker *filter);	


	int step_cnt;

};
