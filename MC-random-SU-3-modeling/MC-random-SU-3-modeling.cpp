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
static const int16_t limit = 2048;

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

class dist_2018
{
public:
	typedef int16_t result_type;
	dist_2018(const result_type _s) :
		s(_s),
		t((gen_max_power_2 - s) % s) {}

	result_type operator()(const function<uint64_t()> &gen) const {
		uint64_t x  = gen();
		         x &= gen_max;
		int64_t m = x * s;
		int64_t l = m % gen_max_power_2;
		if (l < s) {
			while (l < t) {
				x = gen();
				m = x * s;
				l = m % gen_max_power_2;
			}
		}
		return m / gen_max_power_2;
	}

private:
	// upper bound
	result_type s;
	const int64_t t;
	// assuming 64-bit generator
	static constexpr uint64_t gen_max = numeric_limits<uint64_t>::max() >> (sizeof(limit) * 8);
	// overflow if limit's type width is 64 bits
	static constexpr uint64_t gen_max_power_2 = gen_max + 1; 
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
	static const int64_t repeat_count = 100000000;

	pcg_extras::seed_seq_from<random_device> seed_source;
	uniform_int_distribution<int64_t> dist(-limit, limit);

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

	cout << "reference (truncate, correct)" << endl;
	int16_t t2;
	time_total = 0;
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

	system("pause");
	return 0;
}
