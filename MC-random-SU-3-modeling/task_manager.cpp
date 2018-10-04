#include "pch.h"

#include <chrono>
#include <limits>

#include "task_manager.h"

task_t::task_t(const config_t &_input, const size_t &_index) :
	input(_input),
	index(_index),
	repeats_left(input.repeats) {
	output_file = ofstream(input.output_path, ios::out | ios::app | ios::binary);
	log_file    = ofstream(input.   log_path, ios::out | ios::app);
}

task_t::task_t(const task_t &task) :
	index(task.index),
	input(task.input),
	repeats_left(task.repeats_left) {
	output_file = ofstream(input.output_path, ios::out | ios::app | ios::binary);
	log_file    = ofstream(input.   log_path, ios::out | ios::app);
}

task_t & task_t::operator=(const task_t &rhs) {
	task_t t(rhs);
	return t;
}

task_t::task_t(task_t && task) :
	index(task.index),
	input(task.input),
	repeats_left(task.repeats_left),
	output_file(move(task.output_file)),
	log_file(move(task.log_file)) {}

task_t & task_t::operator=(task_t && rhs) {
	task_t t(rhs);
	return t;
}

task_t::~task_t() {
	output_file.close();
	log_file.close();
}

//all logics of simulation behaviour here
bool task_t::invoke() 
{
	//output simulation parameters
	log_file << "--------------------------------" << endl;
	log_file << "task #" << index << " started" << endl;
	log_file << "n: "       << input.n       << endl;
	log_file << "repeats: " << input.repeats << endl;

	//init statistics collection
#ifdef COLLECT_METRICS
#ifdef SINGLE_CYCLE_STATISTICS
	int64_t total_time = 0;
	int64_t total_iter = 0;
	int64_t max_iter = 0;
	int64_t min_iter = numeric_limits<int64_t>::max();
#else
	auto start = chrono::system_clock::now();
#endif
#endif

	markov_chain_t markov_chain(input.n);
	for (; repeats_left > 0; repeats_left--) 
	{
		try 
		{
			//start measuring single iteration time, if it should be collected
#ifdef COLLECT_METRICS
#ifdef SINGLE_CYCLE_STATISTICS
			auto start = chrono::system_clock::now();
#endif
#endif
			//run the cycle
			auto output = markov_chain.do_cftp();
			for (size_t i = 0; i < output.size(); ++i) 
			{
				output_file.write(reinterpret_cast<const char *>(&(output[i])), sizeof(output[i]));
			}

			//measure single cycle time 
#ifdef COLLECT_METRICS
#ifdef SINGLE_CYCLE_STATISTICS
			auto end = chrono::system_clock::now();
			total_time += chrono::duration_cast<chrono::milliseconds>(end - start).count();
			total_iter += markov_chain.get_last_evolution_length();
			max_iter = max(max_iter, markov_chain.get_last_evolution_length());
			min_iter = min(min_iter, markov_chain.get_last_evolution_length());
#endif
#endif
		} 
		catch (exception &e) 
		{
			log_file << "Exception was thrown on " << input.n - repeats_left << "th repeat: " << endl;
			log_file << e.what() << endl;
			output_file.flush();
			return false;
		}
	}

	//if only cumulative measurements where specified -- measure the total time
#ifdef COLLECT_METRICS
#ifdef SINGLE_CYCLE_STATISTICS
	//
#else
	auto end = chrono::system_clock::now();
#endif
#endif

	output_file.flush();

	log_file << "task #" << index << " finished successully" << endl;
	log_file << endl;

#ifdef COLLECT_METRICS
#ifdef SINGLE_CYCLE_STATISTICS
	log_file << "metrics (SINGLE_CYCLE_STATISTICS - yes)" << endl;
	log_file << "total time: " << total_time << " ms" << endl;
	log_file << "total iterations: " << total_iter << endl;
	log_file << "maximum number of iterations: " << max_iter << endl;
	log_file << "minimum number of iterations: " << min_iter << endl;

#else
	log_file << "metrics (SINGLE_CYCLE_STATISTICS - no)" << endl;
	log_file << "total time: " << chrono::duration_cast<chrono::milliseconds>((end - start)).count() << " ms" << endl;
#endif
#endif

	return true;
}

task_manager_t::task_manager_t(const vector<config_t> &configs, const chrono::milliseconds &_poll_int) :
	polling_interval(_poll_int) {
	available_tasks.reserve(configs.size());
	for (size_t i = 0; i < configs.size(); ++i) {
		available_tasks.emplace_back(configs[configs.size() - 1 - i], i);
	}

	total_threads = thread::hardware_concurrency();
	active_tasks.reserve(total_threads);
}

void task_manager_t::run() {
	while (available_tasks.size() > 0) {
		for (auto it = active_tasks.begin(); it != active_tasks.end(); ++it) {
			if (it->second.wait_for(polling_interval) == future_status::ready) {
				if (it->second.get() == true) {
					active_tasks.erase(it);
					break;
				} else {
					available_tasks.push_back(move(it->first));
					break;
				}
			}
		}

		if (active_tasks.size() < total_threads) {
			active_tasks.emplace_back(move(*available_tasks.rbegin()), move(future<bool>()));
			auto future = async(launch::async, [&] { return active_tasks.rbegin()->first.invoke(); });
			active_tasks.rbegin()->second = move(future);

			available_tasks.erase(available_tasks.end() - 1);
			continue;
		}
	}

	for (auto& task : active_tasks) {
		task.second.wait();
	}
}
