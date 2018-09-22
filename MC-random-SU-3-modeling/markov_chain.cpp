#include "pch.h"

#include <cmath>
#include <string>
#include "mt64.h"

#include "markov_chain.h"

markov_chain_t::markov_chain_t(const int16_t &_n) :
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

	int i = genrand64_int64() >> 60;
	sequences[1][i] = !sequences[1][i];

	init_eps_ms();
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

	init_eps_ms();
}

void markov_chain_t::init_eps_ms()
{
	size_t eps_ms_size = 5 * n * n * n * log(n);
	eps_ms_size += eps_ms_size % 4;
	eps_ms.reserve(eps_ms_size);

	for (auto e = eps_ms.begin(); e < eps_ms.end(); e += 4) {
		uint64_t r = genrand64_int64();

		*e       = static_cast<int16_t>( r >> 48)         ,
		*(e + 1) = static_cast<int16_t>((r >> 32) & 65535),
		*(e + 2) = static_cast<int16_t>((r >> 16) & 65535),
		*(e + 3) = static_cast<int16_t>( r        & 65535);
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

bool markov_chain_t::state_compare() const
{
	for (int i = 0; i < sequences[0].size(); i++) {
		if (sequences[0][i] != sequences[1][i])
			return false;
	}

	return true;
}

void markov_chain_t::markov_chain_step(const sequence_label_t &index, const int16_t &eps_m)
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
