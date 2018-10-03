#include "pch.h"

#include <chrono>
#include <thread>
//#include <windows.h>

#include "task_manager.h"

// old code

/*task_manager_t::task_manager_t(vector<config_t> input_params, chrono::milliseconds poll_int) :
	polling_interval(poll_int)
{
	MEMORYSTATUSEX memory_status;
	memory_status.dwLength = sizeof(memory_status);
	GlobalMemoryStatusEx(&memory_status);
	total_memory = memory_status.ullTotalPhys;
	available_memory = get_available_memory();
	cout << "Memory available: " << available_memory << endl;

	total_threads = thread::hardware_concurrency();
	active_futures.reserve(total_threads);

	procedures.reserve(input_params.size());

	for (size_t i = 0; i < input_params.size(); ++i) 
	{
		auto& set = input_params[i];

		auto proc = shared_ptr<task_t>(new task_t(set, i));

		if (proc->get_memory_estimation() < total_memory) 
		{
			cout << proc->index << endl;
			cout << proc->get_time_estimation() << endl;
			procedures.push_back(move(proc));
		} 
		else 
		{
			cout << "parameters set's #" << i << " memory requirements exceeds system capabilities" << endl;
		}

		// sort use "less" by default, so "greater" makes sort descending
		// reverse iterators should also work, but it's much less readable this way
		sort(procedures.rbegin(), procedures.rend(), [](shared_ptr<task_t> lhs, shared_ptr<task_t> rhs) { return lhs->get_time_estimation() < rhs->get_time_estimation(); });
	}
}

size_t task_manager_t::get_available_memory()
{
	//retrieving memory status
	MEMORYSTATUSEX memory_status;
	memory_status.dwLength = sizeof(memory_status);
	GlobalMemoryStatusEx(&memory_status);
	//return memory_status.ullAvailPhys;
	return memory_status.ullAvailPageFile;
}

void task_manager_t::start()
{
	while (procedures.size() > 0) {
		for (auto it = active_futures.begin(); it != active_futures.end(); ++it)
		{
			//check the thread status;
			//polling interval is required in order to unload CPU on polling (otherwise large amount of time will be spent on polling)
			if (it->wait_for(polling_interval) == future_status::ready)
			{
				active_futures.erase(it);
				break;
			}
		}

		//for (int i = 0; i < procedures.size(); ++i) 
		for (auto it = procedures.begin(); it != procedures.end(); ++it)
		{
			if (          active_futures.size()   < total_threads
				&& (*it)->get_memory_estimation() < available_memory)
			{
				auto task = [task = (*it), &available_memory = available_memory]()
				{
					cout << "Task # " << task->index << " started" << endl;
					available_memory -= task->get_memory_estimation();
					cout << "Memory available: " << available_memory << endl;
					task->invoke();
					available_memory += task->get_memory_estimation();
					stringstream str;
					str << "Task # " << task->index << " finished with error code " << ret << endl;
					cout << str.str();
				};

				// launch the task object
				auto a = async(launch::async, task);
				// put the task to the queue for tracking
				active_futures.push_back(move(a));
				// remove this task from the pool of available tasks...
				procedures.erase(it);
				// ...and immidiately break, because iterator is no longer valid
				break;
			}
		}
	}

	//wait for all threads that are still working
	for (auto& future : active_futures)
		future.wait();
}

*/

task_t::task_t(const config_t &_input, const size_t &_index) :
	input(_input),
	index(_index),
	repeats_left(input.repeats) {
	output_file = ofstream(input.output_path, ios::out | ios::app | ios::binary);
	log_file    = ofstream(input.   log_path, ios::out | ios::app);
}

// destructor and his company for initial implementation

/*
task_t::task_t(const task_t &task) :
	index(task.index),
	input(task.input),
	repeats_left(task.repeats_left) {
	output_file = ofstream(input.output_path, ios::out | ios::app | ios::binary);
	log_file    = ofstream(input.   log_path, ios::out | ios::app);
}

task_t & task_t::operator=(const task_t &rhs) {
	task_t task(rhs);
	return task;
}

task_t::~task_t() {
	output_file.close();
	log_file   .close();
}
*/

// test verion without using async

task_manager_t::task_manager_t(const vector<config_t> &configs, const chrono::milliseconds &_poll_int) :
	polling_interval(_poll_int) {
	available_tasks = configs;

	total_threads = thread::hardware_concurrency();
	active_tasks.reserve(total_threads);
}

void task_manager_t::run() {
	auto config = available_tasks[0];
	ofstream output_file = ofstream(config.output_path, ios::out | ios::app | ios::binary);
	ofstream log_file = ofstream(config.log_path, ios::out | ios::app);
	auto repeats_left = config.repeats;

	log_file << "task started" << endl;
	for (; repeats_left > 0; repeats_left--) {
		try {
			markov_chain_t markov_chain(config.n);
			auto output = markov_chain.do_cftp();
			for (size_t i = 0; i < output.size(); ++i) {
				output_file.write(reinterpret_cast<const char *>(&(output[i])), sizeof(output[i]));
			}
		} catch (exception &e) {
			log_file << "Exception occured on " << config.n - repeats_left << "th repeat: " << endl;
			log_file << e.what() << endl;
			output_file.flush();
		}
	}

	output_file.flush();
	log_file << "task finished" << endl;
}

// without using task class
/*
void task_manager_t::run() {
	while (available_tasks.size() > 0) {
		for (auto it = active_tasks.begin(); it != active_tasks.end(); ++it) {
			if (it->wait_for(polling_interval) == future_status::ready) {
				active_tasks.erase(it);
				break;
			}
		}

		if (active_tasks.size() < total_threads) {
			auto config = move(available_tasks[0]);
			auto future = async(launch::async, [&] {
				ofstream output_file = ofstream(config.output_path, ios::out | ios::app | ios::binary);
				ofstream log_file    = ofstream(config.   log_path, ios::out | ios::app);
				auto repeats_left = config.repeats;

				log_file << "task started" << endl;
				for (; repeats_left > 0; repeats_left--) {
					try {
						markov_chain_t markov_chain(config.n);
						auto output = markov_chain.do_cftp();
						for (size_t i = 0; i < output.size(); ++i) {
							output_file.write(reinterpret_cast<const char *>(&(output[i])), sizeof(output[i]));
						}
					} catch (exception &e) {
						log_file << "Exception occured on " << config.n - repeats_left << "th repeat: " << endl;
						log_file << e.what() << endl;
						output_file.flush();
					}
				}

				output_file.flush();
				log_file << "task finished" << endl;
			});
			active_tasks.emplace_back(move(future));
			break;
		}
	}

	for (auto& task : active_tasks) {
		task.wait();
	}
}
*/

// initial implementation

/*
bool task_t::invoke() {
	log_file << "task #" << index << " started";
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
					//active_tasks.erase(it);
					break;
				} else {
					available_tasks.push_back(move(it->first));
					break;
				}
			}
		}

		if (active_tasks.size() < total_threads) {
			auto task = move(available_tasks[0]);
			auto future = async(launch::async, [&task] { return task.invoke(); });
			active_tasks.emplace_back(move(task), move(future));
			break;
		}
	}

	for (auto& task : active_tasks) {
		task.second.wait();
	}
}
*/

