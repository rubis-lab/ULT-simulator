#include <math.h>
#include "Sphere.h"


Sphere::Sphere(Vector center_, double radius_)
{
	center = center_;
	radius = radius_;
}


Sphere::~Sphere(void)
{
}


bool Sphere::CheckInclusion (Vector point)
{
	return center.getDistance(point) <= radius;
}

bool Sphere::CheckCollision (Vector p1, Vector p2)
{
	Vector vEdge = p2 - p1;
	Vector vEdgeUnit = vEdge.getUnitVector();
	double lEdge = vEdge.getSize();

	double length = vEdgeUnit & (center - p1);

	Vector vPL = p1 + vEdgeUnit * length;	// perpendicular leg
	double distPL = vPL.getDistance(center);	// distance from perpendicular leg to center

	if (distPL > radius) return false;		// no intersection
	double dist = sqrt(pow(radius, 2) - pow(distPL, 2));	// distance from perpendicular leg to intersection points
	// two relative distance from v1 to intersected points
	double relDist1 = length - dist;
	double relDist2 = length + dist;

	// intersection points should be placed between p1 and p2
	if ((relDist1 < 0 || relDist1 > lEdge) &&
		(relDist2 < 0 || relDist2 > lEdge)) return false;

	return true;
}
