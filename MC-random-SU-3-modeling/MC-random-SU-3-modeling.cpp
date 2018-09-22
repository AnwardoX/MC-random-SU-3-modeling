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

using namespace std;
using namespace chrono;

random_device rd;
mt19937_64 gen(rd());
uniform_int_distribution<uint64_t> dist(0, numeric_limits<uint64_t>::max());
auto random = bind(dist, gen);

tuple<int16_t, int16_t, int16_t, int16_t> generate_eps_m() {
	uint64_t r = genrand64_int64();
	return make_tuple(
		static_cast<int16_t>( r >> 48)         ,
		static_cast<int16_t>((r >> 32) & 65535),
		static_cast<int16_t>((r >> 16) & 65535),
		static_cast<int16_t>( r        & 65535)
	);
}

int main()
{
	init_genrand64(rd());
	bool eps = rd();
	uniform_int_distribution<int16_t> d_signed(-6, 6);
	auto random_signed = bind(d_signed, gen);

	array<int16_t, 100> eps_ms;
	generate(eps_ms.begin(), eps_ms.end(), random_signed);
	
	array<uint8_t, n> a = { !rd(), !rd(), !rd(), !rd(), !rd(), !rd(), !rd() };

	tuple<int16_t, int16_t, int16_t, int16_t> b;

	/*cout << "step single argument" << endl;
	for (int j = 0; j < 10; j++) {
		auto start = system_clock::now();
		for (int i = 0; i < 100000000; i++)
			step(a, eps_ms[j]);
		auto end = system_clock::now();
		cout << "eps_m: " << eps_ms[j] << endl;
		cout << "Time elapsed: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;
	}*/

	/*cout << "64-bit random genrand" << endl;
	for (int j = 0; j < 10; j++) {
		auto start = system_clock::now();
		for (int i = 0; i < 25000000; i++)
			b = generate_eps_m();
		auto end = system_clock::now();
		cout << "Time elapsed: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;
	}*/

	system("pause");
	return 0;
}
