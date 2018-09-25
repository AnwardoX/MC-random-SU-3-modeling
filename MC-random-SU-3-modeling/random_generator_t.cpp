#include "pch.h"
#include "random_generator_t.h"

template<typename output_T>
random_generator_t<output_T>::random_generator_t(const output_T _lower_limit, const output_T _upper_limit):
	upper_limit(_upper_limit),
	lower_limit(_lower_limit),
	distribution(_lower_limit, _upper_limit),
	pcg64(pcg_extras::seed_seq_from<random_device>())
{
}

template<typename output_T>
random_generator_t<output_T>::~random_generator_t()
{
}

template<typename output_T>
output_T random_generator_t<output_T>::operator()(void)
{
	return distribution(pcg64);
}
