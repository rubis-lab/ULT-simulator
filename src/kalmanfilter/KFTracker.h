#pragma once
#include "KFArgument.h"
#include "KalmanFilter.h"
#include "Solver.h"



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
