#pragma once

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

	// destuctor and his company

	/*task_t(const task_t &task);
	task_t & operator=(const task_t &rhs);
	~task_t();*/

private:
	const config_t input;
	size_t repeats_left;
	ofstream output_file;
	ofstream log_file;
};

// initial implementation

/*
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
*/

class task_manager_t
{
public:
	task_manager_t(const vector<config_t> &configs, const chrono::milliseconds &_poll_int);
	void run();

private:
	long total_threads;
	const chrono::milliseconds polling_interval;
	vector<config_t> available_tasks;
	vector<future<void>> active_tasks;
};