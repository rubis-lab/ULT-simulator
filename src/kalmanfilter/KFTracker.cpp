#include <math.h>
#include "KFTracker.h"


KFTracker::KFTracker(void)
{
}

KFTracker::~KFTracker(void)
{
}

KFTracker::KFTracker(KFTracker *tracker)
{
	this->mode = tracker->mode;
	this->period = tracker->period;
	for (int i = 0; i < 3; i++)
		tracker->KF[i].Duplicate(KF[i]);

}
KFTracker::KFTracker(KFArgument args)
{
	setTracker(args);
}

void KFTracker::setTracker(KFArgument args)
{	
	
	this->mode = args.mode;
	this->period = args.timeSlot/1000.0;
	double measErr = args.KFMeasError;
	double systemErr = args.KFSystemError;

	switch(mode)
	{
	case KF::P:		RootConfP(period, measErr, systemErr);		break;
	case KF::PV :	RootConfPV(period, measErr, systemErr);		break;
	case KF::PVA :	RootConfPVA(period, measErr, systemErr);	break;
	default : printf("unknown KFT mode\n"); getchar();exit(60);	break;
	}
	
}

void KFTracker::SetMatrix(KalmanFilter* KF, 
						  gsl_matrix* A, gsl_matrix* B, gsl_matrix* H, 
						  gsl_matrix* Q, gsl_matrix* R, gsl_matrix* P)
{
	KF->SetMatrix(KF_A, A);
	KF->SetMatrix(KF_B, B);
	KF->SetMatrix(KF_H, H);
	KF->SetMatrix(KF_Q, Q);
	KF->SetMatrix(KF_R, R);
	KF->SetMatrix(KF_P, P);
}


void KFTracker::RootConfP(double period, double measErr, double systemErr)
{
	for (int i = 0; i < 3; i++)
		KF[i].matrix_alloc(1, 1, 1);

	double posNoise = systemErr;
	double baseA [1] = {period/period};
	double baseB[1] = {0};
	double baseH [1] = {1};
	double baseP [1] = {0};
	double baseW [1] = {posNoise};
	double baseV [1] = {measErr};

	gsl_matrix_view A = gsl_matrix_view_array(baseA, 1, 1);
	gsl_matrix_view B = gsl_matrix_view_array(baseB, 1, 1);
	gsl_matrix_view H = gsl_matrix_view_array(baseH, 1, 1);
	gsl_matrix_view W = gsl_matrix_view_array(baseW, 1, 1);
	gsl_matrix_view V = gsl_matrix_view_array(baseV, 1, 1);
	gsl_matrix_view P = gsl_matrix_view_array(baseP, 1, 1);
	
	gsl_matrix* Q = gsl_matrix_alloc(1, 1);
	gsl_matrix* R = gsl_matrix_alloc(1, 1);
	
	gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, &W.matrix, &W.matrix, 0.0, Q);
	gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, &V.matrix, &V.matrix, 0.0, R);

	for (int i = 0; i < 3; i++)
		SetMatrix(&KF[i], &A.matrix, &B.matrix, &H.matrix, Q, R, &P.matrix);	

	gsl_matrix_free(Q);
	gsl_matrix_free(R);
}

void KFTracker::RootConfPV(double period, double measErr, double systemErr)
{	
	for (int i = 0; i < 3; i++)
		KF[i].matrix_alloc(2, 1, 1);

	double p = period;
	double velNoise = systemErr;
	double posNoise = p * systemErr;
	double baseA[4] = {1, p,
					   0, 1};						
	double baseB[2] = {0, 0};
	double baseH[2] = {1, 0};
	double baseP[4] = {0, 0,
					   0, 0};
	double baseW[2] = {posNoise, velNoise};		
	double baseV[1] = {measErr};

	gsl_matrix_view A = gsl_matrix_view_array(baseA, 2, 2);
	gsl_matrix_view B = gsl_matrix_view_array(baseB, 2, 1);
	gsl_matrix_view H = gsl_matrix_view_array(baseH, 1, 2);
	gsl_matrix_view W = gsl_matrix_view_array(baseW, 2, 1);
	gsl_matrix_view V = gsl_matrix_view_array(baseV, 1, 1);
	gsl_matrix_view P = gsl_matrix_view_array(baseP, 2, 2);
	
	gsl_matrix* Q = gsl_matrix_alloc(2, 2);
	gsl_matrix* R = gsl_matrix_alloc(1, 1);
	
	gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, &W.matrix, &W.matrix, 0.0, Q);
	gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, &V.matrix, &V.matrix, 0.0, R);
	
	for (int i = 0; i < 3; i++)
		SetMatrix(&KF[i], &A.matrix, &B.matrix, &H.matrix, Q, R, &P.matrix);	

	gsl_matrix_free(Q);
	gsl_matrix_free(R);
}

