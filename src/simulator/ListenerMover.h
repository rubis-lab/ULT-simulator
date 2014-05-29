#pragma once

#include "SimulatorArgument.h"


#define ARM_LENGTH 50
#define GRAVITY_ACC 9.8

class ListenerInfo
{
public:
	ListenerInfo()
	{
	}
	ListenerInfo(unsigned long timestamp, Vector location, Vector facing) :
		timestamp(timestamp), 
		location(location),
		facing(facing)
	{
	}

	unsigned long timestamp;
	Vector location;
	Vector facing;
};

class PathInfo
{
public:
	Vector vStartPosition;
	Vector vPosition;	
	Vector uvDirection;
	Vector vDestination;
	
	Vector uvFace;

	PathInfo(Random *random);
	~PathInfo();
//	void SetFace(double theta, double phi);
	void setFace(Vector faceVector);
	void setStartPosition(Vector position);
	void setFinishPosition(Vector position);
	void setCoefficient(double speed_avg, double speed_dev, 
						double angular_avg, double angular_dev, double interval);
	
	int moveNext();


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
	Vector getFaceVector(double theta, double phi);
	double face_theta;		// projected angle to xy-plane
	double face_phi;		// angle to z-axis	
	Random *random;	




};

class ListenerMover
{
public:
	ListenerMover(SimulatorArgument *args);
	~ListenerMover(void);
	std::vector <PathInfo> pathList;


	

public:
	void reset();
	void reset(int width, int length, int height);
	void setPath(Vector point, double speed);
	bool moveNext(ListenerInfo &path);
	Vector getListenerPosition();
	Vector getListenerFace();

	


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

