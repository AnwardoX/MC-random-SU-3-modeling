#include "pch.h"
#include <array>
#include <chrono>
#include <emmintrin.h>
#include <iostream>
#include <smmintrin.h>
#include <tmmintrin.h>

using namespace std;
using namespace chrono;

static const int64_t n = 8;

void step(array<uint8_t, n> &a, const uint8_t eps, const int64_t m) {
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

void step_if(array<uint8_t, n> &a, const uint8_t eps, const int64_t m) {
	/*if (m + 1 >= n) {
		throw "M is out of bounds of A";
	}*/

	if (eps == 0 && a[m] == 0 && a[m + 1] == 1) {
		a[m]     = 1;
		a[m + 1] = 0;
	}

	if (eps == 1 && a[m] == 1 && a[m + 1] == 0) {
		a[m]     = 0;
		a[m + 1] = 1;
	}
}

int main()
{
	array<uint8_t, n> a = { 0, 1, 0, 0, 0, 1, 0};

	cout << "shuffle" << endl;
	for (int j = 0; j < 10; j++) {
		auto start = system_clock::now();
		for (int i = 0; i < 100000000; i++)
			step(a, 0, 0);
		auto end = system_clock::now();
		cout << "Time elapsed: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;
	}

	cout << "if" << endl;
	for (int j = 0; j < 10; j++) {
		auto start = system_clock::now();
		for (int i = 0; i < 100000000; i++)
			step_if(a, 0, 0);
		auto end = system_clock::now();
		cout << "Time elapsed: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;
	}

	system("pause");
	return 0;
}
