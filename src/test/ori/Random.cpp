#include "Random.h"
#include <time.h>

Random::Random(unsigned long seed)
{
	initialization(seed);
}

Random::Random()
{
	int seed = (int) time(NULL);
	printf("seed %d\n", seed);
	initialization(seed);
}



Random::~Random(void)
{
	gsl_rng_free(r);
}




double Random::GetGaussDist(double mean, double dev)
{
	return gsl_ran_gaussian(r, dev) + mean;
}

// return floating value between [0,1)
// excludes 1
double Random::GetUniformDist()
{
	return gsl_rng_uniform(r);
}


void Random::initialization(unsigned long seed)
{
	gsl_rng_env_setup();
	T = gsl_rng_default;
	r = gsl_rng_alloc(T);	
	gsl_rng_set(r, seed);	
}
