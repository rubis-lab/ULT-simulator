#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include "KalmanFilter.h"


KalmanFilter::KalmanFilter(void)
{
	memset(m, 0, sizeof(gsl_matrix*) * N_KF);
}

KalmanFilter::KalmanFilter(int nX, int nU, int nZ)
{
	matrix_alloc(nX, nU, nZ);
}

KalmanFilter::KalmanFilter(KalmanFilter *kf)
{

	for (int i = 0; i < N_KF; i++)
	{
		this->m[i] = gsl_matrix_alloc(kf->m[i]->size1, kf->m[i]->size2);
		gsl_matrix_memcpy(this->m[i], kf->m[i]);
	}	
}


KalmanFilter::~KalmanFilter(void)
{
	matrix_free();
}

void KalmanFilter::matrix_alloc(int nX, int nU, int nZ)
{		
	m[KF_X] = gsl_matrix_alloc(nX, 1);
	m[KF_X0] = gsl_matrix_alloc(nX, 1);
	m[KF_U] = gsl_matrix_alloc(nU, 1);

	m[KF_A] = gsl_matrix_alloc(nX, nX);
	m[KF_B] = gsl_matrix_alloc(nX, nU);
	m[KF_H] = gsl_matrix_alloc(nZ, nX);

	m[KF_Q] = gsl_matrix_alloc(nX, nX);
	m[KF_R] = gsl_matrix_alloc(nZ, nZ);

	m[KF_P] = gsl_matrix_alloc(nX, nX);
	m[KF_P0] = gsl_matrix_alloc(nX, nX);
}

void KalmanFilter::matrix_free()
{
	for (int i = 0; i < N_KF; i++)
	{
		if (m[i] != NULL)
			gsl_matrix_free(m[i]);
	}
	memset(m, 0, sizeof(gsl_matrix*) * N_KF);
}

/*
void KalmanFilter::predict()
{
//	double alpha = 1.0, beta = 0.0;
*/	
	/*
	gsl_blas_sgemm(TransA, TransB, alpha, A, B, beta, C)
	C = alpha * OP(A) * OP(B) + beta * C
	OP(A) = A, A^T, A^H for TransA = CblasNoTrans, CblasTrans, CblasConjTrans
	*/
/*	
	//X0 = (A * X) + (B * U)
	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, m[KF_A], m[KF_X], 0.0, m[KF_X0]);		// X0 = A * X
	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, m[KF_B], m[KF_U], 1.0, m[KF_X0]);		// X0 = B * U + X0


	// P0 = A * P * A^T +  B * U * B^T + Q
	gsl_matrix *AP = gsl_matrix_alloc(m[KF_A]->size1, m[KF_P]->size2);
	gsl_matrix *BU = gsl_matrix_alloc(m[KF_B]->size1, m[KF_U]->size2);

	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, m[KF_A], m[KF_P], 0.0, AP);		// AP = A * P
	gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, AP, m[KF_A], 0.0, m[KF_P0]);		// P0 = A * P * A^T
	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, m[KF_B], m[KF_U], 0.0, BU);		// BU = B * U
	gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, BU, m[KF_B], 1.0, m[KF_P0]);		// P0 = BU * B^T + P0
	gsl_matrix_add(m[KF_P0], m[KF_Q]);
	
	gsl_matrix_free(BU);
	gsl_matrix_free(AP);	
}
*/
void modify_singular(gsl_matrix *LU)
{
	if (LU->size1 != LU->size2) return;
	for (size_t i = 0; i < LU->size1; i++)
	{
		if (gsl_matrix_get(LU, i, i) == 0)
			gsl_matrix_set(LU, i, i, 0.00001);
	}
}
void KalmanFilter::fast_predict()
{
	//X0 = (A * X)
	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, m[KF_A], m[KF_X], 0.0, m[KF_X0]);		// X0 = A * X
	//PrintMatrix("X0 = A * X", m[KF_X0]);
	//PrintMatrix("X", m[KF_X]);

	// P0 = A * P * A^T + Q
	gsl_matrix *AP = gsl_matrix_alloc(m[KF_A]->size1, m[KF_P]->size2);

	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, m[KF_A], m[KF_P], 0.0, AP);		// AP = A * P
	gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, AP, m[KF_A], 0.0, m[KF_P0]);		// P0 = A * P * A^T
	gsl_matrix_add(m[KF_P0], m[KF_Q]);	

	//PrintMatrix("A", m[KF_A]);
	//PrintMatrix("P", m[KF_P]);
	//PrintMatrix("Q", m[KF_Q]);
	//PrintMatrix("P0 = A * P * A' + Q", m[KF_P0]);
	
	gsl_matrix_free(AP);	
}


