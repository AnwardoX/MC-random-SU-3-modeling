#include "pch.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <random>
#include <tuple>

#include "pcg-cpp/pcg_extras.hpp"
#include "pcg-cpp/pcg_random.hpp"

#include "markov_chain.h"

static const int16_t n = 8;
// should be less than 64-bit type due to dist_2018
static const int16_t limit = 1743;

using namespace std;
using namespace chrono;

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

//generates numbers from (-limit;limit)
class dist_2018
{
public:
	typedef int32_t result_type;
	dist_2018(const result_type _limit) :
		limit(_limit),
		s(2 * _limit - 1),
		t( (static_cast<uint64_t>(~(s - 1))) % s ) //in this line (2^64 - s) is calculated as (~(s - 1))
	{}

	result_type operator()(const function<uint64_t()> &gen) const
	{
		uint64_t x, m;
		uint32_t l = 0;
		while (l < t) 
		{
			x = gen() & gen_output_mask; //get first 32 bits
			m = x * s;
			l = m >> gen_output_number_of_bits; //get a remainder of division by 2^32
		}
		int32_t almost_output = m >> output_number_of_bits; //get a quotient from division by 2^32
		return almost_output - limit;
	}

private:
	// upper bound
	result_type s;
	const uint32_t t;
	const int64_t limit;
	// assuming 64-bit generator
	//mask to extract first 32 bits from generator output
	static constexpr uint64_t gen_output_mask = numeric_limits<uint32_t>::max();
	//number of bits to shift for division by 2^32
	static constexpr uint8_t gen_output_number_of_bits = 8 * sizeof(uint64_t);
	//number of bits in output for division  for division by 2^L
	static constexpr uint8_t output_number_of_bits = 8 * sizeof(result_type);
	//static constexpr uint64_t gen_max = numeric_limits<uint64_t>::max() >> (sizeof(limit) * 8);
	// overflow if limit's type width is 64 bits
	//static constexpr uint64_t gen_max_power_2 = gen_max + 1; 
};

tuple<int16_t, int16_t, int16_t, int16_t> generate_ref() {
	uint64_t r = genrand64_int64();

	return make_tuple(
		static_cast<int16_t>(r >> 48),
		static_cast<int16_t>((r >> 32) & 65535),
		static_cast<int16_t>((r >> 16) & 65535),
		static_cast<int16_t>(r & 65535)
	);
}

//correct realization
int16_t generate() {
	double r = genrand64_real2();//[0,1) range
	int16_t r_i = static_cast<int16_t>((r * (2 * limit + 1)) + 0.5);
	r_i -= limit;
	return r_i;
}

void genrand_int_limit(vector<int16_t> &v) {
	uint64_t r = genrand64_int64();

	auto a1 = static_cast<int16_t>(r >> 48);
	auto a2 = static_cast<int16_t>((r >> 32) & 65535);
	auto a3 = static_cast<int16_t>((r >> 16) & 65535);
	auto a4 = static_cast<int16_t>(r & 65535);

	if (a1 > -limit && a1 < limit)
		v.push_back(a1);
	if (a2 > -limit && a2 < limit)
		v.push_back(a2);
	if (a3 > -limit && a3 < limit)
		v.push_back(a3);
	if (a4 > -limit && a4 < limit)
		v.push_back(a4);
}


void pcg64_limit(vector<int16_t> &v, pcg64_oneseq_once_insecure &pcg) {
	uint64_t r = pcg();

	auto a1 = static_cast<int16_t>(r >> 48);
	auto a2 = static_cast<int16_t>((r >> 32) & 65535);
	auto a3 = static_cast<int16_t>((r >> 16) & 65535);
	auto a4 = static_cast<int16_t>(r & 65535);

	if (a1 > -limit && a1 < limit)
		v.push_back(a1);
	if (a2 > -limit && a2 < limit)
		v.push_back(a2);
	if (a3 > -limit && a3 < limit)
		v.push_back(a3);
	if (a4 > -limit && a4 < limit)
		v.push_back(a4);
}

