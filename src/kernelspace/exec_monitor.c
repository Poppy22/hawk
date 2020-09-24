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

#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>
#include "process_info.h"

struct {
	__uint(type, BPF_MAP_TYPE_RINGBUF);
	__uint(max_entries, 1 << 24);
} ringbuf SEC(".maps");

long ringbuffer_flags = 0;
int filter_by_ppid;
int ppid_list[10];
int ppid_list_size;

int filter_by_name;
char name_list[5][21];
int name_list_size;

int max_n_proc;
int n_monitored_proc;

SEC("lsm/bprm_committed_creds")
void BPF_PROG(exec_audit, struct linux_binprm *bprm)
{
	if (max_n_proc != 0 && n_monitored_proc == max_n_proc) {
		return;
	}

	long pid_tgid;
	int ppid, selected_ppid = 0;
	char task_name[21];
	int selected_name = 0;
	struct process_info *process;
	struct task_struct *current_task;

	// Get the parent pid
	current_task = (struct task_struct *)bpf_get_current_task();
	ppid = (int)BPF_CORE_READ(current_task, real_parent, pid);

	if (filter_by_ppid) {
		for (int i = 0; i < 10; i++) {
			if (ppid == ppid_list[i]) {
				selected_ppid = 1;
				break;
			}
			if (i == ppid_list_size)
				break;
		}

		if (!selected_ppid) {
			return; // No need to continue monitoring this process
		}
	}

	if (filter_by_name) {
		// Get the executable name
		bpf_get_current_comm(&task_name, sizeof(task_name));
		for (int i = 0; i < 10; i++) {
			// if (__builtin_strcmp(name_list[i], task_name) == 0) {
			// 	selected_name = 1;
			// 	break;
			// }

			if (i == name_list_size)
				break;

			int j = 0;
			while (1) {
				if (name_list[i][j] != task_name[j]) {
					break;
				}
				if (name_list[i][j] == '\0' && task_name[j] == '\0') {
					selected_name = 1;
				}
				j++;
			}

			selected_name = 1;
		}

		if (!selected_name) {
			return; // No need to continue monitoring this process
		}
	}

	// Reserve space on the ringbuffer for the sample
	process = bpf_ringbuf_reserve(&ringbuf, sizeof(*process), ringbuffer_flags);
	if (!process)
		return;
	
	// Get information about the current process
	pid_tgid = bpf_get_current_pid_tgid();
	process->pid = pid_tgid;
	process->tgid = pid_tgid >> 32;
	process->ppid = ppid;
	__builtin_memcpy(process->name, task_name, sizeof(task_name));

	bpf_ringbuf_submit(process, ringbuffer_flags);
	if (max_n_proc) {
		n_monitored_proc++;
	}
}

char _license[] SEC("license") = "GPL";

