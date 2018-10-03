#include "pch.h"

#include <chrono>
#include "yaml-cpp/yaml.h"

#include "config.h"
#include "markov_chain.h"
#include "task_manager.h"

using namespace std;
using namespace chrono;

int main()
{
	YAML::Node node;
	node = YAML::LoadFile("test.json");
	auto test_config = node.as<vector<config_t>>();

	//vector<config_t> configs = { test_config };

	task_manager_t tm(test_config, chrono::milliseconds(500));
	tm.run();

	/*
	int16_t n = 20, tries = 1;
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

	
	/*

	ofstream file = ofstream("test", ios::out | ios::app | ios::binary);
	for (uint8_t j = 0; j < 255; j++)
	{
		file.write(reinterpret_cast<char *>(&j), sizeof(j));
	}
	*/
	
	system("pause");
	return 0;
}
