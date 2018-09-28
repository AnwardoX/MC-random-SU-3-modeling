#include <chrono>
#include <thread>
#include <windows.h>

#include "task_manager.h"

task_manager_t::task_manager_t(vector<config_t> input_params, chrono::milliseconds poll_int) :
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

		auto proc = shared_ptr<Task>(new Task(set, i));

		if (proc->get_memory_estimation() < total_memory) 
		{
			cout << proc->label << endl;
			cout << proc->get_time_estimation() << endl;
			procedures.push_back(move(proc));
		} 
		else 
		{
			cout << "parameters set's #" << i << " memory requirements exceeds system capabilities" << endl;
		}

		// sort use "less" by default, so "greater" makes sort descending
		// reverse iterators should also work, but it's much less readable this way
		sort(procedures.rbegin(), procedures.rend(), [](shared_ptr<Task> lhs, shared_ptr<Task> rhs) { return lhs->get_time_estimation() < rhs->get_time_estimation(); });
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
					cout << "Task # " << task->label << " started" << endl;
					available_memory -= task->get_memory_estimation();
					cout << "Memory available: " << available_memory << endl;
					auto ret = task->Single_simulation_routine();
					available_memory += task->get_memory_estimation();
					stringstream str;
					str << "Task # " << task->label << " finished with error code " << ret << endl;
					cout << str.str();
				};

				//launch the task object
				auto a = async(launch::async, task);
				//put the task to the que for tracking
				active_futures.push_back(move(a));
				// remove this task from the pool of available tasks...
				procedures.erase(it);
				// ...and immidiately break, because iterator is no longer valid
				break;
			}
		}
	}

	//wait for all threads, that are still working
	for (auto& future : active_futures)
		future.wait();
}

Task::Task(const config_t &inp, const size_t &lab)
{
	input = inp;
	label = lab;
}

void Task::print_chain_information(std::ofstream &log_output, const size_t &number,	const markov_chain_t &chain, const string &log_file_path)
{
	//output the information
	log_output << endl;
	log_output << "Simulation # " << label << "-" << number << " finished." << endl;
	log_output << "Paramters:" << endl
		<< "K = " << simulation.Get_tree_connectivity() << endl
		<< "N = " << simulation.Get_population_size() << endl
		<< "M = " << simulation.Get_number_of_iterations() << endl
		<< "g = " << simulation.Get_interaction() << endl
		<< "OP = " << simulation.Get_order_parameter() << endl
		<< "freq = " << simulation.Get_frequency() << endl
		<< "gamma = " << simulation.Get_gamma() << endl
		<< "eigenval = " << simulation.Get_eigenvalue() << endl;
	//output simulation time
	log_output << "Evaluation time: " << simulation.Last_evaluation_time() << endl;
	//ouput errors:
	Error_status error_struct = simulation.Get_error_struct();
	log_output << "Errors: "
		<< error_struct.common_error_status << ", "
		<< error_struct.cavity_array_error << ", "
		<< error_struct.invalid_input_parameters_error << ", "
		<< error_struct.export_file_error << endl;
	/*
	//output result of KS test
	log_output << "KS test data: ";
	if (KS_failure_flag)
	{
	log_output << "fail." << endl;
	}
	else
	{
	log_output << KS_noise << ", " << sqrt((double)pop_size / 2) * KS_val << endl;
	}
	*/
	//output log file name
	log_output << "Log file name: " << log_file_path << endl;

	log_output << endl;
}

int Task::Single_simulation_routine()
{
	//open the log file
	std::ofstream log_output;
	log_output.open(input.log_path);
	if (log_output.fail())
		return 5;

	Simple_population_dynamics simulation = Simple_population_dynamics(
		input.tree_connectivity, input.population_size_exponent, input.number_of_iterations_array[0],
		input.interaction, input.order_parameter, input.frequency, input.gamma, input.eigenvalue
	);

	/*
	size_t pop_size = simulation.Get_population_size();
	vector<double> DOS_array;
	try
	{
		DOS_array = vector<double>(pop_size);
	}
	catch (exception& e)
	{
		log_output << "DOS storage memory allocation error: " << e.what() << endl;
		log_output.close();
		return 1;
	}
	*/
	
	Error_status error_struct = simulation.Get_error_struct();
	if (error_struct.common_error_status)
	{
		if (error_struct.cavity_array_error)
		{
			log_output << "Cavity array memory allocation error. The message: " << error_struct.error_message << endl;
			log_output.close();
			return 2;
		}
		if (error_struct.invalid_input_parameters_error)
		{
			log_output << "Invalid configuration parameters error." << endl;
			log_output.close();
			return 3;
		}
		return 6; // other unknown error on initialization
	}


	//double KS_val, KS_noise;
	//bool KS_failure_flag = false;
	//clock_t total_sim_time = 0; //accumulates the total amount of time

	/*
	//save the state
	for (size_t k = 0; k < pop_size; k++)
		DOS_array[k] = simulation.Density_of_states();
	//calculate the "noise" level of the test -- how 2 samples of same statistics differ
	KS_noise = Simple_population_dynamics::KS_test_value(DOS_array, simulation);
	*/

	//cycle over number of iterations array
	size_t iter;
	for (iter = 0; iter < input.number_of_iterations_array.size(); iter++)
	{
		//set number of iterations
		simulation.Set_number_of_iterations(input.number_of_iterations_array[iter]);
		//run simulation for the set amount of iterations
		simulation.Single_run();
		//total_sim_time += simulation.Last_evaluation_time();

		//export results;
		out_type out = {
			input.output_flags[0], //complex_data
			input.output_flags[1], //DOS data
			input.output_flags[2], //DOS moments
			input.output_flags[3], //DOS log moments
			input.output_flags[4], //IPR moments
			input.moments_count
		};
		simulation.Export_data(input.output_path_array[iter], out);

		//report the results to log
		Print_simulation_information(log_output, iter, simulation, input.output_path_array[iter]);
	}
	
	/*
	//calculate the difference
	KS_val = Simple_population_dynamics::KS_test_value(DOS_array, simulation);
	//check for errors
	if (KS_val < 0 || KS_noise < 0)
		KS_failure_flag = true;
	*/

	
	log_output.close();

	/*
	if (KS_failure_flag)
		return 4;
	*/
	return 0;
}

size_t Task::get_memory_estimation() const
{
	size_t size = static_cast<size_t>(1) << input.population_size_exponent;
	return 4 * size * sizeof(double);
}

size_t Task::get_time_estimation() const
{
	size_t time = 0;
	size_t size = round(pow(2, input.population_size_exponent));

	//evaluate constant of productvity, currently actual
	double time_unit = 1797.0 / (int64_t(8388608) * 30 * 127);

	size_t total_iter =	accumulate(
		input.number_of_iterations_array.begin(), 
		input.number_of_iterations_array.end(), 
		0);

	time = ceil(time_unit * total_iter * size * input.tree_connectivity * 8);

	return time;
}