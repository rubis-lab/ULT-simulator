#include <string.h>
#include <math.h>
#include "Solver.h"

#define min(a,b)            (((a) < (b)) ? (a) : (b))


//////////SolverInput///////////
///////////////////////////////

SolverInput::SolverInput(MeasurementList *measurementList):
		measurementList(measurementList) 
{
}

SolverInput::~SolverInput()
{
}

void SolverInput::setup(unsigned long currentTime, Vector previousLocation)
{
	measurementList->makeSnapshot(currentTime);
	measurements = measurementList->getMeasurements(); 
	initLocation = previousLocation;
}

void SolverInput::setInvalidButGather(int flag)
{
	if (flag >= MAX_FLAG_NUM) return;
	invalidButGather[flag] = true;
}

void SolverInput::resetGatherFlags()
{
	memset(invalidButGather, 0, sizeof(bool) * MAX_FLAG_NUM);
}

double SolverInput::getError(Vector location)
{
	double err = 0;
	for (size_t i = 0; i < measurements.size(); i++)
	{		
		err += pow(measurements[i]->getDistance() 
					- (measurements[i]->getLocation().getDistance(location)),2);
	}
	return err / measurements.size();
}


//////////SolverResult//////////
///////////////////////////////
//
SolverResult::SolverResult() : isEmpty(true)
{
}


SolverResult::SolverResult(Vector location, std::vector<Measurement*> snapshot) :
	location(location), 
	snapshot(snapshot), 
	overThreshold(false), 
	isEmpty(false),
	isInside(true)
{
	error = getError(location);
}

bool SolverResult::isValid()
{
	if (overThreshold || isEmpty || !isInside) return false;
	return true;
}
void SolverResult::setCorrectedLocation(Vector vCorrect)
{
	correctedLocation = vCorrect;
	distErrorCorrected = vCorrect.getDistance(location);
}
void SolverResult::setPredictedLocation(Vector vPredict)
{
	predictedLocation = vPredict;
	distErrorPredicted = vPredict.getDistance(location);
}
double SolverResult::getPredictedDistanceError()
{
	return distErrorPredicted;
}
double SolverResult::getError(Vector location)
{
	double err = 0;
	for (size_t i = 0; i <snapshot.size(); i++)
	{
		err += pow(snapshot[i]->getDistance() - 
				(snapshot[i]->getLocation().getDistance(location)), 2);
	}

	return err;
}
double SolverResult::getCorrectedError()
{
	return getError(correctedLocation);
}
double SolverResult::getPredictedError()
{
	return getError(predictedLocation);
}
double SolverResult::getError()
{
	return error;
}
Vector SolverResult::getCorrectedLocation()
{
	return correctedLocation;
}
Vector SolverResult::getPredictedLocation()
{
	return predictedLocation;
}

///////SolverResultList///////////////////////
///////////////////////////////////////////

SolverResultList::SolverResultList()
{
	fail = false;
}
SolverResultList::~SolverResultList()
{
}

void SolverResultList::addResult(SolverInput *input, Vector location)
{
	results.push_back(SolverResult(
			location,
			input->measurements));
}

void SolverResultList::setFail(SolverInput *input)
{
	results.push_back(SolverResult(input->initLocation, input->measurements));
	fail = true;
	
}


size_t SolverResultList::size()
{
	return results.size();
}

SolverResult* SolverResultList::at(int idx)
{
	return &results[idx];
}

bool SolverResultList::isValid(int idx)
{
	return results[idx].isValid();
}

Vector SolverResultList::getLocation(int idx)
{
	return results[idx].location;
}

SolverResult SolverResultList::getFilteredResult()
{
	return filteredResult;
}

SolverResult SolverResultList::getFirstResult()
{
	if (results.size() == 0)
	{
		printf("SolverResultList::getFirstResult(). results.size() == 0\n");
		exit(30);
	}
	return results[0];
}

void SolverResultList::setFilteredResult(SolverResult result)
{
	filteredResult = result;
}

bool SolverResultList::isFail()
{
	return fail;
}

////////Solver/////////
/////////////////////////////////

Solver::Solver()
{
}

Solver::~Solver()
{
}

void Solver::setSolverCondition(SolverCondition condition)
{
	this->condition = condition;
}

void Solver::solve(SolverInput *input, SolverResultList *results)
{
	if ((int)input->measurements.size() < condition.minBeaconSize)
	{
		results->setFail(input);
		return;
	}

	if (condition.solveNaive) 
	{
		solveNaive(input, results);
	}
	else
	{
		solveWithPlanes(input, results);
	}
}



void Solver::solveNaive(SolverInput *input, SolverResultList *results)
{
	Vector location = NLLeastSquareSolver(input);
	results->addResult(input, location);
}

void Solver::solveWithPlanes(SolverInput *input, SolverResultList *results, int currentIdx)
{
	if ((size_t)currentIdx == input->measurements.size())
	{
		//end condition
		Vector location = NLLeastSquareSolver(input);
		results->addResult(input, location);
#ifdef GATHER_DATA
// add more codes to treat gather data option.
#endif
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
#ifdef GATHER_DATA
//gather data is turned off to increase performance
				if (condition.gatherData) 
					input->setInvalidButGather(0);
				else 
					continue;
#else
				continue;
#endif
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

		solveWithPlanes(input, results, currentIdx + 1);
#ifdef GATHER_DATA
		input->resetGatherFlag();
#endif
	}
}

bool Solver::checkCutBranch1(SolverInput *input, int currentIdx)
{
	for (int i = 0; i < currentIdx; i++)
	{
		if (checkBranchCutoff(input->measurements[currentIdx], input->measurements[i]))
			return true;
	}
	return false;
}
		

