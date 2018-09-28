#pragma once

#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <numeric>
#include <thread>
#include <windows.h>

#include "config.h"
#include "markov_chain.h"

class Task
{
private:
	config_t input;
	void print_chain_information(std::ofstream &log, const size_t &number, const markov_chain_t &chain, const string &log_path);
	
public:
	size_t label;
	/*
	error codes:
	0 - OK
	1 - DOS alloc failure
	2 - cavity array alloc failure
	3 - invalid input parameters
	4 - KS test failure
	5 - log file error
	6 - uknown error
	*/
	int Single_simulation_routine();
	Task(const config_t &input, const size_t &label);
	//resource estimation
	size_t get_memory_estimation() const;
	size_t get_time_estimation() const;
};

class task_manager_t
{
public:
	task_manager_t(vector<config_t>, chrono::milliseconds poll_int);
	void start();//starts the simulation

private:
	const chrono::milliseconds polling_interval;
	vector<shared_ptr<Task>> procedures;
	vector<future<void>> active_futures;
	size_t  total_memory;
	int64_t total_threads;
	atomic_size_t available_memory;

	size_t  get_available_memory();
};

