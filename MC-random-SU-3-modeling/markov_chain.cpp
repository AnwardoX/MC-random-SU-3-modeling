#include "pch.h"

#include <string>
#include "markov_chain.h"

markov_chain_t::markov_chain_t(int16_t &_n) :
	n(_n)
{
	if (_n < 2) {
		throw("N < 2, which doesn't make sense.");
	}

	for (auto h = sequences[0].begin(), l = sequences[1].begin();
		      h < sequences[0].end() && l < sequences[1].end(); // paranoid. may be optimized if necessary
		      h++, l++) {
		if (h < sequences[0].begin() + n)
			*h = 1;
		else
			*h = 0;

		if (l < sequences[1].begin() + n)
			*l = 1;
		else
			*l = 0;
	}
}

markov_chain_t::markov_chain_t(const vector<uint8_t> &_init_seq_high, const vector<uint8_t> &_init_seq_low) :
	n(_init_seq_high.size() / 2)
{	
	if (_init_seq_high.size() % 2 != 0) {
		throw("High init sequence length is an odd number.");
	}

	if (_init_seq_high.size() != _init_seq_low.size()) {
		throw("Sequences has different length.");
	}

	for (auto h = _init_seq_high.cbegin(), l = _init_seq_high.cbegin();
		      h < _init_seq_low .cend() && l < _init_seq_low .cend(); // paranoid. may be optimized if necessary
		      h++, l++) {
		if (h <  _init_seq_high.cbegin() + n && *h != 1)
			throw( "First half of high init sequence doesn't consists of 1's only. See " + to_string(h - _init_seq_high.cbegin()) + " element (base 0).");
		if (h >= _init_seq_high.cbegin() + n && *h != 0)
			throw("Second half of high init sequence doesn't consists of 0's only. See " + to_string(h - _init_seq_high.cbegin()) + " element (base 0).");

		if (l <  _init_seq_low.cbegin() + n && *l != 0)
			throw( "First half of low init sequence doesn't consists of 0's only. See " + to_string(l - _init_seq_low.cbegin()) + " element (base 0).");
		if (l >= _init_seq_low.cbegin() + n && *l != 1) 
			throw("Second half of low init sequence doesn't consists of 1's only. See " + to_string(l - _init_seq_low.cbegin()) + " element (base 0).");
	}

	sequences[0] = move(_init_seq_high);
	sequences[1] = move(_init_seq_low);
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
	for (auto h = sequences[0].cbegin(), l = sequences[1].cbegin();
		      h < sequences[0].cend() && l < sequences[1].cend(); // paranoid. may be optimized if necessary
		      h++, l++) {
		if (*h != *l) {
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
