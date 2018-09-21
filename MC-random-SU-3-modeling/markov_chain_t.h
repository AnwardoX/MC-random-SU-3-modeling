#pragma once
#include <vector>

using namespace std;

enum state_label_t {high, low};

class markov_chain_t
{
private:
	//stores the sequence length
	const int16_t n;
	//stores the 2 states
	vector<uint8_t> state_array[2];
public:
	//inits the the 2 states with high and low values
	markov_chain_t(int16_t n);
	//inits with arbitrary state;
	//safe function: in case of either init_state vector is of invalid length, inits with default high/low states;
	//in case of non 0-1 values, inits with value % 2
	markov_chain_t(int16_t n, vector<uint8_t> init_state_high, vector<uint8_t> init_state_low);
	~markov_chain_t();

	//getters:
	int16_t get_n_f() const;
	vector<uint8_t> get_state_f(state_label_t index) const;

	//gives the L1 distance between the 2 states
	uint16_t state_distance_f() const;
	//quick state comparison; returns true, if the states coincide
	bool state_compare_f() const;

	//perfoms a single MC step with random parameter eps_m
	//NOT A SAFE FUNCTION OF argument eps_m - eps_m must be in the range [0, 2n-2]
	void markov_chain_step_f(const int16_t& eps_m, const state_label_t& index);
};

