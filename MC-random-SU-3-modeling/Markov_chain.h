#pragma once

#include <vector>

using namespace std;

class Markov_chain
{
private:
	//stores the sequence length
	const int16_t n;
	//stores the state
	vector<uint8_t> state;
	//storage for initialization of high and low states
	static const vector<uint8_t> high_init;
	static const vector<uint8_t> low_init;
public:
	//inits the the state with either high (is_high = true) or low (false) state
	Markov_chain(int16_t n, bool is_high);
	//inits with arbitrary state
	Markov_chain(int16_t n, vector<uint8_t> init_state);
	~Markov_chain();

	//gives the L1 distance between the 2 states
	const uint16_t State_distance(const Markov_chain& st_1, const Markov_chain& st_2);
	//quick state comparison
	const bool State_compare(const Markov_chain& st_1, const Markov_chain& st_2);

	//perfoms a single MC step with random parameter eps_m
	void Markov_chain_step(const int16_t& eps_m);
};

