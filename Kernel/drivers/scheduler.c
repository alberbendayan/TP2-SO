#include <libasm.h>
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
#define QUANTUM_COEF 2
#define SCHEDULER_ADDRESS 0x60000

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
	for(int8_t i = 0;i<scheduler->qty_processes;i++){
		process* proc = (process*)scheduler->processes[i]->data;
		if(pid==proc->pid){
			// tx_put_word("devuelvo el proceso de nombre:", 0xffff00);
			// tx_put_word(proc->name, 0xffff00);
			// tx_put_word("\n", 0xffff00);	
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

void
force_process(uint16_t pid)
{
	
	scheduler_ADT scheduler = get_address();
	process* p = get_process_by_pid(pid);
	if(p==NULL){
		return;
	}
	p->status = READY;
	scheduler->current_pid = pid;
	asm_move_rsp(p->stack_pos);
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
	return process_node != NULL && ((process*)process_node->data)->status != ZOMBIE;
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

	if ((new_status == RUNNING && old_status != READY) || new_status == ZOMBIE || old_status == ZOMBIE) {
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

void*
schedule(void* prev_stack_pointer)
{
	static int flag_is_first = 1;
	scheduler_ADT scheduler = get_address();

	scheduler->remaining_quantum--;
	if (!scheduler->qty_processes || scheduler->remaining_quantum > 0) {
		return prev_stack_pointer;
	}

	process* current_process;
	node* current_node = scheduler->processes[scheduler->current_pid];

	if (current_node != NULL) {
		current_process = (process*)current_node->data;
		if (!flag_is_first) {
			current_process->stack_pos = prev_stack_pointer;
		} else {
			flag_is_first = 0;
		}
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

static void
destroy_zombie(scheduler_ADT scheduler, process* zombie)
{
	node* zombie_node = scheduler->processes[zombie->pid];
	scheduler->qty_processes--;
	scheduler->processes[zombie->pid] = NULL;
	free_process(zombie);
	mm_free(zombie_node);
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

	if (process_to_kill->status == ZOMBIE || process_to_kill->unkillable) {
		return -1;
	}

	close_file_descriptors(process_to_kill);

	uint8_t priority_index = process_to_kill->status != BLOCKED ? process_to_kill->priority : BLOCKED_INDEX;

	remove_node(scheduler->levels[priority_index], process_to_kill_node);

	process_to_kill->ret_value = ret_value;

	process_to_kill->status = ZOMBIE;

	// comienzo a iterar
	begin(process_to_kill->zombie_children);
	while (has_next(process_to_kill->zombie_children)) {
		destroy_zombie(scheduler, (process*)next(process_to_kill->zombie_children));
	}

	node* parent_node = scheduler->processes[process_to_kill->parent_pid];
	if (parent_node != NULL && ((process*)parent_node->data)->status != ZOMBIE) {
		process* parent = (process*)parent_node->data;
		append_node(parent->zombie_children, process_to_kill_node);
		if (process_is_waiting(parent, process_to_kill->pid)) {
			set_status(process_to_kill->parent_pid, READY);
		}
	} else {
		destroy_zombie(scheduler, process_to_kill);
	}
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
