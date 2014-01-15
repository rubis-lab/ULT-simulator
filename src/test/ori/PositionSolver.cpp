//#include "jama/jama_qr.h"
#include <string.h>
#include <math.h>
#include "PositionSolver.h"

#define min(a,b)            (((a) < (b)) ? (a) : (b))



PositionSolver::PositionSolver(Argument args)
{
	lsq_threshold = LSQ_THRESHOLD;
	validPrevPosEx = false;
	cnt = 0;
	this->args = args;
}

PositionSolver::~PositionSolver()
{
}

void PositionSolver::SetLSQThreshold(double threshold)
{
	lsq_threshold = threshold;
}


double PositionSolver::GetError(HeardList &heardList, Vector &point)
{
	double err = 0;
	for (size_t i = 0; i<heardList.heardInfo.size(); i++)
	{
		err += pow(heardList.heardInfo[i].distance 
					- (heardList.heardInfo[i].location.getDistance(point)),2);
	}
	return err;
}

Vector PositionSolver::InvalidPosition()
{
	return Vector(MAX_VAL, MAX_VAL, MAX_VAL);
}
bool PositionSolver::IsInvalidPosition(Vector &pos)
{	
	return pos.isEqual(InvalidPosition());
}


	
bool PositionSolver::CheckBranchCutoff(DistanceInfo &info1, DistanceInfo &info2)
{
	double a = info1.location.getDistance(info2.location);
	double b = info1.distance;
	double c = info2.distance;

	double d,e,f;

	if (a > b)	
	{
		if (a > c)	{
			if (b > c)	{	d = a;	e = b;	f = c;	} 	// a, b, c
			else		{	d = a;	e = c;	f = b;	}} 	// a, c, b
		else			{	d = c;	e = a;	f = b;	}	// c, a, b
	}
	else 
	{ 
		if (a > c)		{	d = b;	e = a;	f = c;	}	// b, a, c
		else {if (b > c){	d = b;	e = c;	f = a;	}	// b, c, a
			else		{	d = c;	e = b;	f = a;	}}	// c, b, a
	}

	return d <= e + f + args.MaxMeasError * 2;
}
		



bool PositionSolver::ExLeastSquareSolver(HeardList &heardList, std::vector<Plane> &planeList, int **reflection, int bid, int ref_cnt,
										   SolvedPosition &solvedPosition)
{
	if (bid == (int)heardList.heardInfo.size()) 
	{		
		double minErr = -1;
		/*T1{*/args.analyzer->SolverSolving.StartTimer();	/* start */
		/**/Vector minPoint = NLLeastSquareSolver(heardList, &minErr);
		/*}T1*/args.analyzer->SolverSolving.StopTimer();	/* stop */
//		/**/Vector minPoint = LeastSquareSolver2D(heardList, &minErr);

		cnt ++;
		solvedPosition.SetPosition(minPoint, minErr);
		positionInfo.push_back(solvedPosition);
		return true;
	}
	if (ref_cnt == REFLECTION_LIMIT)	
	{
		return ExLeastSquareSolver(heardList, planeList, reflection, bid + 1, 0, solvedPosition);
	}
	if ((ref_cnt > 0 && reflection[bid][ref_cnt] != 0)
		&& (reflection[bid][ref_cnt] == reflection[bid][ref_cnt - 1] 
			|| reflection[bid][ref_cnt - 1] == 0)) 
			{
				return false;
			}

	HeardList heardListReflection(heardList.l_id);
	heardList.Duplicate(heardListReflection);

	for (size_t i = 0; i < planeList.size() + 1; i++)
	{
		SolvedPosition nextPosition = solvedPosition.Duplicate();
		nextPosition.AddHistory(bid, i);

		reflection[bid][ref_cnt] = i;
		if (i == 0)
		{
			heardListReflection.heardInfo[bid].location = heardList.heardInfo[bid].location;
		}
		else
		{
			int pid = i - 1;
			double dist = planeList[pid].getDistanceToPoint(heardList.heardInfo[bid].location);

			if (dist < MIN_SEP || dist >= heardList.heardInfo[bid].distance) 
				continue;
			heardListReflection.heardInfo[bid].location = 
				planeList[pid].GetReflectedPoint(heardList.heardInfo[bid].location);

		}
		

		/* check Branch Cutting off possibility */
		bool fgCheck = true;
		if (args.Optimization & OPT::BRANCHCUT)
		{

			for (int j = 0; j < bid; j++)
			{
				if (!CheckBranchCutoff(heardListReflection.heardInfo[bid],
							heardListReflection.heardInfo[j]))
				{
					fgCheck = false;
					break;
				}
			}

		}
		if (fgCheck && (args.Optimization & OPT::BRANCHCUT_2) && bid > 2)
		{
			std::vector <Cylinder> cylinderList;
			cylinderList.empty();

			for (int j = 1; j < bid; j++)
			{
				Cylinder cl = Cylinder(heardListReflection.heardInfo[j - 1], 
										heardListReflection.heardInfo[j], 
										args.MaxMeasError);
				cylinderList.push_back(cl);
			}

			// cylinder with circle
			for (size_t j = 0; j < cylinderList.size(); j++)
			{
				if (!cylinderList[j].CheckCollision(heardListReflection.heardInfo[bid], args.MaxMeasError))
				{
					fgCheck = false;
					break;
				}
			}
/*
			// cylinder with cylinder
			for (size_t j = 1; j < cylinderList.size(); j++)
			{
				if (!cylinderList[j-1].CheckCollision(cylinderList[j]))
					fgCheck = false;
				break;
			}
*/
		}



		if (fgCheck)
		{
			ExLeastSquareSolver(heardListReflection,planeList, reflection, bid, ref_cnt + 1, nextPosition);
		}
	}
	reflection[bid][ref_cnt] = 0;

	return true;
}