void KalmanFilter::correct(gsl_matrix *Z)
{
	gsl_matrix *K = gsl_matrix_alloc(m[KF_P]->size1, m[KF_H]->size1);

	// S = H * P0 * H^T + R
	gsl_matrix *P0Ht = gsl_matrix_alloc(m[KF_P0]->size1, m[KF_H]->size1);
	gsl_matrix *S = gsl_matrix_alloc(m[KF_H]->size1, m[KF_H]->size1);
	gsl_matrix *Si = gsl_matrix_alloc(m[KF_H]->size1, m[KF_H]->size1);
	gsl_permutation *p = gsl_permutation_alloc(m[KF_H]->size1);
	int s;

	gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, m[KF_P0], m[KF_H], 0.0, P0Ht);
	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, m[KF_H], P0Ht, 0.0, S);
	gsl_matrix_add(S, m[KF_R]);
	
	gsl_matrix *Sdup = gsl_matrix_alloc(S->size1, S->size2);
	gsl_matrix_memcpy(Sdup, S);
	gsl_linalg_LU_decomp(Sdup, p, &s);
	modify_singular(Sdup);
	gsl_linalg_LU_invert(Sdup, p, Si);
		
	
	// K = P0 * H^T * S^-1
	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, P0Ht, Si, 0.0, K);


	// X = X0 + K * (Z - H * X0)
	gsl_matrix *innov = gsl_matrix_alloc(m[KF_H]->size1, m[KF_X0]->size2);		

	gsl_matrix_memcpy(innov, Z);
	gsl_matrix_memcpy(m[KF_X], m[KF_X0]);

	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, -1.0, m[KF_H], m[KF_X0], 1, innov);	// innov = (-H*X0 + Z)
	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, K, innov, 1.0, m[KF_X]);	// X = K * innov + X0

	// P = (I - (K * H)) * P0
	gsl_matrix *I = gsl_matrix_alloc(K->size1, m[KF_H]->size2);
	gsl_matrix_set_identity(I);

	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, -1.0, K, m[KF_H], 1.0, I);
	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, I, m[KF_P0], 0, m[KF_P]);


	// calculate conditional probability density function to calculate likelihood
	gsl_matrix *exponent = gsl_matrix_alloc(innov->size1, innov->size1);
	gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, innov, Si, 0, exponent);
	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, exponent, innov, 0, exponent);
	if (exponent->size1 != 1 || exponent->size2 != 1 || S->size1 != 1 || S->size2 != 1)
	{
		printf("conditional pdf error\n");
		getchar();
		exit(32);
	}
	pdf = 1.0 / (pow(2 * M_PI,(double)m[KF_X]->size1) * sqrt(gsl_matrix_get(S, 0, 0))) *
		exp(-0.5 * gsl_matrix_get(exponent, 0, 0));



	gsl_permutation_free(p);
	gsl_matrix_free(Si);
	gsl_matrix_free(S);
	gsl_matrix_free(P0Ht);	

	gsl_matrix_free(innov);

	gsl_matrix_free(I);
	gsl_matrix_free(K);	

	gsl_matrix_free(exponent);

}

void KalmanFilter::Duplicate(KalmanFilter &kFilter)
{
	kFilter.matrix_free();
	for (int i = 0; i < N_KF; i++)
	{
		kFilter.m[i] = gsl_matrix_alloc(m[i]->size1, m[i]->size2);
		gsl_matrix_memcpy(kFilter.m[i], m[i]);
	}
}

