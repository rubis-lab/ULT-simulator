#include <string.h>
#include <math.h>
#include <omp.h>
#include "MultiSolver.h"


#define min(a,b)            (((a) < (b)) ? (a) : (b))


MultiSolverInput::MultiSolverInput(SolverInput *input)
{
	for (size_t i = 0; i < input->measurements.size(); i++)
	{
		data.push_back(MeasurementInstance(input->measurements[i]));
	}
}
MultiSolverInput::~MultiSolverInput()
{
}

size_t MultiSolverInput::size()
{
	return data.size();
}


Vector MultiSolverInput::getLocation(int idx)
{
	return data[idx].getLocation();
}

double MultiSolverInput::getDistance(int idx)
{
	return data[idx].getDistance();
}

///////////////////////////////////////////////////////////




MultiSolver::MultiSolver()
{
}

MultiSolver::~MultiSolver()
{
}

void MultiSolver::setSolverCondition(SolverCondition condition)
{
	this->condition = condition;
}

void MultiSolver::solve(SolverInput *input, SolverResultList *results)
{
	if ((int)input->measurements.size() < condition.minBeaconSize)
	{
		results->setFail(input);
		return;
	}

	if (condition.solveNaive) 
	{
		SolverResult result;
		MultiSolverInput msInput(input);
		result = solveNaive(msInput);
		results->addResult(result);
	}
	else
	{
		inputList.clear();
		solveWithPlanes(input);
		solveInputList(results);
	}
}



SolverResult MultiSolver::solveNaive(MultiSolverInput &input)
{
	Vector location = NLLeastSquareSolver(&input);
	return SolverResult(location, input.data);
}

void MultiSolver::solveInputList(SolverResultList *results)
{
	SolverResult result;
	#pragma omp parallel for private(result) num_threads(8)
	for (size_t i = 0; i < inputList.size(); i++)
	{
		result = solveNaive(inputList[i]);
		#pragma omp critical
		{
			results->addResult(result);
		}
	}
}

void MultiSolver::solveWithPlanes(SolverInput *input, int currentIdx)
{
	if ((size_t)currentIdx == input->measurements.size())
	{
		inputList.push_back(MultiSolverInput(input));
		//end condition
		return;
	}
	Measurement *measurement = input->measurements[currentIdx];
	measurement->resetIterator();
	while(measurement->nextVBeacon())
	{	
		// check measured distance is long enough to reflect corresponding wall
		if (!measurement->isValidDistance())
			continue;

		// check optimization
		if (condition.cutBranch1)
		{
			if (checkCutBranch1(input, currentIdx))
			{
				continue;
			}
		}

		// optimization 2 is not added
#if 0
		if (condition.cutBranch2)
		{
			if (checkBranchCutoffCylinder(input, currentIdx))
			{
				if (condition.gatherData)
					input->setInvalidButGather(1);

				else continue;
			}
		}
#endif

		solveWithPlanes(input, currentIdx + 1);
	}
}

bool MultiSolver::checkCutBranch1(SolverInput *input, int currentIdx)
{
	for (int i = 0; i < currentIdx; i++)
	{
		if (!checkBranchCutoff(input->measurements[currentIdx], input->measurements[i]))
			return true;
	}
	return false;
}
		

bool MultiSolver::checkBranchCutoff(Measurement* meas1, Measurement* meas2)
{
	double a = meas1->getLocation().getDistance(meas2->getLocation());
	double b = meas1->getDistance();
	double c = meas2->getDistance();

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

	return d <= e + f + condition.maxMeasError * 2;
}

#if 0
bool MultiSolver::checkBranchCutoffCylinder(SolverInput *input, currentIdx)
{
	if (currentIdx < 3) return false;
	std::vector <Cylinder> cylinderList;
	cylinderList.empty();

	for (int j = 1; j < currentIdx; j++)
	{
		Cylinder cl = Cylinder(heardListReflection.heardInfo[j - 1], 
				heardListReflection.heardInfo[j], 
				condition.maxMeasError);
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
#endif




/***********************************
//
//  Non Linear Least Square Solver
//
// ***********************************/

int ms_f(const gsl_vector *x, void *data, gsl_vector *f)
{
	MultiSolverInput *input = (MultiSolverInput*)data;
	double x_h = gsl_vector_get(x, 0);
	double y_h = gsl_vector_get(x, 1);
	double z_h = gsl_vector_get(x, 2);

	Vector location;
	double distance;
	for (size_t i = 0; i < input->data.size(); i++)
	{
		location = input->getLocation(i);
		distance = input->getDistance(i);
		gsl_vector_set(f, i, 
			pow(x_h - location.x, 2) + 
			pow(y_h - location.y, 2) +
			pow(z_h - location.z, 2) -
			pow(distance, 2));
	}
	return GSL_SUCCESS;
}

int ms_df(const gsl_vector *x, void *data, gsl_matrix *J)
{
	MultiSolverInput *input = (MultiSolverInput*)data;
	double x_h = gsl_vector_get(x, 0);
	double y_h = gsl_vector_get(x, 1);
	double z_h = gsl_vector_get(x, 2);

	Vector location;
	for (size_t i = 0; i < input->data.size(); i++)
	{
		Vector location = input->getLocation(i);

		gsl_matrix_set(J, i, 0, 2 * (x_h - location.x));
		gsl_matrix_set(J, i, 1, 2 * (y_h - location.y));
		gsl_matrix_set(J, i, 2, 2 * (z_h - location.z));
	}

	return GSL_SUCCESS;
}

int ms_fdf(const gsl_vector *x, void *data, gsl_vector *f, gsl_matrix *J)
{
	ms_f(x, data, f);
	ms_df(x, data, J);
	return GSL_SUCCESS;
}

Vector MultiSolver::NLLeastSquareSolver(MultiSolverInput *input)
{
	/**/condition.analyzer->solverSolving.startTimer();	// ---- ---- T5 start
	const gsl_multifit_fdfsolver_type *T;
	gsl_multifit_fdfsolver *s;
	gsl_multifit_function_fdf f;
	
	double x_init[3] = {0.0, 0.0, 150.0};
//	x_init[0] = input->initLocation.x;
//	x_init[1] = input->initLocation.y;
//	x_init[2] = input->initLocation.z;
	
	gsl_vector_view x = gsl_vector_view_array(x_init, 3);

	int n = (int)input->data.size();

	// sort restriction..
//	n = 3;

	int p = 3;

	f.f = &ms_f;
	f.df = &ms_df;
	f.fdf = &ms_fdf;
	f.n = n;
	f.p = p;	
	f.params = input;

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

	gsl_vector_free(gradt);
	gsl_multifit_fdfsolver_free(s);


	/**/condition.analyzer->solverSolving.stopTimer();	// ---- ---- T5 end
	return point;	
}