void PositionSolver::ExLeastSquareSolver(HeardList &heardList, std::vector<Plane> &planeList)
{

	int **reflection =  (int**)calloc(sizeof(int*), heardList.heardInfo.size());
	for (unsigned int i = 0; i < heardList.heardInfo.size(); i++)
		reflection[i] = (int*)calloc(sizeof(int), REFLECTION_LIMIT);


	positionInfo.clear();

	SolvedPosition position;
	ExLeastSquareSolver(heardList, planeList, reflection, 0, 0, position);
	for (unsigned int i = 0; i < heardList.heardInfo.size(); i++)
		free (reflection[i]);
	free (reflection);	
}




/***********************************
//
//  Non Linear Least Square Solver
//
// ***********************************/

int func_f(const gsl_vector *x, void *data, gsl_vector *f)
{
	HeardList *heardList = (HeardList*)data;
	double x_h = gsl_vector_get(x, 0);
	double y_h = gsl_vector_get(x, 1);
	double z_h = gsl_vector_get(x, 2);

	for (size_t i = 0; i < heardList->heardInfo.size(); i++)
	{
		gsl_vector_set(f, i, 
			pow(x_h - heardList->heardInfo[i].location.x, 2) + 
			pow(y_h - heardList->heardInfo[i].location.y, 2) +
			pow(z_h - heardList->heardInfo[i].location.z, 2) -
			pow(heardList->heardInfo[i].distance, 2));
	}
	return GSL_SUCCESS;
}

int func_df(const gsl_vector *x, void *data, gsl_matrix *J)
{
	HeardList *heardList = (HeardList*)data;
	double x_h = gsl_vector_get(x, 0);
	double y_h = gsl_vector_get(x, 1);
	double z_h = gsl_vector_get(x, 2);

	int i = 0;
	for (size_t iter = 0; iter < heardList->heardInfo.size(); iter++)
	{
		if (heardList->heardInfo[iter].valid)
		{
			gsl_matrix_set(J, i, 0, 2 * (x_h - heardList->heardInfo[iter].location.x));
			gsl_matrix_set(J, i, 1, 2 * (y_h - heardList->heardInfo[iter].location.y));
			gsl_matrix_set(J, i, 2, 2 * (z_h - heardList->heardInfo[iter].location.z));
			i++;		
		}
	}
	if (heardList->n_valid != i)
	{
		getchar();
		exit(22);
	}

/*
	for (int i = 0; i < (int)heardList->heardInfo.size(); i++)
	{
		gsl_matrix_set(J, i, 0, 2 * (x_h - heardList->heardInfo[i].location.x));
		gsl_matrix_set(J, i, 1, 2 * (y_h - heardList->heardInfo[i].location.y));
		gsl_matrix_set(J, i, 2, 2 * (z_h - heardList->heardInfo[i].location.z));

	}
*/
	return GSL_SUCCESS;
}