void KFTracker::RootConfPVA(double period, double measErr, double systemErr)
{
	mode = KF::PVA;
	for (int i = 0; i < 3; i++)
		KF[i].matrix_alloc(3, 1, 1);

	double p = period;
	double p2h = p * p * 0.5;
	double accNoise = systemErr;
	double velNoise = p * accNoise;
	double posNoise = p * p * accNoise * 0.5;
	double baseA[9] = {1, p, p2h,
					   0, 1, p,
					   0, 0, 1};						
	double baseB[3] = {0, 0, 0};
	double baseH[3] = {1, 0, 0};
	double baseP[9] = {0, 0, 0,
		               0, 0, 0,
					   0, 0, 0};
	double baseW[3] = {posNoise, velNoise, accNoise};		
	double baseV[1] = {measErr};

	gsl_matrix_view A = gsl_matrix_view_array(baseA, 3, 3);
	gsl_matrix_view B = gsl_matrix_view_array(baseB, 3, 1);
	gsl_matrix_view H = gsl_matrix_view_array(baseH, 1, 3);
	gsl_matrix_view W = gsl_matrix_view_array(baseW, 3, 1);
	gsl_matrix_view V = gsl_matrix_view_array(baseV, 1, 1);
	gsl_matrix_view P = gsl_matrix_view_array(baseP, 3, 3);
	
	gsl_matrix* Q = gsl_matrix_alloc(3, 3);
	gsl_matrix* R = gsl_matrix_alloc(1, 1);
	
	gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, &W.matrix, &W.matrix, 0.0, Q);
	gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, &V.matrix, &V.matrix, 0.0, R);
	
	for (int i = 0; i < 3; i++)	
		SetMatrix(&KF[i], &A.matrix, &B.matrix, &H.matrix, Q, R, &P.matrix);	

	gsl_matrix_free(Q);
	gsl_matrix_free(R);
}

Vector KFTracker::predict()
{
	for (int i = 0; i < 3; i++)
		KF[i].fast_predict();
	
	return getX0();
	
}

Vector KFTracker::correct(SolverResult *result)
{
	Vector location = result->location;
	double baseZ[3] = {location.x, location.y, location.z};


//	error = 1;
	for (int i = 0; i < 3; i++)
	{
		gsl_matrix_view Z = gsl_matrix_view_array(baseZ + i, 1, 1);
		KF[i].correct(&Z.matrix);
//		error *= KF[i].pdf;
	}


	return getX();


	// TODO:
	//1.to be sure getX get correction value.
	//2.to be sure getX0 get predicted value.
	//3.current logic :
	//predict -> correct, then check predicted value is near result
	//that means it finds most predictable result and then correct it
	//to fast it's process, first predict, 
	//and find most proper result, 
	//finally correct it.
	

}

void KFTracker::initTracker(SolverResult *result)
{
	Vector location = result->location;

	double baseX[9] = {location.x, 0, 0, 
					   location.y, 0, 0, 
					   location.z, 0, 0};
	gsl_matrix_view X[3];

	int size1;
	switch(mode)
	{
	case KF::P	:	size1 = 1;	break;
	case KF::PV :	size1 = 2;	break;
	case KF::PVA :	size1 = 3;	break;
	default : size1 = 0;
	}
	for (int i = 0; i < 3; i++)
	{
		X[i] = gsl_matrix_view_array(baseX + (i * 3), size1, 1);
		KF[i].SetMatrix(KF_X, &X[i].matrix);		
	}

	predict();
	//TODO:
	//predict procee will excuted in next step.
	//it doesn't need to be excuted here.

	Vector vPredict = getX0();
	result->setPredictedLocation(vPredict);


	result->setCorrectedLocation(vPredict);
}

Vector KFTracker::getX()
{
	return Vector(gsl_matrix_get(KF[0].m[KF_X], 0, 0), 
				  gsl_matrix_get(KF[1].m[KF_X], 0, 0), 
				  gsl_matrix_get(KF[2].m[KF_X], 0, 0));
}

Vector KFTracker::getX0()
{
	return Vector(gsl_matrix_get(KF[0].m[KF_X0], 0, 0), 
				  gsl_matrix_get(KF[1].m[KF_X0], 0, 0), 
				  gsl_matrix_get(KF[2].m[KF_X0], 0, 0));

}


double round(double val)
{
	
	return (int)(val *1000) / 1000.0;
//	return (double)((int)(val));
}



bool KFTracker::IsSame(KFTracker* tracker)
{
	for (int i = 0; i < 3; i++)
	{
		if (round(gsl_matrix_get(KF[i].m[KF_X], 0, 0)) !=
			round(gsl_matrix_get(tracker->KF[i].m[KF_X], 0, 0)))
			return false;
		for (size_t j = 0; j < KF[i].m[KF_P]->size1; j++)
		{
			for (size_t k = 0; k < KF[i].m[KF_P]->size2; k++)
			{
				if (round(gsl_matrix_get(KF[i].m[KF_P], j, k)) !=
					round(gsl_matrix_get(tracker->KF[i].m[KF_P], j, k)))
					return false;
			}
		}
	}
	return true;
}

