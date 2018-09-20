#include "Step_function.h"

void step(array<uint8_t, n> &a, const int16_t eps_m)
{
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