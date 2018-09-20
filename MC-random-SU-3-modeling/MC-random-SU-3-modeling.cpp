#include "pch.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <emmintrin.h>
#include <functional>
#include <immintrin.h>
#include <iostream>
#include <limits>
#include <random>
#include <smmintrin.h>
#include <tmmintrin.h>
#include <tuple>

#include "mt64.h"

static const int16_t n = 8;

using namespace std;
using namespace chrono;

random_device rd;
mt19937_64 gen(rd());
uniform_int_distribution<uint64_t> dist(0, numeric_limits<uint64_t>::max());

void step_shuffle(array<uint8_t, n> &a, const int16_t eps_m) {
	bool eps = signbit(double(eps_m));
	int16_t m = abs(eps_m);
	// 16 bytes in reverse order
	__m128i shuffle_reg = _mm_set_epi8(6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, a[m + 1], a[m]);
	// 128 — zero
	const __m128i shuffle_control = _mm_set_epi8(128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, !eps ^ a[m + 1], !eps ^ a[m]);
	shuffle_reg = _mm_shuffle_epi8(shuffle_reg, shuffle_control);

	a[m] = _mm_extract_epi8(shuffle_reg, 0);
	a[m + 1] = _mm_extract_epi8(shuffle_reg, 1);
}

void step_shuffle256(array<uint8_t, n> &a, const int16_t eps_m) {
	bool eps = signbit(double(eps_m));
	int16_t m = abs(eps_m);
	__m256i shuffle_reg = _mm256_setr_epi8(a[m], a[m + 1], 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6);
	// 128 — zero
	const __m256i shuffle_control = _mm256_setr_epi8(!eps ^ a[m], !eps ^ a[m + 1], 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128);
	shuffle_reg = _mm256_shuffle_epi8(shuffle_reg, shuffle_control);

	a[m] = _mm256_extract_epi8(shuffle_reg, 0);
	a[m + 1] = _mm256_extract_epi8(shuffle_reg, 1);
}

