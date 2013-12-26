#include <string.h>
#include "time.h"
#include "LocationEstimator.h"


LocationEstimator::LocationEstimator(Argument &_args)
{	
	args = _args;
	constructor();
}

LocationEstimator::~LocationEstimator(void)
{
	destructor();
}


void LocationEstimator::constructor()
{
	beaconListSize = 20;
	beaconList = (BeaconInfo**) calloc(beaconListSize, sizeof(BeaconInfo*));

	stringBuf[0] = '\0';
	recentTimestamp = 0;

//	// KTODO : make dt more flexible 
	filterManager = NULL;
	solver = new PositionSolver(args);
	solver->positionInfo.clear();
	n_candidate = 0;
	n_candidate_filtered = 0;

	loadPlane();
}

void LocationEstimator::destructor()
{
	for (int i = 0; i<beaconListSize; i++)
	{
		if (beaconList[i] != NULL) 
		{
			delete beaconList[i];
			beaconList[i] = NULL;
		}
	}
	free (beaconList);
	if (filterManager != NULL) 
	{
		delete filterManager;
		filterManager = NULL;
	}
	planeList.clear();

	if (solver != NULL)
	{
		delete solver;
		solver = NULL;
	}
	
}
void LocationEstimator::Reset()
{
	destructor();
	constructor();
}

void LocationEstimator::SetBeaconInfo(int beacon_id, Vector location)
{
	if (beaconListSize <= beacon_id)
	{
		BeaconInfo** newList = (BeaconInfo**) calloc(beacon_id + 1, sizeof(BeaconInfo*));
		memcpy(newList, beaconList, sizeof(BeaconInfo*)* beaconListSize);
		beaconListSize = beacon_id + 1;
		free(beaconList);
		beaconList = newList;		
	}
	if (beaconList[beacon_id] != NULL) 
		delete beaconList[beacon_id];
	beaconList[beacon_id] = new BeaconInfo(beacon_id, location);

}

void LocationEstimator::DeleteBeaconInfo(int beacon_id)
{
	if (beaconListSize <= beacon_id) return;
	if (beaconList[beacon_id] == NULL) return;
	delete beaconList[beacon_id];
	beaconList[beacon_id] = NULL;

}

void LocationEstimator::SetPlane(Vector p1, Vector p2, Vector p3)
{
	// 안에서 보았을 때 시계방향순의 세 점
	Plane plane = Plane(p1, p2, p3);
	plane.idx = planeList.size();
	planeList.push_back(plane);	
}

void LocationEstimator::SetPlane(Vector p1, Vector p2, Vector p3, Vector p4)
{
	// 안에서 보았을 때 시계방향순으로 경계점
	Plane plane = Plane(p1, p2, p3, p4);
	plane.idx = planeList.size();
	planeList.push_back(plane);	
	
}

void LocationEstimator::SetPlane(Plane plane)
{
	plane.idx = planeList.size();
	planeList.push_back(plane);
}

void LocationEstimator::loadPlane()
{
	FILE* fp;
	fp = fopen(args.PlaneConfigFilename, "r");

	if (fp == NULL)
	{
		SetPlanes(args.RoomWidth, args.RoomLength, args.RoomHeight);
		savePlanes(args.PlaneConfigFilename);
		return;
	}

	char buf[1024];

	printf("Load Planes\n");
	while (fgets(buf, 1023, fp) != NULL)
	{
		printf("%s ... ", buf);
		Plane plane = Plane(buf);
		SetPlane(plane);		
		printf("done\n");
//		printf(".");
	}
	printf("\n");
	fclose(fp);


}

