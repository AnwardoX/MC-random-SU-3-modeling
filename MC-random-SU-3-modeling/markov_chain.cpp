#include "pch.h"
#include "markov_chain.h"
#include <string>

size_t markov_chain_t::expected_number_of_steps() const
{
	return static_cast<size_t>(5 * n * n * n * log(n + 1));
}

void markov_chain_t::init_eps_ms()
{
	size_t eps_ms_size = expected_number_of_steps();
	eps_ms_size -= eps_ms_size % 4; //to make the length a whole number of 4
	eps_ms.resize(eps_ms_size);

	for (auto e = eps_ms.begin(); e < eps_ms.end(); ++e)
		*e = generator();
}

void markov_chain_t::evolve_mc()
{
	for (auto i = eps_ms.crbegin(); i < eps_ms.crend(); i++)
	{
		do_mc_step(high, *i);
		do_mc_step(low, *i);
	}
}

void markov_chain_t::update_eps_ms()
{
	//double the size
	size_t eps_ms_old_size = eps_ms.size();
	eps_ms.resize(2 * eps_ms_old_size);

	for(auto e = eps_ms.begin() + eps_ms_old_size; e < eps_ms.end(); ++e)
		*e = generator();
}

vector<uint8_t> markov_chain_t::do_cftp()
{
	//step 1: run CFTP algo
	//a flag to indicate the first entrance to the cycle
	bool is_first_cycle = true;
	while (!sequences_are_equal())
	{
		if (is_first_cycle)
		{
			is_first_cycle = false;
			//in the first cycle nothing needs to be done
		}
		else
		{
			//in further cycles a sequence reset and random source update is needed
			reset_sequence();
			update_eps_ms();
		}

		evolve_mc();
	}

	//step 2: store the results
	auto output_sequence = get_sequence(high);
	last_evolution_length = eps_ms.size();

	//step 2: restore the init state
	reset_sequence();
	init_eps_ms();
	return output_sequence;
}

markov_chain_t::markov_chain_t(const int16_t &_n) :
	n(_n),
	generator(1 - 2 * _n, 2 * _n - 2)
{
	if (_n < 1) 
	{
		throw("ERROR: n must be positive integer.");
	}

	//a quick way to reset the states to their default values
	reset_sequence();

	//generate the required number of random elements
	init_eps_ms();
}

markov_chain_t::markov_chain_t(const vector<uint8_t> &_init_seq_high, const vector<uint8_t> &_init_seq_low) :
	n(_init_seq_high.size() / 2),
	generator(1 - _init_seq_high.size(), _init_seq_high.size() - 2)
{	
	if (_init_seq_high.size() % 2 != 0)
	{
		throw("ERROR: High init sequence length is an odd number.");
	}

	if (_init_seq_high.size() != _init_seq_low.size())
	{
		throw("ERROR: Sequences has different length.");
	}

	//test on the input validity
	for (auto h = _init_seq_high.cbegin(), l = _init_seq_high.cbegin();
		      h < _init_seq_low .cend() && l < _init_seq_low .cend(); // paranoid. may be optimized if necessary
		      h++, l++)
	{
		//index string in case of error
		string index_string = to_string(h - _init_seq_high.cbegin());
		if (*h != 1 && *h != 0)
			throw("ERROR: The " + index_string + "th element of the high init sequence is neither 0 nor 1");
		if (*l != 1 && *l != 0)
			throw("ERROR: The " + index_string + "th element of the low init sequence is neither 0 nor 1");
	}

	//init the sequence, if everything is ok
	sequences[0] = move(_init_seq_high);
	sequences[1] = move(_init_seq_low);

	//generate the required number of random elements
	init_eps_ms();
}

void markov_chain_t::reset_sequence()
{
	for (auto h = sequences[0].begin(), l = sequences[1].begin();
		      h < sequences[0].end() && l < sequences[1].end(); // paranoid. may be optimized if necessary
	          h++, l++)
	{
		if (h < sequences[0].begin() + n)
		{
			*h = 1;
			*l = 0;
		}
		else
		{
			*h = 0;
			*l = 1;
		}
	}
}

int16_t markov_chain_t::get_n() const
{
	return n;
}

vector<uint8_t> markov_chain_t::get_sequence(const sequence_label_t &index) const
{
	return sequences[index];
}

int64_t markov_chain_t::get_last_evolution_length() const
{
	return last_evolution_length;
}

int16_t markov_chain_t::sequence_distance() const
{
	int16_t dist = 0;
	for (auto h = sequences[0].cbegin(), l = sequences[1].cbegin();
		      h < sequences[0].cend() && l < sequences[1].cend(); // paranoid. may be optimized if necessary
		h++, l++)
		dist += *h - *l;

	dist /= 2 * n;
	return dist;
}

bool markov_chain_t::sequences_are_equal() const
{
	for (auto h = sequences[0].cbegin(), l = sequences[1].cbegin();
		      h < sequences[0].cend() && l < sequences[1].cend(); // paranoid. may be optimized if necessary
		      h++, l++)
	{
		if (*h != *l)
			return false;
	}

	return true;
}

void markov_chain_t::do_mc_step(const sequence_label_t &index, const int16_t &eps_m)
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
		&& sequences[index][m - 1] == 1
		&& sequences[index][m    ] == 0)
	{
		sequences[index][m - 1] = 0;
		sequences[index][m    ] = 1;
	}
}

