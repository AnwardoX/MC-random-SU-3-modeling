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
	if (   argc < 2
		|| argc > 4) {
		return -1;
	}

	string config_path(argv[1]);
	long thread_count = thread::hardware_concurrency();
	chrono::milliseconds poll_interval(500);

	if (argc >= 3) thread_count = stoi(argv[2]);
	if (argc >= 4) poll_interval = chrono::milliseconds(stoi(argv[3]));

	YAML::Node node;
	node = YAML::LoadFile(config_path);
	auto configs = node.as<vector<config_t>>();

	task_manager_t tm(configs, poll_interval, thread_count);
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

	//system("pause");
	return 0;
}
