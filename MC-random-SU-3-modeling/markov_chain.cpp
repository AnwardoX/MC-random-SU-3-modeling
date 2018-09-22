#include "pch.h"

#include "markov_chain.h"

markov_chain_t::markov_chain_t(int16_t &_n) :
	n(_n) {}

markov_chain_t::markov_chain_t(vector<uint8_t> &_init_state_high, vector<uint8_t> &_init_state_low) :
	n(_init_state_high.size())
{	
	sequences[0] = move(_init_state_high);

	if (_init_state_low.size() == _init_state_high.size()) {
		sequences[1] = move(_init_state_low);
	} else {}
}

int16_t markov_chain_t::get_n() const
{
	return n;
}

vector<uint8_t> markov_chain_t::get_sequence(const sequence_label_t &index) const
{
	return sequences.at(index);
}

bool markov_chain_t::state_compare() const
{
	for (int16_t j = 0; j < 2 * n; j++) {
		if (sequences[0][j] != sequences[1][j]) {
			return false;
		}
	}
	return true;
}

void markov_chain_t::markov_chain_step(const int16_t &eps_m, const sequence_label_t &index)
{
	int16_t m = abs(eps_m); 

	if (   eps_m >= 0
		&& sequences[index][m    ] == 0
		&& sequences[index][m + 1] == 1)
	{
		sequences[index][m    ] = 1;
		sequences[index][m + 1] = 0;
	}

	if (   eps_m < 0
		&& sequences[index][m    ] == 1
		&& sequences[index][m + 1] == 0)
	{
		sequences[index][m    ] = 0;
		sequences[index][m + 1] = 1;
	}
}