void step_shuffle256_populated(array<array<uint8_t, n>, 16> &a16, const vector<int16_t> &eps_ms) {
	vector<bool> epses(eps_ms.size());
	vector<int16_t> ms(eps_ms.size());
	for (int i = 0; i < eps_ms.size(); i++) {
		epses[i] = signbit(double(eps_ms[i]));
		ms[i] = abs(eps_ms[i]);
	}
	__m256i shuffle_reg = _mm256_setr_epi8(
		a16[0][ms[0]], a16[0][ms[0] + 1],
		a16[1][ms[1]], a16[1][ms[1] + 1],
		a16[2][ms[2]], a16[2][ms[2] + 1],
		a16[3][ms[3]], a16[3][ms[3] + 1],
		a16[4][ms[4]], a16[4][ms[4] + 1],
		a16[5][ms[5]], a16[5][ms[5] + 1],
		a16[6][ms[6]], a16[6][ms[6] + 1],
		a16[7][ms[7]], a16[7][ms[7] + 1],
		a16[8][ms[8]], a16[8][ms[8] + 1],
		a16[9][ms[9]], a16[9][ms[9] + 1],
		a16[10][ms[10]], a16[10][ms[10] + 1],
		a16[11][ms[11]], a16[11][ms[11] + 1],
		a16[12][ms[12]], a16[12][ms[12] + 1],
		a16[13][ms[13]], a16[13][ms[13] + 1],
		a16[14][ms[14]], a16[14][ms[14] + 1],
		a16[15][ms[15]], a16[15][ms[15] + 1]);
	const __m256i shuffle_control = _mm256_setr_epi8(
		!epses[0] ^ a16[0][ms[0]], !epses[0] ^ a16[0][ms[0] + 1],
		!epses[1] ^ a16[1][ms[1]], !epses[1] ^ a16[1][ms[1] + 1],
		!epses[2] ^ a16[2][ms[2]], !epses[2] ^ a16[2][ms[2] + 1],
		!epses[3] ^ a16[3][ms[3]], !epses[3] ^ a16[3][ms[3] + 1],
		!epses[4] ^ a16[4][ms[4]], !epses[4] ^ a16[4][ms[4] + 1],
		!epses[5] ^ a16[5][ms[5]], !epses[5] ^ a16[5][ms[5] + 1],
		!epses[6] ^ a16[6][ms[6]], !epses[6] ^ a16[6][ms[6] + 1],
		!epses[7] ^ a16[7][ms[7]], !epses[7] ^ a16[7][ms[7] + 1],
		!epses[8] ^ a16[8][ms[8]], !epses[8] ^ a16[8][ms[8] + 1],
		!epses[9] ^ a16[9][ms[9]], !epses[9] ^ a16[9][ms[9] + 1],
		!epses[10] ^ a16[10][ms[10]], !epses[10] ^ a16[10][ms[10] + 1],
		!epses[11] ^ a16[11][ms[11]], !epses[11] ^ a16[11][ms[11] + 1],
		!epses[12] ^ a16[12][ms[12]], !epses[12] ^ a16[12][ms[12] + 1],
		!epses[13] ^ a16[13][ms[13]], !epses[13] ^ a16[13][ms[13] + 1],
		!epses[14] ^ a16[14][ms[14]], !epses[14] ^ a16[14][ms[14] + 1],
		!epses[15] ^ a16[15][ms[15]], !epses[15] ^ a16[15][ms[15] + 1]);

	shuffle_reg = _mm256_shuffle_epi8(shuffle_reg, shuffle_control);

	a16[0][ms[0]] = _mm256_extract_epi8(shuffle_reg, 0);
	a16[0][ms[0] + 1] = _mm256_extract_epi8(shuffle_reg, 1);
	a16[1][ms[1]] = _mm256_extract_epi8(shuffle_reg, 2);
	a16[1][ms[1] + 1] = _mm256_extract_epi8(shuffle_reg, 3);
	a16[2][ms[2]] = _mm256_extract_epi8(shuffle_reg, 4);
	a16[2][ms[2] + 1] = _mm256_extract_epi8(shuffle_reg, 5);
	a16[3][ms[3]] = _mm256_extract_epi8(shuffle_reg, 6);
	a16[3][ms[3] + 1] = _mm256_extract_epi8(shuffle_reg, 7);
	a16[4][ms[4]] = _mm256_extract_epi8(shuffle_reg, 8);
	a16[4][ms[4] + 1] = _mm256_extract_epi8(shuffle_reg, 9);
	a16[5][ms[5]] = _mm256_extract_epi8(shuffle_reg, 10);
	a16[5][ms[5] + 1] = _mm256_extract_epi8(shuffle_reg, 11);
	a16[6][ms[6]] = _mm256_extract_epi8(shuffle_reg, 12);
	a16[6][ms[6] + 1] = _mm256_extract_epi8(shuffle_reg, 13);
	a16[7][ms[7]] = _mm256_extract_epi8(shuffle_reg, 14);
	a16[7][ms[7] + 1] = _mm256_extract_epi8(shuffle_reg, 15);
	a16[8][ms[8]] = _mm256_extract_epi8(shuffle_reg, 16);
	a16[8][ms[8] + 1] = _mm256_extract_epi8(shuffle_reg, 17);
	a16[9][ms[9]] = _mm256_extract_epi8(shuffle_reg, 18);
	a16[9][ms[9] + 1] = _mm256_extract_epi8(shuffle_reg, 19);
	a16[10][ms[10]] = _mm256_extract_epi8(shuffle_reg, 20);
	a16[10][ms[10] + 1] = _mm256_extract_epi8(shuffle_reg, 21);
	a16[11][ms[11]] = _mm256_extract_epi8(shuffle_reg, 22);
	a16[11][ms[11] + 1] = _mm256_extract_epi8(shuffle_reg, 23);
	a16[12][ms[12]] = _mm256_extract_epi8(shuffle_reg, 24);
	a16[12][ms[12] + 1] = _mm256_extract_epi8(shuffle_reg, 25);
	a16[13][ms[13]] = _mm256_extract_epi8(shuffle_reg, 26);
	a16[13][ms[13] + 1] = _mm256_extract_epi8(shuffle_reg, 27);
	a16[14][ms[14]] = _mm256_extract_epi8(shuffle_reg, 28);
	a16[14][ms[14] + 1] = _mm256_extract_epi8(shuffle_reg, 29);
	a16[15][ms[15]] = _mm256_extract_epi8(shuffle_reg, 30);
	a16[15][ms[15] + 1] = _mm256_extract_epi8(shuffle_reg, 31);
}

