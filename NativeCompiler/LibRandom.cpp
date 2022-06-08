#include "NativeCompiler.h"
#include <random>

std::default_random_engine re;

int64_t call_RNDI(int64_t upper_bound)
{
	int lower_bound = 0;
	std::uniform_int_distribution<int> unif(lower_bound, upper_bound);
	auto r = unif(re);
	return r;
}

double call_RNDF(double upper_bound)
{
	double lower_bound = 0.0;
	std::uniform_real_distribution<double> unif(lower_bound, upper_bound);
	return unif(re);
}
