#pragma once

#include "SimulatorArgument.h"


#define ARM_LENGTH 50
#define GRAVITY_ACC 9.8


class PathInfo
{
public:
	Vector vStartPosition;
	Vector vPosition;	
	Vector uvDirection;
	Vector vDestination;
	
	Vector uvFace;

	PathInfo();
	~PathInfo();
//	void SetFace(double theta, double phi);
	void SetFace(Vector faceVector);
	void SetStartPosition(Vector position);
	void SetFinishPosition(Vector position);
	void SetCoefficient(double speed_avg, double speed_dev, 
						double angular_avg, double angular_dev, double interval);
	

	int MoveNext();

	double speed_avg;			// speed in m / s
	double speed_dev;
	double angular_avg;
	double angular_dev;	


	double interval;			// interval in ms
//	double timestamp;			// timestamp in ms

	double thetaX;
	double thetaY;
	double thetaZ;

private:
	Vector GetFaceVector(double theta, double phi);
	double face_theta;		// projected angle to xy-plane
	double face_phi;		// angle to z-axis	
	




};

class ListenerMover
{
public:
	ListenerMover(SimulatorArgument *args);
	~ListenerMover(void);
	std::vector <PathInfo> pathList;


	

public:
	void Reset();
	void Reset(int width, int length, int height);
	void SetPath(Vector point, double speed);
	long MoveNext();
	Vector GetListenerPosition();
	Vector GetListenerFace();

	


public:
	int width, length, height;

private:

	Vector vPosition;
	Vector vFace;
	bool bFirst;
	std::vector<PathInfo>::iterator itPathList;
	long timestamp;

	Vector vArmAcc;
	Vector vArmVel;
	Vector pArm;
	SimulatorArgument *args;
};

