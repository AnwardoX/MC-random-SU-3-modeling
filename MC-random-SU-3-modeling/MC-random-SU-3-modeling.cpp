#include "pch.h"
#include <chrono>

#include "markov_chain.h"

using namespace std;
using namespace chrono;

int main()
{

	/*
	int16_t n = 10, tries = 1;
	markov_chain_t chain(n);

	vector<uint8_t> output;
	uint64_t time = 0;
	for (int16_t j = 0; j < tries; j++)
	{
		auto start = system_clock::now();
		output = chain.do_cftp();
		auto end = system_clock::now();
		auto elapsed = duration_cast<milliseconds>(end - start).count();
		time += elapsed;
		cout << "{";
		for (auto j = output.begin(); j < output.end(); ++j)
			cout << static_cast<uint16_t>(*j) << ",";
		cout << "}" << endl;
	}
	cout << "Average time: " << double(time) / tries << " ms" << endl;
	*/
	ofstream file = ofstream("test", ios::out | ios::app | ios::binary);
	for (auto j = 0; j < 256; j++)
		file << bitset<8>(j);

	system("pause");
	return 0;
}
