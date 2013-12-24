#include "FilterManager.h"



FilterManager::FilterManager(Argument& args)
{
	this->args = args;
	Reset();
}

FilterManager::~FilterManager(void)
{
	clearFilterInfo();
}


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

void FilterManager::initFilterInfo(std::vector <SolvedPosition> &positionInfo)
{
/*
	if (positionInfo.size() == 0) return;
	blInit = false;
	
	for (size_t i = 0; i < positionInfo.size(); i++)
	{		
		if (i >= 1) break;		
		KFTracker *initKFT = new KFTracker(&rootTracker);
		initKFT->InitTracker(positionInfo[i].position);
		filterInfo.push_back(initKFT);
	}
*/
	for (size_t i=0; i < positionInfo.size(); i++)
	{
		if (!positionInfo[i].valid) continue;
		KFTracker *initKFT = new KFTracker(&rootTracker);
		initKFT->InitTracker(positionInfo[i].position);
		filterInfo.push_back(initKFT);
		blInit = false;
		break;
	}
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
		if ((*it)->error > tracker->error) break;
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