void LocationEstimator::SetPlanes(int width, int length, int height)
{

	/* 
	when making 8 points of cube (2 x 2 x 2), RH rules
		v1 = (-1,  1, 2)
		v2 = ( 1,  1, 2)
		v3 = (-1, -1, 2)
		v4 = ( 1, -1, 2)
		v5 = (-1,  1, 0)
		v6 = ( 1,  1, 0)
		v7 = (-1, -1, 0)
		v8 = ( 1, -1, 0)

		  1------2
		 /|     /|
		3------4 |
		| 5----|-6 
		|/     |/
		7------8
	*/

	double cx = width / 2.0;
	double cy = length / 2.0;
	double cz = height;

	Vector v1 = Vector(-cx,  cy, cz);
	Vector v2 = Vector( cx,  cy, cz);
	Vector v3 = Vector(-cx, -cy, cz);
	Vector v4 = Vector( cx, -cy, cz);
	Vector v5 = Vector(-cx,  cy, 0);
	Vector v6 = Vector( cx,  cy, 0);
	Vector v7 = Vector(-cx, -cy, 0);
	Vector v8 = Vector( cx, -cy, 0);

	SetPlane(v1, v2, v6, v5);		// front plane
	SetPlane(v2, v4, v8, v6);		// right side plane
	SetPlane(v4, v3, v7, v8);		// back plane
	SetPlane(v3, v1, v5, v7);		// left plane
	SetPlane(v3, v4, v2, v1);		// ceiling
	SetPlane(v5, v6, v8, v7);		// floor

/*
	// original plane definition 
	SetPlane(Vector(0, length / 2, 0), Vector(0, length / 2, height), Vector (width/ 2, length / 2, 0));
	SetPlane(Vector(0, - length / 2, 0), Vector(0, - length /2, height), Vector (-width / 2, -length / 2, 0));
	SetPlane(Vector(-width / 2, 0, 0), Vector (-width / 2, -length / 2 , 0), Vector(-width / 2, 0, height));
	SetPlane(Vector(width / 2, 0, 0), Vector (width / 2, length /2 , 0), Vector(width / 2, 0, height));
	SetPlane(Vector(0, 0, 0), Vector(0, length / 2, 0), Vector(width / 2, 0, 0));
	SetPlane(Vector(0, 0, height), Vector(width / 2, 0, height), Vector(0, length / 2, height));
*/
/*
	// Jihyes's pentagon planes
	SetPlane(Vector(0, length / 2, 0), Vector(0, length / 2, height), Vector (width/ 2, length / 2, 0));
	SetPlane(Vector(0, - length / 2, 0), Vector(0, - length /2, height), Vector (-width / 2, -length / 2, 0));
	SetPlane(Vector(-width / 2, 0, 0), Vector (-width / 2, -length / 2 , 0), Vector(-width / 2, 0, height));
	SetPlane(Vector(width / 2, 0, 0), Vector (width / 2, length /2 , 0), Vector(width / 2, 0, height));
	SetPlane(Vector(0, 0, 0), Vector(0, length / 2, 0), Vector(width / 2, 0, 0));
	SetPlane(Vector(0, 0, height), Vector(width / 2, 0, height), Vector(0, length / 2, height));
	SetPlane(Vector(width / 6, length / 2, 0), Vector(width / 6, length / 2, height), Vector(width / 2, length / 6, 0));
*/
}

void LocationEstimator::savePlanes(char *filename)
{
	FILE* fp;
	fp = fopen(filename, "w");
	if (fp == NULL)
	{
		printf("file open error : %s in LocationEstimator::savePlanes\n", filename);
		getchar();
		exit(10);
	}
	char buf[1024];
	for (size_t i = 0; i < planeList.size(); i ++)
	{
		planeList[i].GetString(buf);
		fprintf(fp, "%s\n", buf);
	}
	fclose(fp);
}

/*
TODO: Multi-listener
*/
void LocationEstimator::InsertHeardData(int listener_id, long seq, int beacon_id, double distance)
{		
	if (distance == 0 || beaconList[beacon_id] == NULL) return;
	beaconList[beacon_id]->temp2 = listener_id;
	beaconList[beacon_id]->timestamp = seq;
	beaconList[beacon_id]->distance =  distance;	
	if (recentTimestamp < seq)
		recentTimestamp = seq;

}

bool LocationEstimator::cutHeardList(HeardList &heardList, int start_idx, int cutsize, double cut_thresh)
{
	if (heardList.n_valid == cutsize)
	{
		double err;
		solver->NLLeastSquareSolver(heardList, &err);
		if (err < cut_thresh) return true;
		return false;
	}
	heardList.n_valid ++;
	for (int i = start_idx; i < (int) heardList.heardInfo.size() - (cutsize - heardList.n_valid); i++)
	{
		heardList.heardInfo[i].valid = true;		
		if (cutHeardList(heardList, i + 1, cutsize, cut_thresh))
			return true;
		heardList.heardInfo[i].valid = false;
	}
	heardList.n_valid --;
	return false;
}
		


bool LocationEstimator::makeHeardList(HeardList &heardList, int minBeaconNum, int cutSize, bool fitCheck)
{
	for (int i = 0; i < beaconListSize; i++)
	{
		if (beaconList[i] == NULL) continue;
		if ((recentTimestamp - beaconList[i]->timestamp) > 1000) continue;
		if (beaconList[i]->distance < args.MinValidDistance) continue;
		heardList.insertHeardBeacon(0, i, beaconList[i], beaconList[i]->distance);
	}

	if ((int)heardList.heardInfo.size() < minBeaconNum) return false;
	if (fitCheck) 
	{
		heardList.CheckReflectionFitting(planeList);
		heardList.Sort();
		heardList.Cut(cutSize);
	}
	else
	{
		heardList.n_valid = heardList.heardInfo.size();
		while ((int)heardList.heardInfo.size() > cutSize)
		{
			int idx = (int)(heardList.heardInfo.size() * random.GetUniformDist());
			heardList.heardInfo.erase(heardList.heardInfo.begin() + idx);
			heardList.n_valid --;
		}
		for (size_t i = 0; i < heardList.heardInfo.size(); i++)
			heardList.heardInfo[i].valid = true;
	}
	return true;
}