void step_shuffle256_populated_opt(array<array<uint8_t, n>, 16> &a16, const vector<int16_t> &eps_ms) {
	vector<bool> epses(eps_ms.size());
	vector<int16_t> ms(eps_ms.size());
	for (int i = 0; i < eps_ms.size(); i++) {
		epses[i] = signbit(double(eps_ms[i]));
		ms[i] = abs(eps_ms[i]);
	}

	__m256i epses_reg = _mm256_setr_epi8(
		epses[0], epses[0],
		epses[1], epses[1],
		epses[2], epses[2],
		epses[3], epses[3],
		epses[4], epses[4],
		epses[5], epses[5],
		epses[6], epses[6],
		epses[7], epses[7],
		epses[8], epses[8],
		epses[9], epses[9],
		epses[10], epses[10],
		epses[11], epses[11],
		epses[12], epses[12],
		epses[13], epses[13],
		epses[14], epses[14],
		epses[15], epses[15]
	);

	__m256i shuffle_reg = _mm256_setr_epi8(
		a16[0][ms[0]], a16[0][ms[0] + 1],
		a16[1][ms[1]], a16[1][ms[1] + 1],
		a16[2][ms[2]], a16[2][ms[2] + 1],
		a16[3][ms[3]], a16[3][ms[3] + 1],
		a16[4][ms[4]], a16[4][ms[4] + 1],
		a16[5][ms[5]], a16[5][ms[5] + 1],
		a16[6][ms[6]], a16[6][ms[6] + 1],
		a16[7][ms[7]], a16[7][ms[7] + 1],
		a16[8][ms[8]], a16[8][ms[8] + 1],
		a16[9][ms[9]], a16[9][ms[9] + 1],
		a16[10][ms[10]], a16[10][ms[10] + 1],
		a16[11][ms[11]], a16[11][ms[11] + 1],
		a16[12][ms[12]], a16[12][ms[12] + 1],
		a16[13][ms[13]], a16[13][ms[13] + 1],
		a16[14][ms[14]], a16[14][ms[14] + 1],
		a16[15][ms[15]], a16[15][ms[15] + 1]
	);

	epses_reg = _mm256_xor_si256(epses_reg, _mm256_cmpeq_epi8(epses_reg, epses_reg));

	const __m256i shuffle_control = _mm256_xor_si256(epses_reg, shuffle_reg);

	shuffle_reg = _mm256_shuffle_epi8(shuffle_reg, shuffle_control);

	a16[0][ms[0]] = _mm256_extract_epi8(shuffle_reg, 0);
	a16[0][ms[0] + 1] = _mm256_extract_epi8(shuffle_reg, 1);
	a16[1][ms[1]] = _mm256_extract_epi8(shuffle_reg, 2);
	a16[1][ms[1] + 1] = _mm256_extract_epi8(shuffle_reg, 3);
	a16[2][ms[2]] = _mm256_extract_epi8(shuffle_reg, 4);
	a16[2][ms[2] + 1] = _mm256_extract_epi8(shuffle_reg, 5);
	a16[3][ms[3]] = _mm256_extract_epi8(shuffle_reg, 6);
	a16[3][ms[3] + 1] = _mm256_extract_epi8(shuffle_reg, 7);
	a16[4][ms[4]] = _mm256_extract_epi8(shuffle_reg, 8);
	a16[4][ms[4] + 1] = _mm256_extract_epi8(shuffle_reg, 9);
	a16[5][ms[5]] = _mm256_extract_epi8(shuffle_reg, 10);
	a16[5][ms[5] + 1] = _mm256_extract_epi8(shuffle_reg, 11);
	a16[6][ms[6]] = _mm256_extract_epi8(shuffle_reg, 12);
	a16[6][ms[6] + 1] = _mm256_extract_epi8(shuffle_reg, 13);
	a16[7][ms[7]] = _mm256_extract_epi8(shuffle_reg, 14);
	a16[7][ms[7] + 1] = _mm256_extract_epi8(shuffle_reg, 15);
	a16[8][ms[8]] = _mm256_extract_epi8(shuffle_reg, 16);
	a16[8][ms[8] + 1] = _mm256_extract_epi8(shuffle_reg, 17);
	a16[9][ms[9]] = _mm256_extract_epi8(shuffle_reg, 18);
	a16[9][ms[9] + 1] = _mm256_extract_epi8(shuffle_reg, 19);
	a16[10][ms[10]] = _mm256_extract_epi8(shuffle_reg, 20);
	a16[10][ms[10] + 1] = _mm256_extract_epi8(shuffle_reg, 21);
	a16[11][ms[11]] = _mm256_extract_epi8(shuffle_reg, 22);
	a16[11][ms[11] + 1] = _mm256_extract_epi8(shuffle_reg, 23);
	a16[12][ms[12]] = _mm256_extract_epi8(shuffle_reg, 24);
	a16[12][ms[12] + 1] = _mm256_extract_epi8(shuffle_reg, 25);
	a16[13][ms[13]] = _mm256_extract_epi8(shuffle_reg, 26);
	a16[13][ms[13] + 1] = _mm256_extract_epi8(shuffle_reg, 27);
	a16[14][ms[14]] = _mm256_extract_epi8(shuffle_reg, 28);
	a16[14][ms[14] + 1] = _mm256_extract_epi8(shuffle_reg, 29);
	a16[15][ms[15]] = _mm256_extract_epi8(shuffle_reg, 30);
	a16[15][ms[15] + 1] = _mm256_extract_epi8(shuffle_reg, 31);
}

