#include "pch.h"

#include "random_generator_t.h"

random_generator_t::random_generator_t(const random_generator_t::output_T &_lower_limit, const random_generator_t::output_T &_upper_limit) :
	upper_limit(_upper_limit),
	lower_limit(_lower_limit),
	distribution(_lower_limit, _upper_limit)
{
	if (lower_limit >= upper_limit) {
		throw("Lower limit is not less than upper limit");
	}

	if (   numeric_limits<random_generator_t::output_T>::min() > lower_limit
		|| numeric_limits<random_generator_t::output_T>::max() < upper_limit) {
		throw("Output type is not sufficient for specified interval");
	}

	pcg_extras::seed_seq_from<random_device> seed_source;
	pcg64 = pcg64_oneseq_once_insecure(seed_source);
}

random_generator_t::output_T random_generator_t::operator()()
{
	return distribution(pcg64);
}

