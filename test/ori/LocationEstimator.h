#pragma once
#include "PositionSolver.h"
#include "BeaconInfo.h"
#include "FilterManager.h"
#include "Argument.h"
#include "Random.h"


#define MAX_LISTENER 10

class LocationEstimator
{
/*
private:
	bool reflectionMapChanged;
	Vector **reflectionMap;
	void setReflectionMap();
	int reflection_bsize;
	int reflection_psize;
	void deleteReflectionMap();
*/

private:
	void PointFilter(int cutThreshold);
	void constructor();
	void destructor();

	bool makeHeardList(HeardList &heardList, int minBeaconNum, int cut_size, bool fitCheck);	
	Argument args;
	void loadPlane();
	void savePlanes(char* filename);
	Random random;


public:
	LocationEstimator(Argument &args);
	~LocationEstimator(void);
	
	BeaconInfo** beaconList;
	HeardList** listenerList;

	std::vector <Plane> planeList;

	int beaconListSize;
	int listenerListSize;
	

	char stringBuf[4096];

	PositionSolver *solver;
	Vector solvedPosition;
	FilterManager *filterManager;

	long recentTimestamp;
	

	// listener 수 만큼 HeardList 필요
	void InsertHeardData(int listener_id, long seq, int beacon_id, double distance);
	void SetBeaconInfo(int beacon_id, Vector location);
	void DeleteBeaconInfo(int beacon_id);
	Vector Solve();
	Vector SolveEx();
	int GetString(char* buf, int buflen);
	void InsertString(const char* type, int l_id, Vector pos, double err);
	void ResetString();
	void SetPlane(Vector p1, Vector p2, Vector p3);	
	void SetPlane(Vector p1, Vector p2, Vector p3, Vector p4);	
	void SetPlanes(int width, int length, int height);
	void SetPlane(Plane plane);

	void SetReflectionMap();
	bool CheckReliableEstimation(HeardList &heardList);

	void Reset();
	bool cutHeardList(HeardList &heardList, int start_idx, int cutsize, double cut_thresh);

//	HeardList tempHeardList;

	long int n_candidate;
	long int n_candidate_filtered;
	
	
};
