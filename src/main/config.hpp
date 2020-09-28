#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include <fstream>

enum Usecase {
	EXEC_MONITOR /* default - monitors process execution */
};

enum ExportFormat {
	STDOUT, /* default - simply prints to stdout the summary execution */
	CSV, /* exports the summary execution in csv format */
	PROTOBUF, /* exports the summary execution in binary format */
};

// Config is a singleton class
class Config
{
private:
	static Config *instance;
	Config(int argc, char *argv[]);

	int exec_monitor_parse_args();
	bool valid; /* is set if the command line arguments are valid */

public:
	static Config *create_instance(int argc, char *argv[]);
	static Config *get_instance();
	
	Usecase usecase;
	ExportFormat export_format;
	std::string filename = "monitor_output"; /* default filename */
	bool is_input_valid();

	// EXEC MONITOR
	std::vector<int> ppid_list;
	std::vector<std::string> name_list;
	int n_proc; /* number of tasks to record */

	// FLAG VALIDATORS
	static bool check_monitor_type(const char *flagname, const std::string &value);
	static bool check_format_type(const char *flagname, const std::string &value);
};

#endif