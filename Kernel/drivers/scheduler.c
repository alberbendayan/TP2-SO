#include <libasm.h>
#include <libc.h>
#include <linkedListADT.h>
#include <memoryManagement.h>
#include <scheduler.h>
#include <stdlib.h>
#include <video.h>

#define QTY_READY_LEVELS 5
#define MAX_PRIORITY 4
#define MIN_PRIORITY 0
#define BLOCKED_INDEX QTY_READY_LEVELS
#define MAX_PROCESSES 4096
#define IDLE_PID 0
#define SHELL_PID 1
#define QUANTUM_COEF 2
#define SCHEDULER_ADDRESS 0x60000
#define BUFFER_SIZE 128

typedef struct scheduler_CDT
{
	node* processes[MAX_PROCESSES];
	linked_list_ADT levels[QTY_READY_LEVELS + 1];
	uint16_t current_pid;
	uint16_t next_unused_pid;
	uint16_t qty_processes;
	int8_t remaining_quantum;
	int8_t kill_fg_process;
} scheduler_CDT;

static scheduler_ADT
get_address()
{
	return (scheduler_ADT)SCHEDULER_ADDRESS;
}

// hay q inicializarlo junto al kernel
scheduler_ADT
create_scheduler()
{
	scheduler_ADT scheduler = (scheduler_ADT)SCHEDULER_ADDRESS;
	for (int i = 0; i < MAX_PROCESSES; i++) {
		scheduler->processes[i] = NULL;
	}
	for (int i = 0; i < QTY_READY_LEVELS + 1; i++) {
		scheduler->levels[i] = create_linked_list_ADT();
	}
	scheduler->next_unused_pid = 0;
	scheduler->kill_fg_process = 0;
	scheduler->qty_processes = 0;
	return scheduler;
}

static process*
get_process_by_pid(uint32_t pid)
{
	scheduler_ADT scheduler = get_address();
	for (int8_t i = 0; i < scheduler->qty_processes; i++) {
		process* proc = (process*)scheduler->processes[i]->data;
		if (pid == proc->pid) {
			return proc;
		}
	}
	return NULL;
}

uint16_t
create_process(process_initialization* data)
{
	scheduler_ADT scheduler = get_address();
	if (scheduler->qty_processes >= MAX_PROCESSES) {
		return -1;
	}
	process* proc = (process*)mm_malloc(sizeof(process));

	init_process(proc,
	             scheduler->next_unused_pid,
	             scheduler->current_pid,
	             data->code,
	             data->args,
	             data->name,
	             data->priority,
	             data->file_descriptors,
	             data->unkillable);

	node* process_node;
	if (proc->pid != IDLE_PID) {
		process_node = append_element(scheduler->levels[proc->priority], (void*)proc);
	} else {
		process_node = mm_malloc(sizeof(node));
		process_node->data = (void*)proc;
	}
	scheduler->processes[proc->pid] = process_node;

	while (scheduler->processes[scheduler->next_unused_pid] != NULL) {
		scheduler->next_unused_pid = (scheduler->next_unused_pid + 1) % MAX_PROCESSES;
	}

	scheduler->qty_processes++;
	return proc->pid;
}

int32_t
unblock_process(uint64_t pid)
{
	return set_status(pid, READY);
}

int32_t
block_process(uint64_t pid)
{
	int aux = set_status(pid, BLOCKED);
	scheduler_ADT scheduler = get_address();
	if (pid == scheduler->current_pid) {
		yield();
	}
	return aux;
}

void
force_process(uint16_t pid)
{
	scheduler_ADT scheduler = get_address();
	process* p = get_process_by_pid(pid);
	if (p == NULL) {
		return;
	}
	p->status = READY;
	scheduler->current_pid = pid;
	asm_move_rsp(p->stack_pos);
}

