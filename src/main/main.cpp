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