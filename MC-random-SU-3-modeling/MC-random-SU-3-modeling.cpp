#include "pch.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <emmintrin.h>
#include <functional>
#include <iostream>
#include <limits>
#include <random>
#include <smmintrin.h>
#include <tmmintrin.h>
#include <tuple>

#include "mt64.h"
#include "Markov_chain.h"

static const int16_t n = 8;

using namespace std;
using namespace chrono;

random_device rd;
mt19937_64 gen(rd());
uniform_int_distribution<uint64_t> dist(0, numeric_limits<uint64_t>::max());
auto random = bind(dist, gen);

/*void step_shuffle(array<uint8_t, n> &a, const uint8_t eps, const int16_t m) {
	// 16 bytes in reverse order
	__m128i shuffle_reg = _mm_set_epi8(6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, a[m + 1], a[m]);
	// 128 — zero
	const __m128i shuffle_control = _mm_set_epi8(128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, !eps ^ a[m + 1], !eps ^ a[m]);
	shuffle_reg = _mm_shuffle_epi8(shuffle_reg, shuffle_control);

	a[m]     = _mm_extract_epi8(shuffle_reg, 0);
	a[m + 1] = _mm_extract_epi8(shuffle_reg, 1);
}*/

tuple<int16_t, int16_t, int16_t, int16_t> generate_eps_m() {
	uint64_t r = genrand64_int64();
	return make_tuple(static_cast<int16_t>(r >> 48),
		static_cast<int16_t>((r >> 32) & 65535),
		static_cast<int16_t>((r >> 16) & 65535),
		static_cast<int16_t>(r & 65535));
}

int main()
{
	init_genrand64(rd());
	bool eps = rd();
	uniform_int_distribution<int16_t> d_signed(-6, 6);
	auto random_signed = bind(d_signed, gen);

	array<int16_t, 100> eps_ms;
	generate(eps_ms.begin(), eps_ms.end(), random_signed);

	uniform_int_distribution<int16_t> dist16(numeric_limits<int16_t>::min(), numeric_limits<int16_t>::max());
	auto random16 = bind(dist16, gen);
	
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
