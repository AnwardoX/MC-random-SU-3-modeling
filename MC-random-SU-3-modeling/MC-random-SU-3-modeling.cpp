#include "pch.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <random>
#include <tuple>

#include "markov_chain.h"

static const int16_t n = 8;
/*
static const int64_t test_count = 5000;
static const int64_t repeat_count = 1000000;
static const int16_t limit = 2048;
*/

using namespace std;
using namespace chrono;

/*
tuple<int16_t, int16_t, int16_t, int16_t> generate_ref() {
	uint64_t r = genrand64_int64();

	return make_tuple(
		static_cast<int16_t>(r >> 48),
		static_cast<int16_t>((r >> 32) & 65535),
		static_cast<int16_t>((r >> 16) & 65535),
		static_cast<int16_t>(r & 65535)
	);
}

int16_t generate_sub_before()
{
	double r = genrand64_real1();
	r -= 0.5;
	if (r >= 0)
		return static_cast<int16_t>((r * limit) + 0.5);
	else
		return static_cast<int16_t>((r * limit) - 0.5);
}

int16_t generate_sub_after()
{
	double r = genrand64_real1();
	int16_t r_i = static_cast<int16_t>((r * limit) + 0.5);
	r_i -= limit;
	return r_i;
}
*/


int main()
{
	random_device rd;
	init_genrand64(rd());

	int16_t n, tries;
	cin >> n >> tries;
	markov_chain_t chain(n);

	vector<uint8_t> output;
	for (int16_t j = 0; j < tries; j++)
	{
		output = chain.do_cftp();
		cout << "{";
		for (auto j = output.begin(); j < output.end(); j++)
			cout << *j << ",";
		cout << "}" << endl;
	}

	/*
	tuple<int16_t, int16_t, int16_t, int16_t> a;

	cout << "reference" << endl;
	long time_total = 0;
	for (int j = 0; j < test_count; j++) {
		auto start = system_clock::now();
		for (int i = 0; i < repeat_count / 4; i++)
			a = generate_ref();
		auto end = system_clock::now();
		auto elapsed = duration_cast<microseconds>(end - start).count();
		time_total += elapsed;
		//cout << "Time elapsed: " << elapsed << " us" << endl;
	}
	cout << "Average time: " << double(time_total) / test_count << " us" << endl;

	cout << "subtract before casting" << endl;
	time_total = 0;
	int16_t d;
	for (int j = 0; j < test_count; j++) {
		auto start = system_clock::now();
		for (int i = 0; i < repeat_count / 4; i++)
			d = generate_sub_before();
		auto end = system_clock::now();
		auto elapsed = duration_cast<microseconds>(end - start).count();
		time_total += elapsed;
		//cout << "Time elapsed: " << elapsed << " us" << endl;
	}
	cout << "Average time: " << double(time_total) / test_count << " us" << endl;

	cout << "subtract after casting" << endl;
	time_total = 0;
	int16_t f;
	for (int j = 0; j < test_count; j++) {
		auto start = system_clock::now();
		for (int i = 0; i < repeat_count / 4; i++)
			f = generate_sub_after();
		auto end = system_clock::now();
		auto elapsed = duration_cast<microseconds>(end - start).count();
		time_total += elapsed;
		//cout << "Time elapsed: " << elapsed << " us" << endl;
	}
	cout << "Average time: " << double(time_total) / test_count << " us" << endl;
	*/

	system("pause");
	return 0;
}
