#include <math.h>
#include "Cylinder.h"
#include "Sphere.h"

Cylinder::Cylinder()
{	
	Reset();
}
Cylinder::~Cylinder()
{
}

void Cylinder::Reset()
{
	vNormUnit = Vector();
	vNorm = Vector();
	minDistance = 0;
	maxDistance = 0;
	minHeight = 0;
	maxHeight = 0;
	pMin = Vector();
	pMax = Vector();
}


Cylinder::Cylinder(DistanceInfo &info1, DistanceInfo &info2, double measError)
{
	vPoint1 = info1.location;
	vPoint2 = info2.location;
	dist1 = info1.distance;
	dist2 = info2.distance;
	// vector from B1 to B2
	vNorm = vPoint2 - vPoint1;
	vNormUnit = vNorm.getUnitVector();
	double distance = vNorm.getSize();
	
	SetMinMax(info1, info2, measError);

	for (int i = -1; i <= 1; i+=2)
	{
		for (int j = -1; j <=1; j+=2)
		{
			double r1sqr = pow(info1.distance + (i*measError), 2);
			double r2sqr = pow(info2.distance + (j*measError), 2);
			double x = (r1sqr - r2sqr) / (2 * distance) + (distance / 2);
			double rsqr = r1sqr - pow(x,2);			
			const double resolution = 0.000000001;
			if (fabs(rsqr) < resolution) rsqr = 0.0;

			if (rsqr < 0)
			{
				continue;
//				printf("- (%d, %d) %f\t r1sqr=%f\tr2sqr=%f\tr=%f\n", i,j, distance, r1sqr, r2sqr,r);
				// x = (1/2t)x(r1^2 - r2^2) + (1/2)xt
			}

			double r = sqrt(rsqr);

			if (x < minDistance) minDistance = x;
			if (x > maxDistance) maxDistance = x;
			if (r < minHeight) minHeight = r;
			if (r > maxHeight) maxHeight = r;
		}
	}
	pMin = vPoint1 + vNormUnit * minDistance;
	pMax = vPoint1 + vNormUnit * maxDistance;
}

bool between(double a, double b, double c)
{
	return (a < b && b < c);
}


void Cylinder::SetMinMax(DistanceInfo &info1, DistanceInfo &info2, double measError)
{
	double distance = info1.location.getDistance(info2.location);
	
	double outer1l = points[0] = -info1.distance - measError;	// left location of outer circle of info 1
	double inner1l = points[1] = -info1.distance + measError;	// left location of inner circle
	double inner1r = points[2] = (info1.distance - measError);	// right inner
	double outer1r = points[3] = (info1.distance + measError);	// right outer
	double outer2l = points[4] = (distance - info2.distance - measError);
	double inner2l = points[5] = (distance - info2.distance + measError);
	double inner2r = points[6] = (distance + info2.distance - measError);
	double outer2r = points[7] = (distance + info2.distance + measError);

	double r1pmeas = pow(info1.distance + measError, 2);
	double r1mmeas = pow(info1.distance - measError, 2);
	double r2pmeas = pow(info2.distance + measError, 2);
	double r2mmeas = pow(info2.distance - measError, 2);
	double r1top_to_r2 = r1pmeas + pow(distance, 2);
	double r2top_to_r1 = r2pmeas + pow(distance, 2);
	if (r2mmeas < r1top_to_r2 && r1top_to_r2 < r2pmeas)
		maxHeight = info1.distance;
	else if (r1mmeas < r2top_to_r1 && r2top_to_r1 < r1pmeas)
		maxHeight = info2.distance;
	else maxHeight = 0;
	

	minHeight = info1.distance + measError;
	minDistance = outer2r;
	maxDistance = outer1l;
	if (between(inner1r, outer2l, outer1r))
		minDistance = outer2l;
	if (between(outer2l, outer1r, inner2l))
		maxDistance = outer1r;
	if (between(outer1l, outer2l, inner1l))
		minDistance = outer2l;
	if (between(outer2l, outer1l, inner2l))
		minDistance = outer1l;
	if (between(inner2r, outer1r, outer2r))
		maxDistance = outer1r;
	if (between(inner1r, outer2r, outer1r))
		maxDistance = outer2r;
}

			
bool Cylinder::CheckCollision(double refmin, double refmax, 
								double cmp1, double cmp2, double cmp3, double cmp4)
{
	double cmpmin = cmp1;
	double cmpmax = cmp1;
	
	if (cmp2 < cmpmin) cmpmin = cmp2;
	if (cmp3 < cmpmin) cmpmin = cmp3;
	if (cmp4 < cmpmin) cmpmin = cmp4;

	if (cmp2 > cmpmax) cmpmax = cmp2;
	if (cmp3 > cmpmax) cmpmax = cmp3;
	if (cmp4 > cmpmax) cmpmax = cmp4;
	
	if (refmin <= cmpmax && refmax >= cmpmin)
		return true;
	return false;
}

