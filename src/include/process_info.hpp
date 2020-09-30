#ifndef PROCESS_INFO_HPP
#define PROCESS_INFO_HPP

#include <linux/limits.h>

#define PPID_LIST_MAP_LEN 10
struct process_info {
	int ppid;
	int pid;
	int tgid;
	char name[PATH_MAX];
};

#endif
