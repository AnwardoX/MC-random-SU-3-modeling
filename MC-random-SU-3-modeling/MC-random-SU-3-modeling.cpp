#include "pch.h"
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

static const int16_t n = 8;

using namespace std;
using namespace chrono;

random_device rd;
mt19937_64 gen(rd());
uniform_int_distribution<uint64_t> dist(0, numeric_limits<uint64_t>::max());
auto random = bind(dist, gen);

uniform_int_distribution<uint16_t> dist2(0, numeric_limits<uint16_t>::max());
auto random2 = bind(dist2, gen);

struct step_params_t {
	uint8_t eps;
	uint16_t m;
};

struct step_params16_t {
	uint8_t eps;
	uint8_t m;
};

void step_shuffle(array<uint8_t, n> &a, const uint8_t eps, const int16_t m) {
	/*if (m + 1 >= n) {
		throw "M is out of bounds of A";
	}*/

	// 16 bytes in reverse order
	__m128i shuffle_reg = _mm_set_epi8(6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, a[m + 1], a[m]);
	// 128 — zero
	const __m128i shuffle_control = _mm_set_epi8(128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, !eps ^ a[m + 1], !eps ^ a[m]);
	shuffle_reg = _mm_shuffle_epi8(shuffle_reg, shuffle_control);

	a[m]     = _mm_extract_epi8(shuffle_reg, 0);
	a[m + 1] = _mm_extract_epi8(shuffle_reg, 1);
}

void step(array<uint8_t, n> &a, const bool eps, const int16_t m) {
	/*if (m + 1 >= n) {
		throw "M is out of bounds of A";
	}*/

	if (eps == 0 && a[m] == 0 && a[m + 1] == 1) {
		a[m] = 1;
		a[m + 1] = 0;
	}

	if (eps == 1 && a[m] == 1 && a[m + 1] == 0) {
		a[m] = 0;
		a[m + 1] = 1;
	}
}

array<step_params_t, 4> generate_step_params() {
	array<step_params_t, 4> output;
	auto r = random();
	for (int i = 0; i < 4; i++) {
		uint16_t m = static_cast<uint16_t>((r >> (64 - (3 - i) * 16)) & 0xFFFF);
		output[i].eps = static_cast<uint8_t>(m >> 15);
		output[i].m = m & 0b0111111111111111;
	}
	return output;
}

tuple<uint8_t, uint16_t> generate_step_params_ref() {
	return make_tuple(!!random2(), random2());
}

int main()
{
	random_device rd;
	bool eps = rd();
	array<uint8_t, n> a = { !rd(), !rd(), !rd(), !rd(), !rd(), !rd(), !rd() };

	/*cout << "if" << endl;
	for (int j = 0; j < 10; j++) {
		auto start = system_clock::now();
		for (int i = 0; i < 1000000000; i++)
			step(a, eps, eps);
		auto end = system_clock::now();
		cout << "Time elapsed: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;
	}*/

	array<step_params_t, 4> b;
	tuple<uint8_t, uint16_t> c;

	cout << "generate_step_params()" << endl;
	for (int j = 0; j < 10; j++) {
		auto start = system_clock::now();
		for (int i = 0; i < 25000000; i++)
			b = generate_step_params();
		auto end = system_clock::now();
		cout << "Time elapsed: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;
	}

	cout << "generate_step_params_ref()" << endl;
	for (int j = 0; j < 10; j++) {
		auto start = system_clock::now();
		for (int i = 0; i < 100000000; i++)
			c = generate_step_params_ref();
		auto end = system_clock::now();
		cout << "Time elapsed: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;
	}

	system("pause");
	return 0;
}
