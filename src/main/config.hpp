#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <map>

enum Usecase {
	EXEC_MONITOR, /* monitors process execution */
};

enum ExportFormat {
	CSV, /* exports the data in a CSV file */
	PROTOBUF, /* exports the data using protobuf */
	NO_EXPORT, /* simply prints the results to stdout */
};

class Config
{
	// EXEC MONITOR
	std::vector<int> ppid_list;
	std::vector<std::string> name_list;
	int n_proc; /* number of tasks to record */

public:
	Config();

	int exec_monitor_parse_args_run();

	// VALIDATORS
	static bool check_export_format(const char *flagname, const std::string &value);
};

#endif