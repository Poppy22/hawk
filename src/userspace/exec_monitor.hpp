#ifndef EXEC_MONITOR_HPP
#define EXEC_MONITOR_HPP

#include <vector>
#include <string>
#include "../main/config.hpp"
#include "monitor.hpp"
#include "../include/process_info.hpp"

class ExecMonitor : public Monitor
{
private:
	static ExecMonitor *instance;
	ExecMonitor(Config *config);

	const std::string default_filename = "exec_monitor_output";
	std::string filename;
	ExportFormat export_format;
	std::fstream file;

	std::vector<int> ppid_list;
	std::vector<std::string> name_list;
	int n_proc; /* number of tasks to record */

public:
	int run();
	void export_data(process_info *process_info);
	ExecMonitor *create_instance(Config *config);
	ExecMonitor *get_instance();
};

#endif