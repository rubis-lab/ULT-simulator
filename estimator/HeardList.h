#pragma once
#include <vector>
#include "Vector.h"
#include "Plane.h"

#define REFLECTION_LIMIT 1

class BeaconInfo
{
public:
	BeaconInfo(int bid, Vector location);
	~BeaconInfo(){};

	BeaconInfo ReflectByPlane(Plane *plane);
	BeaconInfo (BeaconInfo *beaconInfo);
	void AddChild(BeaconInfo *beaconInfo);

	
private:
	int bid;
	int reflectionCount;
	std::vector<Vector> vLocations;
	std::vector<Plane*> pPlanes;
	BeaconInfo *parent;
	std::vector<BeaconInfo *> children;
}
		

class Measurement                                                                    
{                                                                                    
public:                                                                              
	Vector vBeacon;                                                                  
	double distance;                                                                 

	Measurement(Vector vBeacon, distance) : vBeacon(vBeacon), distance(Distance);    
}                                                                                    
                                                                                     


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
