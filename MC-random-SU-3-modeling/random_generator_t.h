#pragma once

#include <random>
#include "pcg-cpp/pcg_extras.hpp"
#include "pcg-cpp/pcg_random.hpp"

using namespace std;

class random_generator_t
{
public:
	typedef int16_t output_T;

private:
	const output_T lower_limit, upper_limit;
	uniform_int_distribution<output_T> distribution;
	pcg64_oneseq_once_insecure pcg64;

public:
	//for arbitrary unifrom distribution
	random_generator_t(const output_T &_lower_limit, const output_T &_upper_limit);

	//get a random number
	output_T operator()();
};

/*
//Some quite interesting variants discovered:

//DISTRIBUTIONS:
//generates numbers from (-limit;limit);  Anwardo edition
class dist_2018_symmetric_limits
{
public:
	typedef int32_t result_type;
	dist_2018_symmetric_limits(const result_type _limit) :
		limit(_limit),
		s(2 * _limit - 1),
		t( output_max % s ) //in this line (2^64 - s) is calculated as (~(s - 1))
	{}

	result_type operator()(const function<uint64_t()> &gen) const
	{
		uint64_t x, m;
		uint32_t l = 0;
		while (l < t)
		{
			//x = gen() & gen_output_mask; //get first 32 bits
			x = gen() % output_max; //get first 32 bits
			m = x * s;
			l = m % output_max; //get a remainder of division by 2^32
		}
		int32_t almost_output = m / output_max; //get a quotient from division by 2^32
		return almost_output - limit;
	}

private:
	// upper bound
	const result_type s;
	const uint32_t t;
	const int64_t limit;
	// assuming 64-bit generator
	//mask to extract first 32 bits from generator output
	static constexpr uint64_t output_max = numeric_limits<uint32_t>::max() + 1;
	//number of bits to shift for division by 2^32
	static constexpr uint8_t output_number_of_bits = 8 * sizeof(result_type);
};

//generates in arbitrary limits (Endill's edition)
class dist_2018_arbitrary_limits
{
public:
	typedef int16_t result_type;
	dist_2018_arbitrary_limits(const result_type _s_l, const result_type _s) :
		s_l(_s_l),
		s(_s),
		t((gen_max_power_2 - s) % s) {
		if (s_l >= s) {
			throw("Lower bound is not less that upper bound");
		}

		if (s_l < 0 && s > 0) {
			s += -s_l;
		} else {
			s -= -s_l;
		}
	}

	result_type operator()(const function<uint64_t()> &gen) const {
		uint64_t x, m;
		int64_t  l = m % gen_max_power_2;

		while (l < t) {
			x = gen() % gen_max_power_2;
			m = x * s;
			l = m % gen_max_power_2;
		}

		return m / gen_max_power_2 - s_l;
	}

private:
	// lower bound
	const result_type s_l;
	// upper bound
	int64_t s;
	// assuming 64-bit generator
	static constexpr uint64_t gen_max = numeric_limits<uint64_t>::max() >> (sizeof(limit) * 8);
	// overflow if limit's type width is 64 bits
	static constexpr uint64_t gen_max_power_2 = gen_max + 1;
	const int64_t t;
};

//GENERATORS; requires mt64.h
class genrand
{
public:
	typedef int64_t result_type;
	genrand() {
		random_device rd;
		init_genrand64(rd());
	}

	static constexpr result_type min() {
		return static_cast<result_type>(0);
	}

	static constexpr result_type max() {
		return static_cast<result_type>(18446744073709551615);
	}

	result_type operator()() {
		return static_cast<result_type>(genrand64_int64());
	}
};

//FULL BINDS (GENERATOR+DISTRIBUTION)
//requires mt64.h; APPROXIMATE uniformness (has some small bias due to floating point arithmetics
//correct realization
int16_t generate() {
	double r = genrand64_real2();//[0,1) range
	int16_t r_i = static_cast<int16_t>((r * (2 * limit + 1)) + 0.5);
	r_i -= limit;
	return r_i;
}
*/

/*
//Sample for tests:
//headers
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <random>

#include "pcg-cpp/pcg_extras.hpp"
#include "pcg-cpp/pcg_random.hpp"


//constant parameters
static const int16_t n = 8;
static const int16_t limit = 1743;

//random seed
random_device rd;
init_genrand64(rd());

//number of tests
static const int64_t test_count = 20;
static const int64_t repeat_count = 100000000;

//init distros
pcg_extras::seed_seq_from<random_device> seed_source;
uniform_int_distribution<int64_t> dist(-limit, limit);
dist_2018 dist_2k18(limit);

	cout << "reference (real2, approx. correct)" << endl;
	int16_t t2;
	long long time_total = 0;
	for (int j = 0; j < test_count; j++) {
		auto start = system_clock::now();
		for (int i = 0; i < repeat_count; i++)
			t2 = generate();
		auto end = system_clock::now();
		auto elapsed = duration_cast<milliseconds>(end - start).count();
		time_total += elapsed;
		//cout << "Time elapsed: " << elapsed << " ms" << endl;
	}
	cout << "Average time: " << double(time_total) / test_count << " ms" << endl;
	cout << endl;

	cout << "pcg64 + stl distribution" << endl;
	pcg64_oneseq_once_insecure pcg64(seed_source);
	int16_t t3;
	time_total = 0;
	for (int j = 0; j < test_count; j++) {
		auto start = system_clock::now();
		for (int i = 0; i < repeat_count; i++)
			t3 = dist(pcg64);
		auto end = system_clock::now();
		auto elapsed = duration_cast<milliseconds>(end - start).count();
		time_total += elapsed;
		//cout << "Time elapsed: " << elapsed << " ms" << endl;
	}
	cout << "Average time: " << double(time_total) / test_count << " ms" << endl;
	cout << endl;

	cout << "stl mt + stl distribution" << endl;
	mt19937_64 mt;
	int16_t t5;
	time_total = 0;
	for (int j = 0; j < test_count; j++) {
		auto start = system_clock::now();
		for (int i = 0; i < repeat_count; i++)
			t5 = dist(mt);
		auto end = system_clock::now();
		auto elapsed = duration_cast<milliseconds>(end - start).count();
		time_total += elapsed;
		//cout << "Time elapsed: " << elapsed << " ms" << endl;
	}
	cout << "Average time: " << double(time_total) / test_count << " ms" << endl;
	cout << endl;

	cout << "genrand + dist_2018" << endl;
	genrand g2;
	int16_t t7;
	time_total = 0;
	for (int j = 0; j < test_count; j++) {
		auto start = system_clock::now();
		for (int i = 0; i < repeat_count; i++)
			t7 = dist_2k18(g2);
		auto end = system_clock::now();
		auto elapsed = duration_cast<milliseconds>(end - start).count();
		time_total += elapsed;
		//cout << "Time elapsed: " << elapsed << " ms" << endl;
	}
	cout << "Average time: " << double(time_total) / test_count << " ms" << endl;
	cout << endl;

	//and so on; gnereally a pair (distro, generator) is tested
*/

