#pragma once
#include "Vector.h"
#include "HeardList.h"

class Cylinder
{
public:
	Cylinder();
	Cylinder(DistanceInfo &info1, DistanceInfo &info2, double measError);
	~Cylinder();

	void Reset();

	Vector vNormUnit;
	Vector vNorm;
	double minDistance;
	double maxDistance;
	double minHeight;
	double maxHeight;
	Vector pMin;
	Vector pMax;
	Vector pMin1, pMin2;
	Vector pMax1, pMax2;
	Vector vPoint1, vPoint2;
	double dist1, dist2;
	double measError;

	bool CheckCollision(double refmin, double refmax, double cmp1, double cmp2, double cmp3, double cmp4);
	bool CheckCollision(Cylinder &cylinder);
	bool CheckCollision(DistanceInfo &info, double measError);
	bool CheckCollision(Vector p1, Vector p2, Vector p3, Vector p4, DistanceInfo &info, double measError);
	void SetCollisionPoints(Vector& vN);


private:
	double points[8];

	void SetMinMax(DistanceInfo &info1, DistanceInfo &info2, double measError);

};