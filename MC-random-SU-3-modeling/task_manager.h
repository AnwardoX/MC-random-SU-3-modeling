#pragma once

//a flag for time and iterations metrics
#define COLLECT_METRICS
//a flag for switching between mean value statistics and single iteration statistics
#define SINGLE_CYCLE_STATISTICS


#include <chrono>
#include <future>
#include <thread>

#include "config.h"
#include "markov_chain.h"

class task_t
{
public:
	const size_t index;
	bool invoke();
	task_t(const config_t &input, const size_t &index);

	task_t(const task_t &task);
	task_t & operator=(const task_t &rhs);
	task_t(task_t &&task) = default;
	task_t & operator=(task_t &&rhs) = default;
	~task_t();

private:
	config_t input;
	size_t repeats_left;
	ofstream output_file;
	ofstream log_file;
};

class task_manager_t
{
public:
	task_manager_t(const vector<config_t> &configs, const chrono::milliseconds &_poll_int);
	void run();

private:
	long total_threads;
	const chrono::milliseconds polling_interval;
	vector<task_t> available_tasks;
	vector<pair<task_t, future<bool>>> active_tasks;
};
