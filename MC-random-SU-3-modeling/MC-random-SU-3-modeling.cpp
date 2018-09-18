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

void step(array<uint8_t, n> &pc, uint8_t eps, int64_t m) {
	if (m + 1 >= n) {
		cout << "m is out of bounds of PC" << endl;
		return;
	}

	// 16 bytes in reverse order
	__m128i shuffle_input   = _mm_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, pc[m + 1], pc[m]);
	// 0 — 1st byte, 8 — 2nd byte, 128 — zero
	//__m128i shuffle_control = _mm_set_epi8(128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 8, 0, 0, 8);
	__m128i shuffle_control = _mm_set_epi8(-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 8, 0, 0, 8);
	__m128i shuffle_output  = _mm_shuffle_epi8(shuffle_input, shuffle_control);

	// certainly wrong — just an example
	int8_t index = 0;
	pc[m]     = _mm_extract_epi8(shuffle_output, 0);
	pc[m + 1] = _mm_extract_epi8(shuffle_output, 1);
}

int main()
{
	array<uint8_t, n> pc = { 0, 1, 0, 0, 0, 1, 0};
	step(pc, 0, 0);
	return 0;
}
