#include "pch.h"
#include "markov_chain_t.h"

bool markov_chain_t::state_compare_f() const
{
	for (uint16_t j = 0; j < 2 * n; j++)
		if (state_array[0][j] != state_array[1][j])
			return false;
	return true;
}

void markov_chain_t::markov_chain_step_f(const int16_t & eps_m, const state_label_t & index)
{
	uint16_t m = abs(eps_m); 

	if (eps_m >= 0 && state_array[index][m] == 0 && state_array[index][m + 1] == 1)
	{
		state_array[index][m] = 1;
		state_array[index][m + 1] = 0;
	}

	if (eps_m < 0 && state_array[index][m] == 1 && state_array[index][m + 1] == 0)
	{
		state_array[index][m] = 0;
		state_array[index][m + 1] = 1;
	}
}