int func_fdf(const gsl_vector *x, void *data, gsl_vector *f, gsl_matrix *J)
{
	func_f(x, data, f);
	func_df(x, data, J);
	return GSL_SUCCESS;
}

Vector PositionSolver::NLLeastSquareSolver(HeardList &heardList, double *err)
{
	const gsl_multifit_fdfsolver_type *T;
	gsl_multifit_fdfsolver *s;
	gsl_multifit_function_fdf f;
	
	double x_init[3] = {0.0, 0.0, 150.0};
/*
	x_init[0] = heardList.initPosition.x;
	x_init[1] = heardList.initPosition.y;
	x_init[2] = heardList.initPosition.z;
*/
/*
	if (validPrevPosEx)
	{
		x_init[0] = prevPositionExLSQ.x;
		x_init[1] = prevPositionExLSQ.y;
		x_init[2] = prevPositionExLSQ.z;
	}
*/
	gsl_vector_view x = gsl_vector_view_array(x_init, 3);

//	int n = (int)heardList.n_valid;
	int n = (int)heardList.heardInfo.size();

	// sort restriction..
//	n = 3;

	int p = 3;

	f.f = &func_f;
	f.df = &func_df;
	f.fdf = &func_fdf;
	f.n = n;
	f.p = p;	
	f.params = &heardList;

	T = gsl_multifit_fdfsolver_lmsder;
	s = gsl_multifit_fdfsolver_alloc(T, n, p);
	gsl_multifit_fdfsolver_set(s, &f, &x.vector);

	gsl_vector *gradt = gsl_vector_alloc(p);

	int iter = 0;
	int status;
	do
	{
		iter ++;
		status = gsl_multifit_fdfsolver_iterate(s);
		if (status) break;
///		status = gsl_multifit_test_delta(s->dx, s->x, 1e-4, 1e-4);
		gsl_multifit_gradient(s->J, s->f, gradt);
		status = gsl_multifit_test_gradient(gradt, 1e-6);
	}
	while (status == GSL_CONTINUE && iter < 500);

	

	Vector point = Vector(gsl_vector_get(s->x, 0), gsl_vector_get(s->x, 1), gsl_vector_get(s->x, 2));
	*err = GetError(heardList, point);

	gsl_vector_free(gradt);
	gsl_multifit_fdfsolver_free(s);


	return point;	
}



Vector PositionSolver::LeastSquareSolver(HeardList &heardList, double *err)
{
	gsl_matrix *matrix_a;
	gsl_vector *vector_x, *vector_b;
	size_t heardSize = heardList.heardInfo.size();

	if (heardSize < 4) 
	{
		return LeastSquareSolver2D(heardList, err);
	}

	matrix_a = gsl_matrix_alloc(heardSize - 1, 3);
	vector_x = gsl_vector_alloc(3);
	vector_b = gsl_vector_alloc(heardSize - 1);

	/* set matrix A, Vector B */
	Vector vOrigin = heardList.heardInfo[0].location;
	for (size_t i = 0; i < heardSize - 1; i++)
	{
		Vector vLocation = heardList.heardInfo[i+1].location;
		Vector vDiff = vLocation - vOrigin;
		gsl_matrix_set(matrix_a, i, 0, 2 * vDiff.x);
		gsl_matrix_set(matrix_a, i, 1, 2 * vDiff.y);
		gsl_matrix_set(matrix_a, i, 2, 2 * vDiff.z);
		
		gsl_vector_set(vector_b, i, vLocation.getSqrSize() - vOrigin.getSqrSize() - 
			pow(heardList.heardInfo[i+1].distance, 2) - pow(heardList.heardInfo[0].distance, 2));
	}

	
	gsl_vector *vector_tau = gsl_vector_alloc(min(heardSize-1, 3));
	gsl_vector *vector_residual = gsl_vector_alloc(heardSize - 1);
	gsl_linalg_QR_decomp(matrix_a, vector_tau);
	gsl_linalg_QR_lssolve(matrix_a, vector_tau, vector_b, vector_x, vector_residual);
	gsl_vector_free(vector_tau);
	gsl_vector_free(vector_residual);

	Vector ret = Vector(gsl_vector_get(vector_x, 0), gsl_vector_get(vector_x, 1), gsl_vector_get(vector_x, 2));

	gsl_vector_free(vector_b);
	gsl_vector_free(vector_x);
	gsl_matrix_free(matrix_a);

	*err = GetError(heardList, ret);
	return ret;

}

