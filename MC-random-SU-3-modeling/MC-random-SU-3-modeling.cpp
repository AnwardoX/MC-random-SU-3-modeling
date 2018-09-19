// MC-random-SU-3-modeling.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <array>
#include <emmintrin.h>
#include <iostream>
#include <smmintrin.h>
#include <tmmintrin.h>

using namespace std;

static const int64_t n = 8;

void step(array<uint8_t, n> &a, const uint8_t eps, const int64_t m) {
	if (m + 1 >= n) {
		throw "M is out of bounds of A";
	}

	// 16 bytes in reverse order
	const __m128i shuffle_input   = _mm_set_epi8(6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, a[m + 1], a[m]);
	// 0 is the 1st byte, 1 is the 2nd and so on, 128 — zero
	const __m128i shuffle_control = _mm_set_epi8(128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 1, 0, 0, 1);
	const __m128i shuffle_output  = _mm_shuffle_epi8(shuffle_input, shuffle_control);

	const int8_t index = eps ^ a[m];
	a[m]     = _mm_extract_epi8(shuffle_output, index);
	a[m + 1] = _mm_extract_epi8(shuffle_output, index + 1);
}

int main()
{
	array<uint8_t, n> a = { 1, 2, 0, 0, 0, 1, 0};
	step(a, 0, 0);
	return 0;
}
