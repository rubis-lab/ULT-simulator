#pragma once
#include "KalmanFilter.h"


namespace KF
{
	enum mode
	{
		P,
		PV,
		pVA,
	};
}

class KFArgument
{
	KFArgument()
	{
		mode = KF::PV;
		timeSlot = 50;
		KFMeasError = 0.001;
		KFSystemError = 0.0008;
	}
	KF::mode mode;
	unsigned long timeSlot;			// in ms
	double KFMeasError;
	double KFSystemError;
};


class KFManager
{
public:
	KFManager(void);
	~KFManager(void);

	void setManager(KFArguement args);
	void getCorrectedResult(SolverResult *result);


private:

	void Predict();
	void Correct(Vector &measurement);
	void initFilter(Vector &measurement);	
	Vector GetX();
	Vector GetX0();
	double GetError();
	
	KalmanFilter KF[3];	
	int mode;
	double period;	
	double error;
	Vector measurement;


	void RootConfP(double period, double measErr, double systemErr);
	void RootConfPV(double period, double measErr, double systemErr);
	void RootConfPVA(double period, double measErr, double systemErr);
	void SetMatrix(KalmanFilter* KF, 
					gsl_matrix* A, gsl_matrix* B, gsl_matrix* H, 
					gsl_matrix* Q, gsl_matrix* R, gsl_matrix* P);


};
