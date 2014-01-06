#pragma once
#include <vector>
#include <stack>
#include <math.h>
#include "LocationEstimator.h"
#include "Random.h"
#include "SimulationTracer.h"
#include "Argument.h"

#define MAX_BID 100
#define RADIATE_ANGLE 120 
#define MAX_REF_CNT 2

typedef struct reflectionInfo
{
	BeaconInfo beacon;
	Plane plane[MAX_REF_CNT + 1];
	int n_plane;
	Vector vListener;
	Vector vCross[MAX_REF_CNT + 1];
	double distance;
	bool isValid;
}reflectionInfo;

class TestRoom
{
public:
	TestRoom(LocationEstimator *estimator, Random* random, Argument args);
	~TestRoom(void);

public:
	int width, length, height;
	int nBeacon;
	int angleRange;
	Vector vListener;
	Vector uvFace;
	LocationEstimator *estimator;

//	double heardDistance[MAX_BID + 1];
//	Vector reflectionPoint[MAX_BID + 1];
//	BeaconInfo* minBeacons[MAX_BID + 1];
	reflectionInfo* minReflect[MAX_BID + 1];
	

private:
	Random *random;
	double errAvg;
	double errDev;
	std::vector <BeaconInfo> beaconList;
	std::vector <reflectionInfo> reflectList;
	Argument args;

public:
	void SetError(double errAvg, double errDev);
	void SetBeacon(int bid, double x, double y, double z);
//	void CalculateBeaconReflection();
	void Reset(int width, int length, int height);
	void Reset();
	void SetListener(Vector location, Vector vFace, long timestamp);
	void SetRandomBeacon(int nBeacon, bool onCeiling = true);
	void SetRandomBeacon2(int nBeacon);
	void SetBeaconLocationFixed(int nBeacon);
	double GetAppError(Vector vBeacon, Vector vListener);

private:
	void setHeardDistance(long timestamp, double errAvg, double errDev);
	void setBeaconList(LocationEstimator *estimator);
	void setReflectedBeacons();
	void setReflectedBeacons(BeaconInfo beacon, reflectionInfo reflect);
	bool checkValidReflection(reflectionInfo &reflect, Vector vListener);
	void setCrossPoint(reflectionInfo *reflect);
	void setBeaconFromFile(char *file);
	void SetBeaconByCoulomb(int nBeacon);
	
};