Vector LocationEstimator::Solve()
{
	// minimum number of beacons should be changed if beacons are not deployed on same plane
	int minBeaconNum = 3;	
	HeardList heardList = HeardList(1);	
	
	if (!makeHeardList(heardList, minBeaconNum, args.NCut, false)) 
		return solver->prevPositionLSQ;

	double err = 0;
	Vector position;
	heardList.initPosition = solver->prevPositionLSQ;
	position = solver->NLLeastSquareSolver(heardList, &err);	
	solver->prevPositionLSQ = position;
	return position;
}


Vector LocationEstimator::SolveEx()
{
	int minBeaconNum = 3;
	bool enoughList;
	Vector position;

	/*T1{*/args.analyzer->EstimatorTotal.StartTimer();
	/*T2{*/args.analyzer->EstimatorSetup.StartTimer();
	/**/HeardList heardList = HeardList(1);	
	/**/enoughList = makeHeardList(heardList, minBeaconNum, args.NCut, args.Optimization & OPT::SELECTION);
	/*}T2*/args.analyzer->EstimatorSetup.StopTimer();

	if ((int)heardList.heardInfo.size() < args.NCut)
	{
		/*V1*/args.analyzer->N_SelectionFail.AddValue(1.0);
		/*V1*/args.analyzer->N_SelectionFail.Commit();
		/*}T1*/args.analyzer->EstimatorTotal.StopTimer();
	}

	if (!enoughList) 
	{
		position = solver->prevPositionExLSQ;		
		return position;
	}
	

	/*V2*/args.analyzer->N_Selected.AddValue(heardList.heardInfo.size());
	/*T3{*/args.analyzer->EstimatorSolving.StartTimer();	/* start */
	/**/heardList.initPosition = solver->prevPositionExLSQ;
	/**/solver->ExLeastSquareSolver(heardList, planeList);
	/**/n_candidate = solver->positionInfo.size();
	/*}T3*/args.analyzer->EstimatorSolving.StopTimer();	/* stop */
	/*V3*/args.analyzer->N_PMS.AddValue((double)n_candidate);

	if (args.Optimization & OPT::THRESHOLD)
	{		
		/*T4{*/args.analyzer->EstimatorPostProc.StartTimer();	/* start */
		/**/PointFilter((int)args.CutThreshold);
		/*}T4*/args.analyzer->EstimatorPostProc.StopTimer();	/* stop */
		n_candidate_filtered = solver->positionInfo.size();

	}

	/*T5{*/args.analyzer->EstimatorKFProc.StartTimer();	/* start */
	/**/filterManager->ProcessNextBranch(solver->positionInfo);		
	/*}T5*/args.analyzer->EstimatorKFProc.StopTimer();
	
	if (filterManager->filterInfo.size() == 0)
		position = solver->prevPositionExLSQ;
	else 
	{
		position = filterManager->GetCorrectedValue();	// get Corrected Position
//		position = filterManager->GetInputValue();	// get Measured Position
	}

	solver->prevPositionExLSQ = position;

	position.temp = 0;
	for (size_t i = 0; i < heardList.heardInfo.size(); i++)
	{
		position.temp += heardList.heardInfo[i].seq;
		/*V4*/args.analyzer->N_Reflection.AddValue((double)heardList.heardInfo[i].seq);
	}

	/*}T1*/args.analyzer->EstimatorTotal.StopTimer();	/* stop */
	/*TTS*/args.analyzer->TickTimeSlot();			// commit all
	
	return position;
}


void LocationEstimator::PointFilter(int cutThreshold)
{	
	int pms_neg_cnt = 0;
	for (size_t i = 0; i < solver->positionInfo.size(); i++)
	{
		solver->positionInfo[i].valid = true;
		// errornous
		if (solver->positionInfo[i].error > cutThreshold) 
		{
			solver->positionInfo[i].valid = false;
			pms_neg_cnt++;
			continue;
		}

		for (size_t j = 0; j < planeList.size(); j++)
		{	
			if (!planeList[j].IsBoundary()) continue;
			if (planeList[j].GetSign(solver->positionInfo[i].position) > 0) 
			{
				solver->positionInfo[i].valid = false;
				pms_neg_cnt++;
				break;
			}
		}
	}
	args.analyzer->N_PMSFiltered.AddValue((double)(solver->positionInfo.size() - pms_neg_cnt));

}