void step_shuffle512_populated(array<array<uint8_t, n>, 32> &a32, const vector<int16_t> &eps_ms) {
	vector<bool> epses(eps_ms.size());
	vector<int16_t> ms(eps_ms.size());
	for (int i = 0; i < eps_ms.size(); i++) {
		epses[i] = signbit(double(eps_ms[i]));
		ms[i] = abs(eps_ms[i]);
	}

	__m512i epses_reg = _mm512_set_epi8(
		epses[31], epses[31],
		epses[30], epses[30],
		epses[29], epses[29],
		epses[28], epses[28],
		epses[27], epses[27],
		epses[26], epses[26],
		epses[25], epses[25],
		epses[24], epses[24],
		epses[23], epses[23],
		epses[22], epses[22],
		epses[21], epses[21],
		epses[20], epses[20],
		epses[19], epses[19],
		epses[18], epses[18],
		epses[17], epses[17],
		epses[16], epses[16],
		epses[15], epses[15],
		epses[14], epses[14],
		epses[13], epses[13],
		epses[12], epses[12],
		epses[11], epses[11],
		epses[10], epses[10],
		epses[9], epses[9],
		epses[8], epses[8],
		epses[7], epses[7],
		epses[6], epses[6],
		epses[5], epses[5],
		epses[4], epses[4],
		epses[3], epses[3],
		epses[2], epses[2],
		epses[1], epses[1],
		epses[0], epses[0]
	);

	__m512i shuffle_reg = _mm512_set_epi8(
		a32[31][ms[31]], a32[31][ms[31] + 1],
		a32[30][ms[30]], a32[30][ms[30] + 1],
		a32[29][ms[29]], a32[29][ms[29] + 1],
		a32[28][ms[28]], a32[28][ms[28] + 1],
		a32[27][ms[27]], a32[27][ms[27] + 1],
		a32[26][ms[26]], a32[26][ms[26] + 1],
		a32[25][ms[25]], a32[25][ms[25] + 1],
		a32[24][ms[24]], a32[24][ms[24] + 1],
		a32[23][ms[23]], a32[23][ms[23] + 1],
		a32[22][ms[22]], a32[22][ms[22] + 1],
		a32[21][ms[21]], a32[21][ms[21] + 1],
		a32[20][ms[20]], a32[20][ms[20] + 1],
		a32[19][ms[19]], a32[19][ms[19] + 1],
		a32[18][ms[18]], a32[18][ms[18] + 1],
		a32[17][ms[17]], a32[17][ms[17] + 1],
		a32[16][ms[16]], a32[16][ms[16] + 1],
		a32[15][ms[15]], a32[15][ms[15] + 1],
		a32[14][ms[14]], a32[14][ms[14] + 1],
		a32[13][ms[13]], a32[13][ms[13] + 1],
		a32[12][ms[12]], a32[12][ms[12] + 1],
		a32[11][ms[11]], a32[11][ms[11] + 1],
		a32[10][ms[10]], a32[10][ms[10] + 1],
		a32[9][ms[9]], a32[9][ms[9] + 1],
		a32[8][ms[8]], a32[8][ms[8] + 1],
		a32[7][ms[7]], a32[7][ms[7] + 1],
		a32[6][ms[6]], a32[6][ms[6] + 1],
		a32[5][ms[5]], a32[5][ms[5] + 1],
		a32[4][ms[4]], a32[4][ms[4] + 1],
		a32[3][ms[3]], a32[3][ms[3] + 1],
		a32[2][ms[2]], a32[2][ms[2] + 1],
		a32[1][ms[1]], a32[1][ms[1] + 1],
		a32[0][ms[0]], a32[0][ms[0] + 1]
	);

	epses_reg = _mm512_xor_si512(epses_reg, _mm512_ternarylogic_epi32(epses_reg, epses_reg, epses_reg, 0b01010101));

	const __m512i shuffle_control = _mm512_xor_si512(epses_reg, shuffle_reg);

	shuffle_reg = _mm512_shuffle_epi8(shuffle_reg, shuffle_control);

	a32[0][ms[0]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[0];
	a32[0][ms[0] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[0];
	a32[1][ms[1]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[1];
	a32[1][ms[1] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[1];
	a32[2][ms[2]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[2];
	a32[2][ms[2] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[2];
	a32[3][ms[3]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[3];
	a32[3][ms[3] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[3];
	a32[4][ms[4]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[4];
	a32[4][ms[4] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[4];
	a32[5][ms[5]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[5];
	a32[5][ms[5] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[5];
	a32[6][ms[6]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[6];
	a32[6][ms[6] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[6];
	a32[7][ms[7]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[7];
	a32[7][ms[7] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[7];
	a32[8][ms[8]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[8];
	a32[8][ms[8] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[8];
	a32[9][ms[9]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[9];
	a32[9][ms[9] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[9];
	a32[10][ms[10]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[10];
	a32[10][ms[10] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[10];
	a32[11][ms[11]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[11];
	a32[11][ms[11] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[11];
	a32[12][ms[12]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[12];
	a32[12][ms[12] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[12];
	a32[13][ms[13]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[13];
	a32[13][ms[13] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[13];
	a32[14][ms[14]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[14];
	a32[14][ms[14] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[14];
	a32[15][ms[15]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[15];
	a32[15][ms[15] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[15];
	a32[16][ms[16]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[16];
	a32[16][ms[16] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[16];
	a32[17][ms[17]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[17];
	a32[17][ms[17] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[17];
	a32[18][ms[18]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[18];
	a32[18][ms[18] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[18];
	a32[19][ms[19]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[19];
	a32[19][ms[19] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[19];
	a32[20][ms[20]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[20];
	a32[20][ms[20] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[20];
	a32[21][ms[21]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[21];
	a32[21][ms[21] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[21];
	a32[22][ms[22]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[22];
	a32[22][ms[22] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[22];
	a32[23][ms[23]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[23];
	a32[23][ms[23] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[23];
	a32[24][ms[24]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[24];
	a32[24][ms[24] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[24];
	a32[25][ms[25]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[25];
	a32[25][ms[25] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[25];
	a32[26][ms[26]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[26];
	a32[26][ms[26] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[26];
	a32[27][ms[27]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[27];
	a32[27][ms[27] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[27];
	a32[28][ms[28]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[28];
	a32[28][ms[28] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[28];
	a32[29][ms[29]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[29];
	a32[29][ms[29] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[29];
	a32[30][ms[30]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[30];
	a32[30][ms[30] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[30];
	a32[31][ms[31]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[31];
	a32[31][ms[31] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[31];
}

void step(array<uint8_t, n> &a, const int16_t eps_m) {
	int16_t m = abs(eps_m);

	if (eps_m >= 0 && a[m] == 0 && a[m + 1] == 1) {
		a[m] = 1;
		a[m + 1] = 0;
	}

	if (eps_m < 0 && a[m] == 1 && a[m + 1] == 0) {
		a[m] = 0;
		a[m + 1] = 1;
	}
}

tuple<int16_t, int16_t, int16_t, int16_t> generate_eps_m() {
	uint64_t r = genrand64_int64();
	return make_tuple(static_cast<int16_t>(r >> 48),
		static_cast<int16_t>((r >> 32) & 65535),
		static_cast<int16_t>((r >> 16) & 65535),
		static_cast<int16_t>(r & 65535));
}

int main()
{
	/*for (int i = 0; i < 32; i++) {
		cout << "a32[" << i << "][ms[" << i << "]] = reinterpret_cast<uint8_t *>(&shuffle_reg)[" << i << "];" << endl;
		cout << "a32[" << i << "][ms[" << i << "] + 1] = reinterpret_cast<uint8_t *>(&shuffle_reg)[" << i << "];" << endl;
	}*/
	init_genrand64(rd());
	uniform_int_distribution<int16_t> d_signed(-6, 6);
	auto random_signed = bind(d_signed, gen);

	vector<int16_t> eps_ms(1024);
	generate(eps_ms.begin(), eps_ms.end(), random_signed);

	uniform_int_distribution<int16_t> dist16(numeric_limits<int16_t>::min(), numeric_limits<int16_t>::max());
	auto random16 = bind(dist16, gen);

	array<uint8_t, n> a;
	generate(a.begin(), a.end(), [&] { return static_cast<uint8_t>(genrand64_int64() >> 63); });

	array<array<uint8_t, n>, 16> a16;
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < n; j++) {
			a16[i][j] = static_cast<uint8_t>(genrand64_int64() >> 63);
		}
	}

	array<array<uint8_t, n>, 32> a32;
	for (int i = 0; i < 32; i++) {
		for (int j = 0; j < n; j++) {
			a32[i][j] = static_cast<uint8_t>(genrand64_int64() >> 63);
		}
	}

	tuple<int16_t, int16_t, int16_t, int16_t> b;

	cout << "step single argument" << endl;
	for (int j = 0; j < 10; j++) {
		auto start = system_clock::now();
		for (int i = 0; i < pow(2, 24); i++)
			step(a, eps_ms[j]);
		auto end = system_clock::now();
		cout << "Time elapsed: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;
	}

	/*cout << "step shuffle 256" << endl;
	for (int j = 0; j < 10; j++) {
		auto start = system_clock::now();
		for (int i = 0; i < pow(2, 25); i++)
			step_shuffle256(a, eps_ms[j]);
		auto end = system_clock::now();
		cout << "Time elapsed: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;
	}

	cout << "step shuffle 256 populated" << endl;
	for (int j = 0; j < 10; j++) {
		auto f = eps_ms.begin() + j * 16;
		auto l = eps_ms.begin() + (j + 1) * 16;
		vector<int16_t> eps_m16(f, l);
		auto start = system_clock::now();
		for (int i = 0; i < pow(2, 20); i++)
			step_shuffle256_populated(a16, eps_m16);
		auto end = system_clock::now();
		cout << "Time elapsed: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;
	}*/

	cout << "step shuffle 256 populated" << endl;
	for (int j = 0; j < 10; j++) {
		auto f = eps_ms.begin() + j * 16;
		auto l = eps_ms.begin() + (j + 1) * 16;
		vector<int16_t> eps_m16(f, l);
		auto start = system_clock::now();
		for (int i = 0; i < pow(2, 20); i++)
			step_shuffle256_populated_opt(a16, eps_m16);
		auto end = system_clock::now();
		cout << "Time elapsed: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;
	}

	cout << "step shuffle 512 populated" << endl;
	for (int j = 0; j < 10; j++) {
		auto f = eps_ms.begin() + j * 32;
		auto l = eps_ms.begin() + (j + 1) * 32;
		vector<int16_t> eps_m32(f, l);
		auto start = system_clock::now();
		for (int i = 0; i < pow(2, 19); i++)
			step_shuffle512_populated(a32, eps_m32);
		auto end = system_clock::now();
		cout << "Time elapsed: " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;
	}

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