bool Cylinder::CheckCollision(Cylinder &cylinder)
{

	Vector vN = this->vNormUnit * cylinder.vNormUnit;
	Vector v1_reg = this->vNormUnit;
	Vector v1_per = (vN * v1_reg).getUnitVector();
	Vector v2_reg = cylinder.vNormUnit;
	Vector v2_per = (vN * v2_reg).getUnitVector();
	double d1_reg = v1_reg & this->vPoint1;
	double d1_per = v1_per & this->vPoint1;
	double d2_reg = v2_reg & cylinder.vPoint1;
	double d2_per = v2_per & cylinder.vPoint1;
	
	this->SetCollisionPoints(v1_per);
	cylinder.SetCollisionPoints(v2_per);

	bool chk;
	
	chk = CheckCollision(d1_reg + minDistance, d1_reg + maxDistance, 
					v1_reg & cylinder.pMin1, v1_reg & cylinder.pMin2,
					v1_reg & cylinder.pMax1, v1_reg & cylinder.pMax2);
	if (!chk) return false;

	chk = CheckCollision(d1_per - maxHeight, d1_per + maxHeight,
					v1_per & cylinder.pMin1, v1_per & cylinder.pMin2,
					v1_per & cylinder.pMax1, v1_per & cylinder.pMax2);
	if (!chk) return false;

	chk = CheckCollision(d2_reg + cylinder.minDistance, d2_reg + cylinder.maxDistance,
					v2_reg & pMin1, v2_reg & pMin2,
					v2_reg & pMax1, v2_reg & pMax2);
	if (!chk) return false;

	chk = CheckCollision(d2_per - cylinder.maxHeight, d2_per + cylinder.maxHeight,
					v2_per & pMin1, v2_per & pMin2,
					v2_per & pMax1, v2_per & pMax2);
	if (!chk) return false;

	return true;
}


void Cylinder::SetCollisionPoints(Vector& vPer)
{
	pMin1 = pMin + (vPer * maxHeight);
	pMin2 = pMin - (vPer * maxHeight);

	pMax1 = pMax + (vPer * maxHeight);
	pMax2 = pMax - (vPer * maxHeight);
}


bool Cylinder::CheckCollision(DistanceInfo &info, double measError)
{
	Vector vPlaneNorm;
	Vector vRectUnit;

	vPlaneNorm = vNorm * (info.location - vPoint2);
	if (vPlaneNorm.getSize() == 0)
		vPlaneNorm = Vector(0, 0, 1);

	vRectUnit = (vNorm * vPlaneNorm).getUnitVector();

	/* Four Rectang Point
	pMin +- (vRectUnit * maxHeight)
	pMax +- (vRectUnit * maxHeight)
	*/
	SetCollisionPoints(vRectUnit);

	return CheckCollision(pMin1, pMax1, pMax2, pMin2, info,measError);
}

// p1 through p4 should be ordered by clock-wise or counter clockwise
bool Cylinder::CheckCollision(Vector p1, Vector p2, Vector p3, Vector p4, DistanceInfo &info, double measError)
{
	Vector vertex[4];
	vertex[0] = p1;
	vertex[1] = p2;
	vertex[2] = p3;
	vertex[3] = p4;

	Sphere maxSphere = Sphere(info.location, info.distance + measError);
	Sphere minSphere = Sphere(info.location, info.distance - measError);

	//vertex inclusion
	for (int i = 0; i < 4; i ++)
	{
		if (maxSphere.CheckInclusion(vertex[i]) && !minSphere.CheckInclusion(vertex[i]))
			return true;
	}
	
	//edge inclusion
	for (int i = 0; i < 4; i ++)
	{
		if (maxSphere.CheckCollision (vertex[i], vertex[(i+1) % 4]) ||
				minSphere.CheckCollision (vertex[i], vertex[(i+1) % 4])) 
			return true;
	}
	return false;
}