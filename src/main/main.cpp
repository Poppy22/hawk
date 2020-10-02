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
#include <iostream>
#include "config.hpp"
#include "exec_monitor.hpp"

int main(int argc, char *argv[])
{
	int ret = 0;
	Config *config = Config::create_instance(argc, argv);

	if (!config->is_input_valid())
		return -1;

	// the set usecase is certainly valid at this point
	switch (config->usecase) {
	case Usecase::EXEC_MONITOR:
		ExecMonitor *exec_monitor = ExecMonitor::create_instance();
		ret = exec_monitor->run();
		break;
	}

	return ret;
}