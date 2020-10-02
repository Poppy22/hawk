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
#include <bpf/libbpf.h>
#include <unistd.h>
#include <bpf/bpf.h>
#include <iostream>
#include <functional>
#include "exec_monitor.skel.h"
#include "process_info.pb.h"
#include "exec_monitor.hpp"


static int process_sample(void *ctx, void *data, size_t len)
{
	if(len < sizeof(struct process_info))
		return -1;

	struct process_info *s = (process_info*)data;
	ExecMonitor *exec_monitor = (ExecMonitor *)ctx;

	// if -n flag is set, check if n processes were already processed
	if (exec_monitor->n_proc > 0 && exec_monitor->proc_count == exec_monitor->n_proc)
		return 0;


	// count the current process if -n flag is set
	if (exec_monitor->n_proc > 0)
		exec_monitor->proc_count++;

	exec_monitor->export_data(s);
	return 0;
}

ExecMonitor *ExecMonitor::instance = nullptr;
ExecMonitor::ExecMonitor()
{
	Config *config = Config::get_instance();
	n_proc = config->n_proc;
	ppid_list = config->ppid_list;
	name_list = config->name_list;
	export_format = config->export_format;
	filename = config->filename;
}

ExecMonitor *ExecMonitor::create_instance()
{
	if (!instance)
		instance = new ExecMonitor();
	return instance;
}

ExecMonitor *ExecMonitor::get_instance()
{
	return instance;
}

int ExecMonitor::run()
{
	struct exec_monitor *skel = exec_monitor__open_and_load();
	struct ring_buffer *ringbuffer;
	int ringbuffer_fd;

	if (!skel) {
		std::cerr << "[EXEC_MONITOR]: Error loading the eBPF program.\n";
		goto out;
	}

	if (exec_monitor__attach(skel) != 0) {
		std::cerr << "[EXEC_MONITOR]: Error attaching the eBPF program.\n";
		goto out;
	}

	ringbuffer_fd = bpf_map__fd(skel->maps.ringbuf);
	if (ringbuffer_fd < 0) {
		std::cerr << "[EXEC_MONITOR]: Error accessing the ringbuffer file descriptor.\n";
		goto out;
	}

	ringbuffer = ring_buffer__new(ringbuffer_fd, process_sample, this, NULL);
	if (!ringbuffer) {
		std::cerr << "[EXEC_MONITOR]: Error allocating the ringbufffer.\n";
		goto out;
	}
	
	if (export_format == STDOUT)
		std::cout << "PPID\tPID\tTGID\tPCOM\n";

	if (ppid_list.size() != 0) {
		int list_size = std::min<int>(ppid_list.size(), PPID_LIST_MAP_LEN);
		skel->bss->ppid_list_size = list_size;
		for (int i = 0; i < list_size; i++) {
			skel->bss->ppid_list[i] = ppid_list[i];
		}
	}

	while (1) {
		// poll for new data with a timeout of -1 ms, waiting indefinitely
		int x = ring_buffer__poll(ringbuffer, -1);

		// if -n flag is set, check if n processes were already recorded
		if (n_proc > 0 && proc_count == n_proc)
			break;
	}

	exec_monitor__destroy(skel);
	return 0;

out:
	exec_monitor__destroy(skel);
	return -1;

}

void ExecMonitor::export_data(process_info *p)
{
	if (export_format == STDOUT)
	{
		std::cout << p->ppid <<"\t" << p->pid << "\t" << p->tgid << "\t" << p->name << std::endl;
		return;
	}

	file.open(filename, std::ios::out | std::ios::app | std::ios::binary);
	if (!file)
	{
		std::cerr << "Could not create file at path: " << filename << "\n";
	}

	switch (export_format) {
	case ExportFormat::CSV:
		file << p->ppid << "," << p->pid << "," << p->tgid << "," << p->name << "\n";
		break;

	case ExportFormat::PROTOBUF:
		ExecOutput exec_output;
		ProcessInfo *process = exec_output.add_process_info();
		process->set_ppid(p->ppid);
		process->set_pid(p->pid);
		process->set_tgid(p->tgid);
		process->set_name(p->name);
		if (!exec_output.SerializeToOstream(&file)) {
			std::cerr << "Failed to write the output to file." << std::endl;
			return;
		}
		break;
	}

	file.close();
}