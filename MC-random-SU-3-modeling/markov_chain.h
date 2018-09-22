#pragma once

#include <vector>
#include <cmath>
#include "mt64.h"

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

	//random source
	//storage for random vars
	vector<int16_t> eps_m_array;
	//an empirical / theoretical estimate for the number of steps
	int64_t exptected_number_of_steps();
	void init_eps_m_array();
	//function, that conducts the MC evolution
	void MC_evolve();
	//storage of recent number of steps
	int64_t last_evolution_length = 0;
public:
	//inits the the 2 states with high and low values
	markov_chain_t(int16_t &_n);
	//inits with arbitrary state;
	//safe function: in case of either init_state vector is of invalid length, inits with default high/low states;
	//in case of non 0-1 values, inits with value % 2
	markov_chain_t(const vector<uint8_t> &_init_state_high, const vector<uint8_t> &_init_state_low);
	//fucntion for resetting the sequences to their defaut state
	void reset_sequence();

	//getters:
	int16_t get_n() const;
	vector<uint8_t> get_sequence(const sequence_label_t &index) const;
	int64_t get_last_evolution_length() const;

	//L1 distance between the 2 states
	uint16_t sequence_distance() const;
	//quick state comparison; returns true, if the states coincide
	bool sequence_compare() const;

	//perfoms a single MC step with random parameter eps_m
	//NOT A SAFE FUNCTION OF argument eps_m - eps_m must be in the range [0, 2n-2]
	void markov_chain_step(const sequence_label_t &index, const int16_t &eps_m); 

	//random source update
	void update_eps_m_array();
	//random source reinit
	void reset_eps_m_array();

	//CFTP:
	// generates a sample and resets the configurations
	vector<uint8_t> CFTP();
};

