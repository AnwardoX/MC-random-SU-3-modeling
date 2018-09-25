#include "pch.h"
#include "random_generator_t.h"

random_generator_t::random_generator_t(int16_t _lower_limit, int16_t _upper_limit):
	upper_limit(_upper_limit),
	lower_limit(_lower_limit),
	distribution(_lower_limit, _upper_limit),
	pcg64(pcg_extras::seed_seq_from<random_device>())
{
}

random_generator_t::~random_generator_t()
{
}

int16_t random_generator_t::generate_random_number()
{
	return distribution(pcg64);
}