linked_list_ADT
get_all_proccesses_snapshot()
{
	scheduler_ADT scheduler = get_address();
	linked_list_ADT snapshots = create_linked_list_ADT();
	node* my_node;
	int i = 0, j = 0;
	while (i < scheduler->qty_processes) {
		my_node = (node*)scheduler->processes[j];
		if (my_node != NULL) {
			process_snapshot* aux = mm_malloc(sizeof(process_snapshot));
			int flag = load_snapshot(aux, my_node->data);
			if (flag) {
				i++;
				my_node = append_element(snapshots, aux);
			} else {
				mm_free(aux);
			}
		}
		j++;
	}
	return snapshots;
}

static void
get_snapshot_info(process_snapshot* snapshot, char* to_ret)
{
	scheduler_ADT scheduler = get_address();
	int j = 0;
	char aux_id[8];
	char aux_pri[14];
	char aux_sp[8];
	char aux_bp[8];
	char aux_fg[8];

	j += strlen(snapshot->name);
	memcpy(to_ret, snapshot->name, j);
	while (j < 18) {
		to_ret[j++] = ' ';
	}

	uint_to_base(snapshot->pid, aux_id, 10);
	memcpy((to_ret + j), aux_id, strlen(aux_id));
	j += strlen(aux_id);

	while (j < 30) {
		to_ret[j++] = ' ';
	}

	uint_to_base(snapshot->priority, aux_pri, 10);
	memcpy((to_ret + j), aux_pri, strlen(aux_pri));
	j += strlen(aux_pri);

	while (j < 37) {
		to_ret[j++] = ' ';
	}

	uint_to_base(snapshot->stack_pos, aux_sp, 10);
	memcpy((to_ret + j), aux_sp, strlen(aux_sp));
	j += strlen(aux_sp);

	while (j < 51) {
		to_ret[j++] = ' ';
	}

	uint_to_base(snapshot->stack_base, aux_bp, 10);
	memcpy((to_ret + j), aux_bp, strlen(aux_bp));
	j += strlen(aux_sp);

	while (j < 67) {
		to_ret[j++] = ' ';
	}

	memcpy((to_ret + j), snapshot->foreground == 1 ? "Yes" : "No ", 3);
	j += 3;
	while (j < 84) {
		to_ret[j++] = ' ';
	}
	char parent[4] = "    ";
	uint_to_base(snapshot->parent_pid, parent, 10);
	memcpy((to_ret + j), parent, 4);
	j += 4;
	to_ret[j] = '\0';
}
char*
get_snapshots_info()
{
	scheduler_ADT scheduler = get_address();
	linked_list_ADT snapshots = get_all_proccesses_snapshot();

	char* to_ret = mm_malloc(scheduler->qty_processes * BUFFER_SIZE);

	char* header = "  Name              ID      Priority      SP            BP      Foreground        Parent ID\n";
	int len_header = strlen(header);
	memcpy(to_ret, header, len_header);
	int i = len_header;
	begin(snapshots);
	while (has_next(snapshots)) {
		process_snapshot* data = (process_snapshot*)next(snapshots);

		char str[BUFFER_SIZE];
		to_ret[i++] = ' ';
		to_ret[i++] = ' ';
		get_snapshot_info(data, str);
		int len = strlen(str);
		memcpy(to_ret + i, str, len);
		i += len;
		to_ret[i++] = '\n';
		mm_free(data->name);
		mm_free(data);
	}
	free_linked_list_ADT_deep(snapshots);
	to_ret[i] = '\0';
	return to_ret;
}

uint16_t
get_pid()
{
	scheduler_ADT scheduler = get_address();
	return scheduler->current_pid;
}

uint32_t
process_is_alive(uint16_t pid)
{
	scheduler_ADT scheduler = get_address();
	node* process_node = scheduler->processes[pid];
	return process_node != NULL;
}

void
yield()
{
	scheduler_ADT scheduler = SCHEDULER_ADDRESS;
	scheduler->remaining_quantum = 0;
	asm_timertick();
}

