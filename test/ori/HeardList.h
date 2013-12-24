#pragma once
#include <vector>
#include "BeaconInfo.h"
#include "Vector.h"
#include "Plane.h"
//#include "PlaneList.h"



class DistanceInfo
{
public:
	DistanceInfo(int bid, BeaconInfo *beacon, double distance, int seq);
	int bid;
	Vector location;
	double distance;
	int seq;
//	bool fgRefFit;
	int nVSS;
	bool valid;	
};


class HeardList
{
public:
	HeardList();
	HeardList(int l_id);
	~HeardList(void);

	int l_id;

	int n_valid;

	Vector initPosition;
	

public:
	void reset();
	void insertHeardBeacon(int seq, int bid, BeaconInfo* beacon_id, double distance);
	int GetValidList(HeardList &heardList);
//	std::vector <Vector> heardBeacon;
//	std::vector <double> heardDistance;
//	std::vector <int> heardSeq;
	std::vector <DistanceInfo> heardInfo;
	int last_seq;
	void Duplicate(HeardList& dupInstance);
	void setHeardBeacon(int seq, Vector position, double distance);
	int Compare(DistanceInfo &a, DistanceInfo &b);
	void Sort();
	void Cut(size_t n);
	bool CheckReflectionFitting(std::vector<Plane>  &planeList);
};
