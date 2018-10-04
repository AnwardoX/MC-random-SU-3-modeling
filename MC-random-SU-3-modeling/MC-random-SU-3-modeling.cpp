#include "pch.h"

#include <chrono>
#include "yaml-cpp/yaml.h"

#include "config.h"
#include "markov_chain.h"
#include "task_manager.h"

using namespace std;
using namespace chrono;

int main(int argc, char *argv[])
{
	if (argc != 3) {
		return -1;
	}

	string config_path(argv[1]);
	chrono::milliseconds poll_interval(stoi(argv[2]));

	YAML::Node node;
	node = YAML::LoadFile(config_path);
	auto config = node.as<vector<config_t>>();

	for (auto i = 0; i < 5; ++i) {
		task_manager_t tm(config, poll_interval);
		tm.run();
	}

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