uint32_t
set_priority(uint16_t pid, uint8_t new_priority)
{
	scheduler_ADT scheduler = get_address();
	node* n = scheduler->processes[pid];

	if (n == NULL || pid == IDLE_PID) {
		return -1;
	}
	process* proc = (process*)n->data;

	if (new_priority < 0 || new_priority >= QTY_READY_LEVELS) {
		return -1;
	}
	if (proc->status == READY || proc->status == RUNNING) {
		remove_node(scheduler->levels[proc->priority], n);
		scheduler->processes[proc->pid] = append_node(scheduler->levels[new_priority], n);
	}

	proc->priority = new_priority;
	return proc->priority;
}

int8_t
set_status(uint16_t pid, uint8_t new_status)
{
	scheduler_ADT scheduler = get_address();
	node* my_node = scheduler->processes[pid];

	if (my_node == NULL || pid == IDLE_PID) {
		return -1;
	}

	process* proc = (process*)my_node->data;
	process_status old_status = proc->status;

	if (new_status == RUNNING && old_status != READY) {
		return -1;
	}
	if (new_status == proc->status) {
		return new_status;
	}
	if (new_status == BLOCKED) {
		remove_node(scheduler->levels[proc->priority], my_node);
		append_node(scheduler->levels[BLOCKED_INDEX], my_node);
	} else if (old_status == BLOCKED) {
		remove_node(scheduler->levels[BLOCKED_INDEX], my_node);
		proc->priority = MAX_PRIORITY;
		prepend_node(scheduler->levels[proc->priority], my_node);
		scheduler->remaining_quantum = 0;
	}

	proc->status = new_status;

	return proc->status;
}

process_status
get_process_status(uint16_t pid)
{
	scheduler_ADT scheduler = get_address();
	node* my_node = scheduler->processes[pid];
	if (my_node == NULL) {
		return DEAD;
	}
	return ((process*)my_node->data)->status;
}

static uint16_t
get_next(scheduler_ADT scheduler)
{
	process* my_process = NULL;

	for (int lvl = QTY_READY_LEVELS - 1; lvl >= 0 && my_process == NULL; lvl--) {
		if (!is_empty(scheduler->levels[lvl])) {
			my_process = (process*)(get_first(scheduler->levels[lvl]))->data;
		}
	}

	if (my_process == NULL) {
		return IDLE_PID;
	}
	return my_process->pid;
}

void
keyboard_interruption()
{
	scheduler_ADT scheduler = get_address();
	process* p;
	uint8_t flag = 1;
	for (int i = 2; i < scheduler->qty_processes; i++) {
		p = scheduler->processes[i]->data;
		if ((p->file_descriptors[STDIN] == STDIN && (p->status == READY || p->status == RUNNING))) {
			flag = 0;
		}
	}
	if (flag) {
		unblock_process(SHELL_PID);
	}
}

void*
schedule(void* prev_stack_pointer)
{
	static int flag_is_first = 1;
	scheduler_ADT scheduler = get_address();
	process* aux = scheduler->processes[scheduler->current_pid]->data;
	tx_put_word(aux->name,0xff0000);
	tx_put_word("\n",0xff0000);

	scheduler->remaining_quantum--;
	if (!scheduler->qty_processes || scheduler->remaining_quantum > 0) {
		return prev_stack_pointer;
	}

	process* current_process;
	node* current_node = scheduler->processes[scheduler->current_pid];

	if (current_node != NULL) {
		current_process = (process*)current_node->data;
		current_process->stack_pos = prev_stack_pointer;

		// if (!flag_is_first) {
		// 	current_process->stack_pos = prev_stack_pointer;

		// } else {
		// 	flag_is_first = 0;
		// }

		if (current_process->status == RUNNING) {
			current_process->status = READY;
		}
		uint8_t new_priority =
		    current_process->priority > 0 ? current_process->priority - 1 : current_process->priority;
		set_priority(current_process->pid, new_priority);
	}

	scheduler->current_pid = get_next(scheduler);
	current_process = scheduler->processes[scheduler->current_pid]->data;

	if (scheduler->kill_fg_process && current_process->file_descriptors[STDIN] == STDIN) {
		scheduler->kill_fg_process = 0;
		if (kill_current_process(-1) != -1) {
			asm_timertick();
		}
	}

	scheduler->remaining_quantum = (MAX_PRIORITY - current_process->priority);
	current_process->status = RUNNING;
	return current_process->stack_pos;
}