int main() {
	random_device rd;
	init_genrand64(rd());

	/*int16_t n, tries;
	cin >> n >> tries;
	markov_chain_t chain(n);

	vector<uint8_t> output;
	for (int16_t j = 0; j < tries; j++)
	{
		output = chain.do_cftp();
		cout << "{";
		for (auto j = output.begin(); j < output.end(); ++j)
			cout << *j << ",";
		cout << "}" << endl;
	}*/

	static const int64_t test_count = 20;
	static const int64_t repeat_count = 10;

	pcg_extras::seed_seq_from<random_device> seed_source;
	uniform_int_distribution<int64_t> dist(-limit, limit);
	dist_2018 dist_2k18(limit);

	/*
	//no need in this, as it is not wirkong with arbitrary limits
	cout << "reference (4 x 16 bit, incorrect)" << endl;
	tuple<int16_t, int16_t, int16_t, int16_t> t1;
	long long time_total = 0;
	for (int j = 0; j < test_count; j++) {
		auto start = system_clock::now();
		for (int i = 0; i < repeat_count / 4; i++)
			t1 = generate_ref();
		auto end = system_clock::now();
		auto elapsed = duration_cast<milliseconds>(end - start).count();
		time_total += elapsed;
		//cout << "Time elapsed: " << elapsed << " ms" << endl;
	}
	cout << "Average time: " << double(time_total) / test_count << " ms" << endl;
	cout << endl;
	*/

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

	/*
	//veeeery slow guy
	cout << "pcg64 + limit" << endl;
	pcg64_oneseq_once_insecure pcg64_2(seed_source);
	time_total = 0;
	for (int j = 0; j < test_count; j++) {
		vector<int16_t> v;
		v.reserve(repeat_count);

		auto start = system_clock::now();
		for (int i = 0; i < repeat_count / 4; i++)
			pcg64_limit(v, pcg64_2);
		while (v.size() < repeat_count)
			pcg64_limit(v, pcg64_2);
		auto end = system_clock::now();
		auto elapsed = duration_cast<milliseconds>(end - start).count();
		time_total += elapsed;
		//cout << "Time elapsed: " << elapsed << " ms" << endl;
	}
	cout << "Average time: " << double(time_total) / test_count << " ms" << endl;
	cout << endl;
	*/

	/*
	//very slow guy
	cout << "genrand int + limit" << endl;
	time_total = 0;
	for (int j = 0; j < test_count; j++) {
		vector<int16_t> v;
		v.reserve(repeat_count);

		auto start = system_clock::now();
		for (int i = 0; i < repeat_count / 4; i++)
			genrand_int_limit(v);
		while (v.size() < repeat_count)
			genrand_int_limit(v);
		auto end = system_clock::now();
		auto elapsed = duration_cast<milliseconds>(end - start).count();
		time_total += elapsed;
		//cout << "Time elapsed: " << elapsed << " ms" << endl;
	}
	cout << "Average time: " << double(time_total) / test_count << " ms" << endl;
	cout << endl;
	*/

	/*
	//not working for some reason
	cout << "genrand + stl distribution" << endl;
	genrand g;
	int16_t t4;
	time_total = 0;
	for (int j = 0; j < test_count; j++) {
		auto start = system_clock::now();
		for (int i = 0; i < repeat_count; i++)
			t4 = dist(g);
		auto end = system_clock::now();
		auto elapsed = duration_cast<milliseconds>(end - start).count();
		time_total += elapsed;
		//cout << "Time elapsed: " << elapsed << " ms" << endl;
	}
	cout << "Average time: " << double(time_total) / test_count << " ms" << endl;
	cout << endl;
	*/

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

	cout << "stl mt + dist_2018" << endl;
	mt19937_64 mt2;
	int16_t t6;
	time_total = 0;
	for (int j = 0; j < test_count; j++) {
		auto start = system_clock::now();
		for (int i = 0; i < repeat_count; i++)
			t6 = dist_2k18(mt2);
		auto end = system_clock::now();
		auto elapsed = duration_cast<milliseconds>(end - start).count();
		time_total += elapsed;
		//cout << "Time elapsed: " << elapsed << " ms" << endl;
	}
	cout << "Average time: " << double(time_total) / test_count << " ms" << endl;
	cout << endl;

	/*
	//not working either due to genrand
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
	*/

	cout << "pcg64 + dist_2018" << endl;
	pcg64_oneseq_once_insecure pcg64_2(seed_source);
	int16_t t8;
	time_total = 0;
	for (int j = 0; j < test_count; j++) {
		auto start = system_clock::now();
		for (int i = 0; i < repeat_count; i++)
			t8 = dist_2k18(pcg64_2);
		auto end = system_clock::now();
		auto elapsed = duration_cast<milliseconds>(end - start).count();
		time_total += elapsed;
		//cout << "Time elapsed: " << elapsed << " ms" << endl;
	}
	cout << "Average time: " << double(time_total) / test_count << " ms" << endl;
	cout << endl;

	system("pause");
	return 0;
}
