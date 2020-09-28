#ifndef MONITOR_HPP
#define MONITOR_HPP

#include <string>
#include "process_info.hpp"
class Monitor {
private:
	/*
		Function called if the --format flag is specified.
		Prints the execution summary in the specified format.
	*/
	virtual void export_data(process_info *process_info) = 0;

public:
	/*
		This is the actual implementation of the usecase.
		Should load and attach the eBPF object, poll for data and process each sample.
	*/
	virtual int run() = 0;
};

#endif