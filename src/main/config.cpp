/*
 * Copyright 2020 Google LLC
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/
#include <cstdio>
#include <iostream>
#include <sstream>
#include <gflags/gflags.h>
#include "config.hpp"

const char DELIMITER = ',';

/*
	FLAGS SECTION
*/

DEFINE_string(monitor, "exec", "Defines the usecase of the program. The default value is \"exec\", which monitors process executions.");
DEFINE_validator(monitor, Config::check_monitor_type);

// EXEC FLAG - MONITOR PROCESS EXECUTION
DEFINE_string(ppid, "", "Specifies the parent pid(s) of the processes to monitor using comma-separated values. "
	"The list should contain maximum 10 elements. If more are specified, only the first 10 will be used.");
DEFINE_string(name, "", "Specifies the name(s) of the processes to monitor using comma-separated values.");
DEFINE_int32(n, 0, "Specifies the number of processes to monitor. The program will stop after n processes were executed.");

// DATA EXPORT FLAGS
DEFINE_string(format, "stdout", "Specifies the format in which to save the terminal output in a separate file. Available formats are csv, protobuf and stdout, the last being the default value.");
DEFINE_validator(format, Config::check_format_type);
DEFINE_string(output_file, "monitor_output", "Specifies the path to write the ouput or where to create the file, if it does not exist at that path.");

/*
	END OF FLAGS SECTION
*/

std::map<std::string, Usecase> usecase_map = {
	{"exec", EXEC_MONITOR},
};

std::map<std::string, ExportFormat> export_format_map = {
	{"csv", CSV},
	{"protobuf", PROTOBUF},
	{"stdout", STDOUT},
};

Config *Config::instance = nullptr;
Config::Config(int argc, char *argv[])
{
	gflags::ParseCommandLineFlags(&argc, &argv, true);
	valid = true;

	switch (usecase_map[FLAGS_monitor]) {
	case Usecase::EXEC_MONITOR:
		usecase = EXEC_MONITOR;
		if (exec_monitor_parse_args() < 0)
			valid = false;
		break;
	}

	export_format = export_format_map[FLAGS_format];
	filename = FLAGS_output_file;

	gflags::ShutDownCommandLineFlags();
}

Config *Config::create_instance(int argc, char *argv[])
{
	if (!instance)
		instance = new Config(argc, argv);
	return instance;
}

Config* Config::get_instance()
{
	return instance;
}

bool Config::is_input_valid()
{
	return valid;
}

int Config::exec_monitor_parse_args()
{
	if (!gflags::GetCommandLineFlagInfoOrDie("ppid").is_default)
	{
		std::string token;
		std::istringstream tokenStream(FLAGS_ppid);
		int ppid;
		while (std::getline(tokenStream, token, DELIMITER))
		{
			try {
				ppid = stoi(token);
			}
			catch(std::invalid_argument& e) {
				std::cerr << "Unexpected ppid format: " << token << "\n";
				return -1;
			}
			ppid_list.push_back(ppid);
		}
	}

	if (!gflags::GetCommandLineFlagInfoOrDie("name").is_default)
	{
		std::string token;
		std::istringstream tokenStream(FLAGS_name);
		while (std::getline(tokenStream, token, DELIMITER))
			name_list.push_back(token);
	}

	if (!gflags::GetCommandLineFlagInfoOrDie("n").is_default)
		n_proc = (int)FLAGS_n;

	return 0;
}

/*
	FLAG VALIDATORS
*/
bool Config::check_monitor_type(const char *flagname, const std::string &value)
{
	return usecase_map.count(value);
}

bool Config::check_format_type(const char *flagname, const std::string &value)
{
	return export_format_map.count(value);
}