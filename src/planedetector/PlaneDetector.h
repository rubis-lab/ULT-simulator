#pragma once
#include "Vector.h"
#include "Plane.h"
#include <vector>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_linalg.h>

class DetectionData
{
public:
	Vector vC;
	Vector vB;
	Vector vL;
	double fC;
	double fR;
public:
	DetectionData(Vector vListener, Vector vB, double fR);
	DetectionData(){}
	~DetectionData(){}

	double getF(Vector vN, double d);
	Vector getDF(Vector vN);
	double getF(double theta, double pi, double d);
	Vector getDF(double theta, double pi, double d);
	double getF(Vector vN, Vector vX1);
	Vector getDF(Vector vN, Vector vX1);
	double getF(double theta, double pi, Vector vX1);
	Vector getDF(double theta, double pi, Vector vX1);
	Vector getP(Vector vN);
	void print();
};

class PlaneDetector
{
public:
	PlaneDetector(Vector vListener);
	~PlaneDetector();

	std::vector<Vector> vP;
	Vector getReflectedPoint(Vector vPoint);
	double getDistanceError(Plane &realPlane);
	double getAngleError(Plane &realPlane);
	void addReflectedDistance(Vector vBeacon, double distance);
	Plane getPlane();
	
		
private:
	void solve();
	std::vector<DetectionData> data;
	Vector vN;
	double fD;
	Vector vListener;
	
};
