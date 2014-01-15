#pragma once

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

class Random
{
public:
	Random(unsigned long seed);
	Random(void);
	~Random(void);

	const gsl_rng_type * T;
	gsl_rng *r;

	double GetGaussDist(double mean, double dev);
	double GetUniformDist();

private:
	void initialization(unsigned long seed);
	unsigned long seed;
};
