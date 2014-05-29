#include "FilterManager.h"



FilterManager::FilterManager(KFArgument args)
{
	this->args = args;
	currentTracker = NULL;
	reset();
}

FilterManager::~FilterManager(void)
{
	destructor();
}

void FilterManager::destructor()
{
	if (currentTracker != NULL)
	{
		delete currentTracker;
		currentTracker = NULL;
	}
}

void FilterManager::reset()
{
	destructor();
	blInit = true;
	currentTracker = new KFTracker(args);

}


void FilterManager::getCorrectedResult(SolverResultList *results)
{
	bool ret;
	if (blInit)
	{
		ret = initFilterInfo(results);
		if (ret) blInit = false;
		
		return;
	}

	SolverResult *result;

	result = processNextStep(results);

	if (result == NULL)
	{
		//cold start
		ret = initFilterInfo(results);
		if (!ret) blInit = true;

		return;
	}

	//TODO
	//instead of processNext, 
	//find the closest predicted value, and then correct it.
	//
	//SolverResult *result;
	//result = findMostAccurateResult(results);
	//correctResult(result);
	
	results->setFilteredResult(*result);
}


SolverResult* FilterManager::processNextStep(SolverResultList *results)
{
	std::vector <KFTracker *> filterInfoTemp;

	double minError = -1;
	KFTracker *minTracker = NULL;
	SolverResult *minResult = NULL;
	for (size_t i = 0; i < results->size(); i++)
	{
		if (!results->isValid(i)) continue;
		KFTracker *repKFT = new KFTracker(currentTracker);
		SolverResult *result = results->at(i);

		Vector vPredict = repKFT->predict();
		result->setPredictedLocation(vPredict);

		Vector vCorrect = repKFT->correct(result);
		results->at(i)->setCorrectedLocation(vCorrect);

		if (minError < 0 || result->getPredictedDistanceError() < minError)
		{
			minError = result->getPredictedDistanceError();

			if (minTracker != NULL)
				delete minTracker;

			minTracker = repKFT;
			minResult = result;
		}
		else 
		{
			delete repKFT;
		}
	}

	if (minTracker == NULL)
	{
		// TODO : cold start
		// ANALYZ
		return NULL;
	}

	delete currentTracker;
	currentTracker = minTracker;
	

	return minResult;
}

bool FilterManager::initFilterInfo(SolverResultList *results)
{
	int minIdx = -1;
	double minValue;
	for (size_t i=0; i < results->size(); i++)
	{
		if (!results->isValid(i)) continue;
		double curValue = results->at(i)->error;
		if (minIdx < 0 || minValue > curValue)
		{
			minIdx = i;
			minValue = curValue;
		}
	}

	if (minIdx < 0) return false;

	SolverResult* result = results->at(minIdx);

	currentTracker->initTracker(result);
	results->setFilteredResult(*result);

	return true;
}

#if 0

void FilterManager::Reset()
{
	clearFilterInfo();
	blInit = true;
	rootTracker.SetTracker(args);	
	cold_start = 0;
	step_cnt = 0;

}




void FilterManager::clearFilterInfo()
{
	for (size_t i = 0; i < filterInfo.size(); i++)	
		delete filterInfo[i];
	filterInfo.clear();
}


void FilterManager::insertFilter(std::vector<KFTracker*> &filterInfo, KFTracker *tracker)
{
	std::vector<KFTracker*>::iterator it;
	
	for(it = filterInfo.begin(); it != filterInfo.end(); it ++)	
	{
		if ((*it)->IsSame(tracker))
		{
			delete tracker;
			return;
		}
		if ((*it)->getError() > tracker->getError()) break;
		//KFTracker's error is deleted. use result's distErrorPredicted instead.
	}		


	filterInfo.insert(it, tracker);
	while (filterInfo.size() > 1)
	{
		delete filterInfo[filterInfo.size() - 1];
		filterInfo.erase(filterInfo.end() - 1);
	}
}

Vector FilterManager::ProcessNext(Vector position)
{
	std::vector <SolvedPosition> positionInfo;
	positionInfo.clear();
	SolvedPosition solvedPosition;
	solvedPosition.SetPosition(position, 0);
	positionInfo.push_back(solvedPosition);
	ProcessNextBranch(positionInfo);
	return filterInfo[0]->GetX();
}



void FilterManager::ProcessNextBranch(std::vector <SolvedPosition> &positionInfo)
{
	if (blInit)
	{
		initFilterInfo(positionInfo);
		return;
	}


	std::vector <KFTracker *> filterInfoTemp;

	for (size_t i = 0 ;i < positionInfo.size(); i++)
	{
		if (!positionInfo[i].valid) continue;
		for (size_t j = 0 ;j < filterInfo.size(); j++)
		{
			KFTracker *repKFT = new KFTracker(filterInfo[j]);
			repKFT->Predict();
			repKFT->Correct(positionInfo[i].position);
			insertFilter(filterInfoTemp, repKFT);
		}
	}
	
	step_cnt ++;
	if (filterInfoTemp.size() != 0)
	{
		clearFilterInfo();	
		filterInfo = filterInfoTemp;
	}

// steady prediction
/*
	else
	{
		Vector vPrevBest = filterInfo[0]->vTemp;

		// just predict previous states
		gsl_matrix_view Z;
		for (size_t i = 0; i < filterInfo.size(); i++)
		{
			filterInfo[i]->fast_predict();
			Z = gsl_matrix_submatrix(filterInfo[i]->m[KF_X0], 0, 0, 3, 1);
			filterInfo[i]->correct(&Z.matrix);
		}

		// add new states based on estimated points
		initFilterInfo(positionInfo);

		// sort by distance between previous position and predicted position
		for (size_t i = 0; i < filterInfo.size(); i++)
		{
			filterInfo[i]->error = vPrevBest.getDistance(filterInfo[i]->GetX0());
			insertFilter(filterInfoTemp, filterInfo[i]);
		}
		filterInfo = filterInfoTemp;
	}
*/

	else	// cold start
	{
		clearFilterInfo();
		initFilterInfo(positionInfo);		
		cold_start ++;
//		printf("%d / %d = %.1f\n", cold_start, step_cnt, ((double) cold_start / (double) step_cnt) * 100);
	}


}




Vector FilterManager::GetInputValue()
{
	return filterInfo[0]->measurement;
}

Vector FilterManager::GetPredictedValue()
{
	return filterInfo[0]->GetX0();
}

Vector FilterManager::GetCorrectedValue()
{
	return filterInfo[0]->GetX();
}
#endif
