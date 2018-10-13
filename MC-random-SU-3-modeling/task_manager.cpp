#include "pch.h"

#include <chrono>
#include <limits>

#include "task_manager.h"

task_t::task_t(const config_t &_input, const size_t &_index, const chrono::milliseconds &_bad_alloc_retry_interval) :
	input(_input),
	index(_index),
	repeats_left(input.repeats),
	bad_alloc_retry_interval(_bad_alloc_retry_interval) {}

//all logics of simulation behaviour here
bool task_t::invoke() 
{
	ofstream log_file = ofstream(input.log_path, ios::out | ios::app);

	//output simulation parameters
	log_file << "--------------------------------" << endl;
	log_file << "task #" << index << " started" << endl;
	log_file << "n: "       << input.n       << endl;
	log_file << "repeats: " << input.repeats << endl;
	log_file << "output file: " << input.output_path << endl;
	log_file << "estimated output size: " << input.n * 2 * input.repeats << " bytes" << endl;

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
	for (; repeats_left > 0; --repeats_left) {
		try {
			//start measuring single iteration time, if it should be collected
#ifdef COLLECT_METRICS
#ifdef SINGLE_CYCLE_STATISTICS
			auto start = chrono::system_clock::now();
#endif
#endif
			//run the cycle
			auto output = markov_chain.do_cftp();

			ofstream output_file = ofstream(input.output_path, ios::out | ios::app | ios::binary);
			for (size_t i = 0; i < output.size(); ++i) {
				output_file.write(reinterpret_cast<const char *>(&(output[i])), sizeof(output[i]));
			}
			output_file.close();

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
		} catch (bad_alloc const &) {
			log_file << "Allocation exception was thrown on " << input.repeats - repeats_left << "th repeat" << endl;
			++repeats_left;
			// actually, do_cftp() repeats last evolve before trying to resize vector,
			// so effectively any interval used here is increased by time of one evolve_mc().
			this_thread::sleep_for(bad_alloc_retry_interval);
			continue;
		} catch (exception const &e) {
			log_file << "An exception was thrown on " << input.repeats - repeats_left << "th repeat: " << endl;
			log_file << e.what() << endl;

			log_file.close();

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

	log_file.close();

	return true;
}

task_manager_t::task_manager_t(const vector<config_t> &configs, const chrono::milliseconds &_poll_int, const long &_thread_count) :
	polling_interval(_poll_int),
	total_threads(_thread_count) {
	available_tasks.reserve(configs.size());
	for (size_t i = 0; i < configs.size(); ++i) {
		size_t index = i;
		// reuse polling interval as bad_alloc interval
		available_tasks.emplace_back(configs[index], index, polling_interval);
	}

	if (total_threads == 0) {
		total_threads = thread::hardware_concurrency();
	}
	active_tasks.reserve(total_threads);
}

void task_manager_t::run() {
	while (available_tasks.size() > 0) {
		for (auto it = active_tasks.begin(); it != active_tasks.end(); ++it) {
			if (it->wait_for(polling_interval) == future_status::ready) {
				if (it->get() == false) {
					cout << "One of the tasks failed" << endl;
				}
				active_tasks.erase(it);
				break;
			}
		}

		if (active_tasks.size() < total_threads) {
			auto future = async(launch::async, [task = move(*available_tasks.begin())]() mutable { return task.invoke(); });
			active_tasks.push_back(move(future));

			available_tasks.erase(available_tasks.begin());
			continue;
		}
	}

	for (auto& task : active_tasks) {
		task.wait();
	}
}