Vector PositionSolver::LeastSquareSolver2D(HeardList &heardList, double *err)
{	
	gsl_matrix *matrix_a;
	gsl_vector *vector_x, *vector_b;
	size_t heardSize = heardList.heardInfo.size();

	matrix_a = gsl_matrix_alloc(heardSize - 1, 2);
	vector_x = gsl_vector_alloc(2);
	vector_b = gsl_vector_alloc(heardSize - 1);

	/* set matrix A, Vector B */
	Vector vOrigin = heardList.heardInfo[0].location;
	for (size_t i = 0; i < heardSize - 1; i++)
	{
		Vector vLocation = heardList.heardInfo[i+1].location;
		Vector vDiff = vLocation - vOrigin;
		gsl_matrix_set(matrix_a, i, 0, 2 * vDiff.x);
		gsl_matrix_set(matrix_a, i, 1, 2 * vDiff.y);
		
		gsl_vector_set(vector_b, i, 
			(pow(vLocation.x, 2) - pow(vOrigin.x, 2)) + 
			(pow(vLocation.y, 2) - pow(vOrigin.y, 2)) -
			(pow(heardList.heardInfo[i+1].distance, 2) - 
				pow(heardList.heardInfo[0].distance, 2)));
	}

	
	gsl_vector *vector_tau = gsl_vector_alloc(min(heardSize-1, 2));
	gsl_vector *vector_residual = gsl_vector_alloc(heardSize - 1);
	gsl_linalg_QR_decomp(matrix_a, vector_tau);
	gsl_linalg_QR_lssolve(matrix_a, vector_tau, vector_b, vector_x,vector_residual);
	gsl_vector_free(vector_tau);
	gsl_vector_free(vector_residual);

	double x = gsl_vector_get(vector_x, 0);
	double y = gsl_vector_get(vector_x, 1);
	double z = 0;

	for (size_t i = 0; i < heardList.heardInfo.size(); i++)
	{
		double xsqr = pow(heardList.heardInfo[i].location.x - x, 2);
		double ysqr = pow(heardList.heardInfo[i].location.y - y, 2);
		double rsqr = pow(heardList.heardInfo[i].distance, 2);
		double sqrsum = fabs(rsqr - xsqr - ysqr);

		if (sqrsum < 0)
		{
			printf("n = %d\n", (int)heardSize);
			printf("actual z = %f\n", heardList.heardInfo[i].location.z);
			printf("x part sqr = %f\n", xsqr);
			printf("y part sqr = %f\n", ysqr);
			printf("r part sqr = %f\n", rsqr);
			printf("result = %f\n", rsqr - xsqr - ysqr);
			continue;
		}

		z = heardList.heardInfo[i].location.z - sqrt(sqrsum);
		break;
	}

	Vector ret = Vector(x, y, z);

	gsl_vector_free(vector_b);
	gsl_vector_free(vector_x);
	gsl_matrix_free(matrix_a);

	*err = GetError(heardList, ret);

	return ret;
	
}


SolvedPosition::SolvedPosition()
{
	history_size = 0;
	filter = 0;
}

void SolvedPosition::AddHistory(int bid, int pid)
{
	b_hist[history_size] = (char)bid;
	p_hist[history_size] = (char)pid;
	history_size ++;
}


SolvedPosition SolvedPosition::Duplicate()
{
	SolvedPosition dup;
	dup.history_size = history_size;
	memcpy(dup.b_hist, b_hist, sizeof(char) * history_size);
	memcpy(dup.p_hist, p_hist, sizeof(char) * history_size);
	dup.position = position;
	dup.error = error;
	dup.filter = filter;
	return dup;
}

void SolvedPosition::SetPosition(Vector &position, double error)
{
	this->position = position;
	this->error = error;
}
