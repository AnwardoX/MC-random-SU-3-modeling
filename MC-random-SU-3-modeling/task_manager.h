#pragma once

//a flag for time and iterations metrics
#define COLLECT_METRICS
//a flag for switching between mean value statistics and single iteration statistics
//#define SINGLE_CYCLE_STATISTICS


#include <chrono>
#include <future>
#include <thread>

#include "config.h"
#include "markov_chain.h"

class task_t
{
public:
	size_t index;
	bool invoke();
	task_t(const config_t &input, const size_t &index, const chrono::milliseconds &_bad_alloc_retry_interval);

private:
	config_t input;
	int64_t repeats_left;
	chrono::milliseconds bad_alloc_retry_interval;
};

class task_manager_t
{
public:
	task_manager_t(const vector<config_t> &configs, const chrono::milliseconds &_poll_int = chrono::milliseconds(500), const long &_thread_count = 0);
	void run();

private:
	long total_threads;
	const chrono::milliseconds polling_interval;
	vector<task_t> available_tasks;
	vector<future<bool>> active_tasks;
};
