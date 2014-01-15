#pragma once
#include <stdio.h>
#include <vector>
#include "LocationEstimator.h"
#include "Random.h"
#include "TestRoom.h"
#include "ListenerMover.h"
#include "SimulationTracer.h"
#include "Argument.h"

class LogInfo
{
public :
	Vector vPosition;
	Vector vFace;
	BeaconInfo* beaconList;
	long timestamp;
};

class PDListener
{
public:
	PDListener(Argument& args)
	{
		moveWeight = 5;
		distWeight = 5;
		distMax = 380;
		distInit = 1;
		z = 130;

		vSize = Vector(args.RoomWidth / 2, args.RoomLength / 2, z);
		vO[0] = Vector(-1, 1, 1);
		vO[1] = Vector(-1, -1, 1);
		vO[2] = Vector(1, -1, 1);
		vO[3] = Vector(1, 1, 1);

		vD[0] = Vector(0, -1, 0);
		vD[1] = Vector(1, 0, 0);
		vD[2] = Vector(0, 1, 0);
		vD[3] = Vector(-1, 0, 0);

		vDist[0] = Vector(1, 0, 0);
		vDist[1] = Vector(0, 1, 0);
		vDist[2] = Vector(-1, 0, 0);
		vDist[3] = Vector(0, -1, 0);

		distCur = distInit;

		setMode(2);
		
	}

	bool MoveNext()
	{
		vCur = vCur + (vD[mode] * moveWeight);
		if (!checkBoundary())
		{
			if (mode == 3) increaseDist();			
			if (distCur > distMax) 
				return false;
//			setMode(mode + 1);
			increaseDist();			
			setMode(mode);
			
		}
		return true;
	}
	Vector GetListenerPosition()
	{
		return vCur;
	}
	Vector GetListenrFace()
	{
		return -vDist[mode];
	}
private:
	bool checkBoundary()
	{
//		if (fabs(vCur.x) > vSize.x - distCur || fabs(vCur.y) > vSize.y -distCur)
		if (fabs(vCur.x) > vSize.x || fabs(vCur.y) > vSize.y - distInit)
			return false;
		return true;
	}
	void increaseDist()
	{
		distCur += distWeight;
	}
	void setMode(int _mode)
	{
		mode = _mode % 4;
		vCur = vO[mode].getCompCross(vSize);		// set to original point
		vCur = vCur + (vD[mode] * distInit);	// move upto current distance
		vCur = vCur + (vDist[mode] * distCur);	// apart from plane up to current distance
	}



private:
	Vector vSize;
	int moveWeight;
	int mode;		// 0: left, 1: bottom, 2: right, 3:top
	Vector vO[4];
	Vector vD[4];
	Vector vDist[4];
	int distCur;
	int distWeight;
	int distMax;
	int distInit;
	int z;
	Vector vCur;
};


class LogRecorder
{
public:
//	LogRecorder(Random *random);
	LogRecorder();
	LogRecorder(unsigned long randomseed);
	~LogRecorder(void);
	

	bool CreateLog(char* filename, Argument& args);
	bool CreateLog(char* filename, Argument& args, unsigned long seed);
	bool LoadLog(char* filename, int *ref_cnt1, int *ref_cnt2);
	bool LoadNextLog();	
	int l_width, l_length, l_height;
	int nBeacon;	

	std::vector <LogInfo> logList;
	bool blLoadFlag;
private:
	
	FILE* fp;
	void Reset();
	int getArea(Vector vPos);
	
	Vector getPoint(Vector vRange, Vector vOffset);
	Vector getPointRND();
	Vector getPointCW(Vector vPos);
	Vector getPointACW(Vector vPos);

	Argument args;



	Random *randomRoom;
	Random *randomListener;
	int margin;
	int hallway_x;
	int hallway_y;



};
