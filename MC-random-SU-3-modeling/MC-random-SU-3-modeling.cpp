#include "pch.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <functional>
#include <iostream>

#include "markov_chain.h"

using namespace std;
using namespace chrono;

int main()
{
	int16_t n, tries;
	cin >> n >> tries;
	markov_chain_t chain(n);

	vector<uint8_t> output;
	for (int16_t j = 0; j < tries; j++)
	{
		output = chain.CFTP();
		cout << "{";
		for (auto j = output.begin(); j < output.end(); j++)
			cout << *j << ",";
		cout << "}" << endl;
	}

	system("pause");
	return 0;
}