int32_t
kill_current_process(int32_t ret_value)
{
	scheduler_ADT scheduler = get_address();
	return kill_process(scheduler->current_pid, ret_value);
}

int32_t
kill_process(uint16_t pid, int32_t ret_value)
{
	scheduler_ADT scheduler = get_address();
	node* process_to_kill_node = scheduler->processes[pid];

	if (process_to_kill_node == NULL) {
		return -1;
	}

	process* process_to_kill = (process*)process_to_kill_node->data;

	if (process_to_kill->unkillable) {
		return -1;
	}

	close_file_descriptors(process_to_kill);

	uint8_t priority_index = process_to_kill->status != BLOCKED ? process_to_kill->priority : BLOCKED_INDEX;

	remove_node(scheduler->levels[priority_index], process_to_kill_node);

	process_to_kill->ret_value = ret_value;

	node* parent_node = scheduler->processes[process_to_kill->parent_pid];
	if (parent_node != NULL) {
		process* parent = (process*)parent_node->data;
		if (process_is_waiting(parent, process_to_kill->pid)) {
			set_status(process_to_kill->parent_pid, READY);
		}
	}
	process* p_aux;
	int cant = scheduler->qty_processes;
	for (int k = 0; k < cant; k++) {
		p_aux = scheduler->processes[k]->data;
		if (p_aux->parent_pid == pid) {
			kill_process(p_aux->pid, ret_value);
		}
	}

	scheduler->qty_processes--;
	scheduler->next_unused_pid = pid;
	free_process(process_to_kill);
	mm_free(process_to_kill_node);
	scheduler->processes[pid] = NULL;

	if (pid == scheduler->current_pid) {
		yield();
	}
	return 0;
}

int8_t
change_FD(uint16_t pid, uint8_t old_fd, int16_t new_fd)
{
	scheduler_ADT scheduler = get_address();
	node* process_node = scheduler->processes[pid];
	if (pid == IDLE_PID || process_node == NULL) {
		return -1;
	}
	process* proc = (process*)process_node->data;
	proc->file_descriptors[old_fd] = new_fd;
	return 0;
}

int
wait_process(uint16_t my_pid, uint16_t pid_to_wait)
{
	scheduler_ADT scheduler = get_address();
	return process_is_waiting(scheduler->processes[my_pid]->data, pid_to_wait);
}

int32_t
kill_foreground_process()
{
	scheduler_ADT scheduler = get_address();
	process* p;
	for (int i = 2; i < scheduler->qty_processes; i++) {
		p = scheduler->processes[i]->data;
		if (p->status == READY || p->status == RUNNING) {
			if (p->file_descriptors[STDIN] == STDIN) {
				unblock_process(SHELL_PID);
				return kill_process(i, 0);
			}
		}
	}
	return 0;
}

int
waitpid(uint16_t pid)
{
	scheduler_ADT scheduler = get_address();

	process* parent;
	process* child;
	node* parent_node;
	node* child_node = scheduler->processes[pid];
	if (child_node == NULL) {
		return -1;
	}
	child = child_node->data;
	parent_node = scheduler->processes[child->parent_pid];
	if (parent_node == NULL) {
		return -1;
	}
	parent = parent_node->data;
	parent->waiting_for_pid = child->pid;
	block_process(parent->pid);

	if (child->parent_pid == scheduler->current_pid) {
		yield();
	}
	return 1;
}