#include "pch.h"

#include <chrono>
//#include <windows.h>

#include "task_manager.h"

task_t::task_t(const config_t &_input, const size_t &_index) :
	input(_input),
	index(_index),
	repeats_left(input.repeats) {
	output_file = ofstream(input.output_path, ios::out | ios::app | ios::binary);
	log_file    = ofstream(input.   log_path, ios::out | ios::app);
}

// destructor and his company for initial implementation


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

bool task_t::invoke() {
	log_file << "task #" << index << " started" << endl;
	log_file.flush();
	for (; repeats_left > 0; repeats_left--) {
		try {
			markov_chain_t markov_chain(input.n);
			auto output = markov_chain.do_cftp();
			for (size_t i = 0; i < output.size(); ++i) {
				output_file.write(reinterpret_cast<const char *>(&(output[i])), sizeof(output[i]));
			}
		} catch (exception &e) {
			log_file << "Exception occured on " << input.n - repeats_left << "th repeat: " << endl;
			log_file << e.what() << endl;
			output_file.flush();
			return false;
		}
	}

	output_file.flush();
	log_file << "task #" << index << " finished" << endl;
	return true;
}

task_manager_t::task_manager_t(const vector<config_t> &configs, const chrono::milliseconds &_poll_int) :
	polling_interval(_poll_int) {
	available_tasks.reserve(configs.size());
	for (size_t i = 0; i < configs.size(); ++i) {
		available_tasks.emplace_back(configs[i], i);
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
