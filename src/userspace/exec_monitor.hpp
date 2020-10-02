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
	ExecMonitor();

	std::string filename;
	ExportFormat export_format;
	std::fstream file;

	std::vector<int> ppid_list;
	std::vector<std::string> name_list;

public:
	int run();
	void export_data(process_info *process_info);
	static ExecMonitor *create_instance();
	static ExecMonitor *get_instance();

	int n_proc = 0; /* number of tasks to record */
	int proc_count = 0; /* counter for the number of executed processes */
};

#endif