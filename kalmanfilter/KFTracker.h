#pragma once
#include "KalmanFilter.h"
#include "Solver.h"

//const double measErr = 0.01;
const double measErr = 0.001;
//const double systemErr = 1.5;
//const double speedErr = 1.5;
//const double accNoise = 1200;
const double systemErr = 0.0008;
// 시간에 대한 matrix로 변경

namespace KF
{
	enum mode
	{
		P = 0,
		PV,
		PVA,
	};
}


class KFArgument
{
public:
	KFArgument()
	{
		mode = KF::PV;
		timeSlot = 50;
		KFMeasError = 0.001;
		KFSystemError = 0.0008;
	}

	KF::mode mode;
	unsigned long timeSlot;		// in ms
	double KFMeasError;
	double KFSystemError;
};


class KFTracker
{
public:
	KFTracker(void);
	KFTracker(KFTracker *tracker);
	KFTracker(KFArgument args);
	~KFTracker(void);

	void setTracker(KFArgument args);
	Vector predict();
	Vector correct(SolverResult *result);
	void initTracker(SolverResult *result);	
	Vector getX();
	Vector getX0();
	
	KalmanFilter KF[3];	
	int mode;
	double period;	
	bool IsSame(KFTracker *tracker);

	SolverResult* getRecentResult();


private:
	void RootConfP(double period, double measErr, double systemErr);
	void RootConfPV(double period, double measErr, double systemErr);
	void RootConfPVA(double period, double measErr, double systemErr);
	void SetMatrix(KalmanFilter* KF, 
					gsl_matrix* A, gsl_matrix* B, gsl_matrix* H, 
					gsl_matrix* Q, gsl_matrix* R, gsl_matrix* P);

	SolverResult *recentResult;


};
