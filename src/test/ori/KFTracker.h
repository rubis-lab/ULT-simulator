#pragma once
#include "Argument.h"
#include "KalmanFilter.h"
#define KFM_P	0
#define KFM_PV	1
#define KFM_PVA	2

//const double measErr = 0.01;
const double measErr = 0.001;
//const double systemErr = 1.5;
//const double speedErr = 1.5;
//const double accNoise = 1200;
const double systemErr = 0.0008;
// 시간에 대한 matrix로 변경


class KFTracker
{
public:
	KFTracker(void);
	KFTracker(KFTracker *tracker);
	~KFTracker(void);

	void SetTracker(Argument& args);
	void Predict();
	void Correct(Vector &measurement);
	void InitTracker(Vector &measurement);	
	Vector GetX();
	Vector GetX0();
	double GetError();
	
	KalmanFilter KF[3];	
	int mode;
	double period;	
	double error;
	Vector measurement;
	bool IsSame(KFTracker *tracker);


private:
	void RootConfP(double period, double measErr, double systemErr);
	void RootConfPV(double period, double measErr, double systemErr);
	void RootConfPVA(double period, double measErr, double systemErr);
	void SetMatrix(KalmanFilter* KF, 
					gsl_matrix* A, gsl_matrix* B, gsl_matrix* H, 
					gsl_matrix* Q, gsl_matrix* R, gsl_matrix* P);


};
