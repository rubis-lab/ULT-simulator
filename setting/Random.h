#pragma once

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

static class Random
{
public:
	Random(void);
	~Random(void);


	const gsl_rng_type * T;
	gsl_rng *r;

	void setSeed(unsigned long seed);
	double getGaussDist(double mean, double dev);
	double getUniformDist();
	double getUniformDist(double min, double max);
	void printSeed();

private:
	void initialization(unsigned long seed);
	void destructor();

	unsigned long seed;

} Random;
