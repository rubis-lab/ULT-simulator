#include "Random.h"
#include <time.h>


Random::Random()
{
	seed = (unsigned long) time(NULL);
	initialization(seed);
}


void Random::setSeed(unsigned long seed)
{
	destructor();
	initialization(seed);
}

Random::~Random(void)
{
	destructor();
}


void Random::destructor()
{
	gsl_rng_free(r);
}


double Random::getGaussDist(double mean, double dev)
{
	return gsl_ran_gaussian(r, dev) + mean;
}

// return floating value between [0,1)
// excludes 1
double Random::getUniformDist()
{
	return gsl_rng_uniform(r);
}

double Random::getUniformDist(double min, double max)
{
	double ratio = max - min;
	return (getUniformDist() * ratio) + min;
}


void Random::initialization(unsigned long seed)
{
	gsl_rng_env_setup();
	T = gsl_rng_default;
	r = gsl_rng_alloc(T);	
	gsl_rng_set(r, seed);	
}

void Random::printSeed()
{
	printf("seed = %lu\n", seed);
}


