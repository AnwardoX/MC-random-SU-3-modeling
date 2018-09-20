#include "pch.h"
#include "Markov_chain.h"

void Markov_chain::Markov_chain_step(const int16_t & eps_m)
{
	int16_t m = abs(eps_m);

	if (eps_m >= 0 && state[m] == 0 && state[m + 1] == 1)
	{
		state[m] = 1;
		state[m + 1] = 0;
	}
	
	if (eps_m < 0 && state[m] == 1 && state[m + 1] == 0)
	{
		state[m] = 0;
		state[m + 1] = 1;
	}
}
