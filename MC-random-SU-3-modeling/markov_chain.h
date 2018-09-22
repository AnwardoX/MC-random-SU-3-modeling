#pragma once

#include <vector>

using namespace std;

enum sequence_label_t {high = 0, low = 1};

class markov_chain_t
{
private:
	//stores the sequence length
	const int16_t n;
	//stores the 2 states
	vector<vector<uint8_t>> sequences = {
		vector<uint8_t>(2 * n),
		vector<uint8_t>(2 * n)
	};

	//vector<int16_t> eps_ms;
	//void init_eps_ms();

public:
	//inits the the 2 states with high and low values
	markov_chain_t(int16_t &_n);
	//inits with arbitrary state;
	//safe function: in case of either init_state vector is of invalid length, inits with default high/low states;
	//in case of non 0-1 values, inits with value % 2
	markov_chain_t(const vector<uint8_t> &_init_state_high, const vector<uint8_t> &_init_state_low);

	//getters:
	int16_t get_n() const;
	vector<uint8_t> get_sequence(const sequence_label_t &index) const;

	//L1 distance between the 2 states
	uint16_t sequence_distance() const;
	//quick state comparison; returns true, if the states coincide
	bool sequence_compare() const;

	//perfoms a single MC step with random parameter eps_m
	//NOT A SAFE FUNCTION OF argument eps_m - eps_m must be in the range [0, 2n-2]
	void markov_chain_step(const sequence_label_t &index, const int16_t &eps_m);
};