bool Solver::checkBranchCutoff(Measurement* meas1, Measurement* meas2)
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
bool Solver::checkBranchCutoffCylinder(SolverInput *input, currentIdx)
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

int func_f(const gsl_vector *x, void *data, gsl_vector *f)
{
	SolverInput *input = (SolverInput*)data;
	double x_h = gsl_vector_get(x, 0);
	double y_h = gsl_vector_get(x, 1);
	double z_h = gsl_vector_get(x, 2);

	Vector location;
	double distance;
	for (size_t i = 0; i < input->measurements.size(); i++)
	{
		location = input->measurements[i]->getLocation();
		distance = input->measurements[i]->getDistance();
		gsl_vector_set(f, i, 
			pow(x_h - location.x, 2) + 
			pow(y_h - location.y, 2) +
			pow(z_h - location.z, 2) -
			pow(distance, 2));
	}
	return GSL_SUCCESS;
}

int func_df(const gsl_vector *x, void *data, gsl_matrix *J)
{
	SolverInput *input = (SolverInput*)data;
	double x_h = gsl_vector_get(x, 0);
	double y_h = gsl_vector_get(x, 1);
	double z_h = gsl_vector_get(x, 2);

	Vector location;
	for (size_t i = 0; i < input->measurements.size(); i++)
	{
		Vector location = input->measurements[i]->getLocation();

		gsl_matrix_set(J, i, 0, 2 * (x_h - location.x));
		gsl_matrix_set(J, i, 1, 2 * (y_h - location.y));
		gsl_matrix_set(J, i, 2, 2 * (z_h - location.z));
	}

	return GSL_SUCCESS;
}

int func_fdf(const gsl_vector *x, void *data, gsl_vector *f, gsl_matrix *J)
{
	func_f(x, data, f);
	func_df(x, data, J);
	return GSL_SUCCESS;
}

Vector Solver::NLLeastSquareSolver(SolverInput *input)
{
	const gsl_multifit_fdfsolver_type *T;
	gsl_multifit_fdfsolver *s;
	gsl_multifit_function_fdf f;
	
	double x_init[3] = {0.0, 0.0, 150.0};
//	x_init[0] = input->initLocation.x;
//	x_init[1] = input->initLocation.y;
//	x_init[2] = input->initLocation.z;
	
	gsl_vector_view x = gsl_vector_view_array(x_init, 3);

	int n = (int)input->measurements.size();

	// sort restriction..
//	n = 3;

	int p = 3;

	f.f = &func_f;
	f.df = &func_df;
	f.fdf = &func_fdf;
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


	return point;	
}




Vector Solver::LeastSquareSolver2D(SolverInput *input)
{	
	gsl_matrix *matrix_a;
	gsl_vector *vector_x, *vector_b;
	size_t dataSize = input->measurements.size();

	matrix_a = gsl_matrix_alloc(dataSize - 1, 2);
	vector_x = gsl_vector_alloc(2);
	vector_b = gsl_vector_alloc(dataSize - 1);

	/* set matrix A, Vector B */
	Vector vOrigin = input->measurements[0]->getLocation();
	double distanceOrigin = input->measurements[0]->getDistance();
	for (size_t i = 0; i < dataSize - 1; i++)
	{
		Vector vLocation = input->measurements[i+1]->getLocation();
		Vector vDiff = vLocation - vOrigin;
		gsl_matrix_set(matrix_a, i, 0, 2 * vDiff.x);
		gsl_matrix_set(matrix_a, i, 1, 2 * vDiff.y);
		
		gsl_vector_set(vector_b, i, 
			(pow(vLocation.x, 2) - pow(vOrigin.x, 2)) + 
			(pow(vLocation.y, 2) - pow(vOrigin.y, 2)) -
			(pow(input->measurements[i+1]->getDistance(), 2) - 
				pow(distanceOrigin, 2)));
	}

	
	gsl_vector *vector_tau = gsl_vector_alloc(min(dataSize-1, 2));
	gsl_vector *vector_residual = gsl_vector_alloc(dataSize - 1);
	gsl_linalg_QR_decomp(matrix_a, vector_tau);
	gsl_linalg_QR_lssolve(matrix_a, vector_tau, vector_b, vector_x,vector_residual);
	gsl_vector_free(vector_tau);
	gsl_vector_free(vector_residual);

	double x = gsl_vector_get(vector_x, 0);
	double y = gsl_vector_get(vector_x, 1);
	double z = 0;

	for (size_t i = 0; i < input->measurements.size(); i++)
	{
		Vector location = input->measurements[i]->getLocation();
		double distance = input->measurements[i]->getDistance();
		double xsqr = pow(location.x - x, 2);
		double ysqr = pow(location.y - y, 2);
		double rsqr = pow(distance, 2);
		double sqrsum = fabs(rsqr - xsqr - ysqr);

		if (sqrsum < 0)
		{
			printf("n = %d\n", (int)dataSize);
			printf("actual z = %f\n", location.z);
			printf("x part sqr = %f\n", xsqr);
			printf("y part sqr = %f\n", ysqr);
			printf("r part sqr = %f\n", rsqr);
			printf("result = %f\n", rsqr - xsqr - ysqr);
			continue;
		}

		z = location.z - sqrt(sqrsum);
		break;
	}

	Vector ret = Vector(x, y, z);

	gsl_vector_free(vector_b);
	gsl_vector_free(vector_x);
	gsl_matrix_free(matrix_a);

	return ret;
	
}

