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
	if (argc != 3) 
	{
		return -1;
	}

	string config_path(argv[1]);
	chrono::milliseconds poll_interval(stoi(argv[2]));

	YAML::Node node;
	node = YAML::LoadFile(config_path);
	auto configs = node.as<vector<config_t>>();

	// move assignment bug example code
	// from now on, use step-by-step debug and pay attention to local watches

	// task_t is a quite simple class, but it creates an ofstream in ctor (2 of them, actually)

	// checking if compiler thinks that task_t is MoveAssignable
	auto a = static_cast<bool>(is_move_assignable<task_t>());
	
	// creating new task_t object and trying to move it
	task_t t(configs[0], 0);
	auto t2 = move(t);
	// success!

	// creating vector of tasks from vector of configs
	// second argument of ctor doesn't matter
	vector<task_t> tasks;
	for (size_t i = 0; i < configs.size(); ++i) {
		tasks.emplace_back(configs[i], i);
	}

	// trying to erase first element
	// actually, any element is fine except of the last
	tasks.erase(tasks.begin());
	// ...and for some reason it removes the last element

	// https://stackoverflow.com/a/28599378 - operation principle of erase()

	// example end

	task_manager_t tm(configs, poll_interval);
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

		
	system("pause");
	return 0;
}
