#include "pch.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <functional>
#include <iostream>
#include <limits>
#include <random>
#include <tuple>

#include "mt64.h"
#include "markov_chain.h"

static const int16_t n = 8;
static const int64_t test_count = 1000;
static const int64_t repeat_count = 1000000;

using namespace std;
using namespace chrono;

random_device rd;

int main()
{
	init_genrand64(rd());

	/*cout << "state compare using indexes" << endl;
	long time_total = 0;
	bool r;
	for (int j = 0; j < test_count; j++) {
		markov_chain_t chain(n);
		auto start = system_clock::now();
		for (int i = 0; i < repeat_count; i++)
			r = chain.state_compare();
		auto end = system_clock::now();
		auto elapsed = duration_cast<microseconds>(end - start).count();
		time_total += elapsed;
		//cout << "Time elapsed: " << elapsed << " us" << endl;
	}
	cout << r << endl;
	cout << "Average time: " << double(time_total) / test_count << " us" << endl;*/

	system("pause");
	return 0;
}
