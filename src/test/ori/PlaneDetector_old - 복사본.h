#pragma once
#include "Vector.h"
#include "HeardList.h"
#include "Argument.h"
#include "Cylinder.h"
#include <vector>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_linalg.h>


class PlaneDetector
{
public:
	PlaneDetector();
	~PlaneDetector();

	Vector vN;
	Vector vP0;
	std::vector<Vector> vP;
	void GetPlane(HeardList &heardList, Vector vListener);
	Vector GetReflectedPoint(Vector vPoint);
	double GetDistanceError(HeardList &heardList, Plane &realPlane);
	double GetAngleError(Plane &realPlane);
	
		
private:
	Vector getPlaneNormal(HeardList& heardList, Vector vListener);
	Vector getPlaneOffset();
	FILE *fpA;
	FILE *fpD;
};