void KalmanFilter::SetMatrix(int mIdx, gsl_matrix *M)
{
	if (mIdx >= N_KF) return;
	if (m[mIdx]->size1 != M->size1 || m[mIdx]->size2 != M->size2)
	{
		printf("error\n");
		getchar();
		exit(31);
	}

//	if (m[mIdx] != NULL) gsl_matrix_free(m[mIdx]);
	gsl_matrix_memcpy(m[mIdx], M);
}


/*
double KalmanFilter::GetError()
{	
	double d1 = gsl_matrix_get(m[KF_P], 0, 0);
//	double d2 = gsl_matrix_get(m[KF_P], 1, 1);
//	double d3 = gsl_matrix_get(m[KF_P], 2, 2);

//	double dd1 = gsl_matrix_get(m[KF_P0], 0, 0);
//	double dd2 = gsl_matrix_get(m[KF_P0], 1, 1);
//	double dd3 = gsl_matrix_get(m[KF_P0], 2, 2);


	return d1;
//	return sqrt(fabs(d1) + fabs(d2) + fabs(d3));
//	return sqrt(d1 + d2 + d3);
//	return sqrt(d1 * d1 + d2 * d2 + d3 * d3);
//	return sqrt(dd1 + dd2 + dd3);
}
*/


/*
void KalmanFilter::fast_correct(gsl_matrix *Z)
{
	gsl_matrix *K = gsl_matrix_alloc(m[KF_P]->size1, m[KF_H]->size1);

	// S = P0 + R
	gsl_matrix *S = gsl_matrix_alloc(m[KF_H]->size1, m[KF_H]->size1);
	gsl_matrix *Si = gsl_matrix_alloc(m[KF_H]->size1, m[KF_H]->size1);
	gsl_permutation *p = gsl_permutation_alloc(m[KF_H]->size1);
	int s;

	gsl_matrix_memcpy(S, m[KF_P0]);
	gsl_matrix_add(S, m[KF_R]);

	gsl_linalg_LU_decomp(S, p, &s);
	gsl_linalg_LU_invert(S, p, Si);
		
	
	// K = P0 * S^-1
	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, m[KF_P0], Si, 0.0, K);

	gsl_permutation_free(p);
	gsl_matrix_free(Si);
	gsl_matrix_free(S);

	// X = X0 + K * (Z - X0)
	gsl_matrix *innov = gsl_matrix_alloc(m[KF_H]->size1, m[KF_X0]->size2);	

	gsl_matrix_memcpy(innov, Z);
	gsl_matrix_sub(Z, m[KF_X0]);
	gsl_matrix_memcpy(m[KF_X], m[KF_X0]);

	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, K, innov, 1.0, m[KF_X]);	// X = K * innov + X0

	gsl_matrix_free(innov);

	// P = (I - K ) * P0
	gsl_matrix *I = gsl_matrix_alloc(K->size1, m[KF_H]->size2);
	gsl_matrix_set_identity(I);

	gsl_matrix_sub(I, K);
	gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, I, m[KF_P0], 0, m[KF_P]);
	
	gsl_matrix_free(I);
	gsl_matrix_free(K);	

}
*/
/*
Vector KalmanFilter::GetX0()
{
	Vector vX0;
	vX0.x = gsl_matrix_get(m[KF_X0], 0, 0);
//	vX0.y = gsl_matrix_get(m[KF_X0], 1, 0);
//	vX0.z = gsl_matrix_get(m[KF_X0], 2, 0);
	return vX0;
}

Vector KalmanFilter::GetX()
{
	return gsl_matrix_get(m[KF_X], 0, 0);

	Vector vX;
	vX.x = gsl_matrix_get(m[KF_X], 0, 0);
//	vX.y = gsl_matrix_get(m[KF_X], 1, 0);
//	vX.z = gsl_matrix_get(m[KF_X], 2, 0);
	return vX;
}
*/

void KalmanFilter::PrintMatrix(char *m_name, gsl_matrix *matrix)
{
	printf("%s\n", m_name);
	for (size_t i = 0; i < matrix->size1; i++)
	{
		for (size_t j =0; j < matrix->size2; j++)
		{
			printf("%.4f\t", gsl_matrix_get(matrix, i, j));			
		}
		printf("\n");
	}			

}
