#pragma once

//#define DEBUG
/*
order of logging:
- do_cftp outputs size of the array of random number before starting each cycle
- mc_evolve output each random number and then both sequences (high, then low)
- when the configuration is ready, do_cftp outputs 0 instead of size of the array and the final config
*/

#ifdef DEBUG
#include <bitset>
#endif

#include <vector>
#include <cmath>
#include <fstream>
#include "random_generator_t.h"

using namespace std;

enum sequence_label_t {high = 0, low = 1};

class markov_chain_t
{
private:
#ifdef DEBUG
	//var for debug logging
	ofstream log_file = ofstream("log_file", ios::out | ios::app | ios::binary);
#endif // DEBUG

	//stores the sequence length
	const int16_t n;
	//stores the 2 states
	vector<vector<uint8_t>> sequences = {
		vector<uint8_t>(2 * n),
		vector<uint8_t>(2 * n)
	};

	//internal random generator; m \in [0,2 n - 2] => eps_m \in [-(2n - 2) - 1, 2n - 2];
	random_generator_t generator;
	//the internal random sequence
	//storage for random vars
	vector<int16_t> eps_ms;
	//storage of recent number of steps
	int64_t last_evolution_length = 0;
	//an empirical / theoretical estimate for the number of steps
	size_t expected_number_of_steps() const;
	//function for internal random source (re)init
	void init_eps_ms();
	//random source update
	void update_eps_ms();
	//function, that conducts the MC evolution
	void evolve_mc();

public:
	//inits the the 2 states with high and low values
	markov_chain_t(const int16_t &_n);
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
	int16_t sequence_distance() const;
	//quick state comparison; returns true, if the states coincide
	bool sequences_are_equal() const;

	//perfoms a single MC step with random parameter eps_m
	//NOT A SAFE FUNCTION OF argument eps_m - eps_m must be in the range [0, 2n-2]
	void do_mc_step(const sequence_label_t &index, const int16_t &eps_m);
	//CFTP:
	// generates a sample and resets the configurations
	vector<uint8_t> do_cftp();

#ifdef DEBUG
	//function for prtinting number to binary test file
	void print_number_to_log(const int64_t &number);
#endif // DEBUG
};

