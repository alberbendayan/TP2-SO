#include <defs.h>
#include <interrupts.h>
#include <stdlib.h>
#include <memoryManagement.h>
#include <process.h>
#include <stdlib.h>
#include <string.h>
#include <video.h>



#define STACK_SIZE (4096)

static char **alloc_arguments();
static void assign_file_descriptor(process *proc, uint8_t fd_index, int16_t fd_value, uint8_t mode);
static void close_file_descriptor(uint16_t pid, int16_t fd_value);

void process_wrapper(main_function code, char **args) {
    int len = string_array_len(args);
    int ret_value = code(len, args);
    kill_current_process(ret_value);
}

void init_process(process *proc, uint16_t pid, uint16_t parent_pid,
                  main_function code, char **args, char *name,
                  uint8_t priority, int16_t file_descriptors[], uint8_t unkillable) {
    proc->pid = pid;
    proc->parent_pid = parent_pid;
    proc->waiting_for_pid = 0;
    proc->stack_base = alloc_memory(STACK_SIZE);
    proc->argv = alloc_arguments(args);
    proc->name = alloc_memory(strlen(name) + 1);
    strcpy(proc->name, name);
    proc->priority = priority;
    void *stack_end = (void *) ((uint64_t) proc->stack_base + STACK_SIZE);
    proc->stack_pos = _initialize_stack_frame(&process_wrapper, code, stack_end, (void *) proc->argv);
    proc->status = READY;
    proc->zombie_children = create_linked_list_adt();
    proc->unkillable = unkillable;

    assign_file_descriptor(proc, STDIN, file_descriptors[STDIN], READ);
    assign_file_descriptor(proc, STDOUT, file_descriptors[STDOUT], WRITE);
    assign_file_descriptor(proc, STDERR, file_descriptors[STDERR], WRITE);
}

static void assign_file_descriptor(process *proc, uint8_t fd_index, int16_t fd_value, uint8_t mode) {
    proc->file_descriptors[fd_index] = fd_value;
    if (fd_value >= BUILT_IN_DESCRIPTORS)
        pipe_open_for_pid(proc->pid, fd_value, mode);
}

void close_file_descriptors(process *proc) {
    close_file_descriptor(proc->pid, proc->file_descriptors[STDIN]);
    close_file_descriptor(proc->pid, proc->file_descriptors[STDOUT]);
    close_file_descriptor(proc->pid, proc->file_descriptors[STDERR]);
}

static void close_file_descriptor(uint16_t pid, int16_t fd_value) {
    if (fd_value >= BUILT_IN_DESCRIPTORS)
        pipe_close_for_pid(pid, fd_value);
}

static char **alloc_arguments(char **args) {
    int argc = string_array_len(args), total_args_len = 0; // falta hacer bien esta func
    int args_len[argc];
    for (int i = 0; i < argc; i++) {
        args_len[i] = strlen(args[i]) + 1;
        total_args_len += args_len[i];
    }
    char **new_args_array = (char **) alloc_memory(total_args_len + sizeof(char **) * (argc + 1));
    char *char_position = (char *) new_args_array + (sizeof(char **) * (argc + 1));
    for (int i = 0; i < argc; i++) {
        new_args_array[i] = char_position;
        memcpy(char_position, args[i], args_len[i]);
        char_position += args_len[i];
    }
    new_args_array[argc] = NULL;
    return new_args_array;
}
void free_process(process *proc) {
    free_linked_list_adt(proc->zombie_children);
    free(proc->stack_base);
    free(proc->name);
    free(proc);
}

process_snapshot *load_snapshot(process_snapshot *snapshot, process *proc) {
    snapshot->name = alloc_memory(strlen(proc->name));
    strcpy(snapshot->name, proc->name);
    snapshot->pid = proc->pid;
    snapshot->parent_pid = proc->parent_pid;
    snapshot->stack_base = proc->stack_base;
    snapshot->priority = proc->priority;
    snapshot->status = proc->status;
    snapshot->foreground = proc->file_descriptors[STDIN] == STDIN;
    snapshot->stack_pos = proc->stack_pos;
    return snapshot;
}

int process_is_waiting(process *proc, uint16_t pid_to_wait) {
    return proc->waiting_for_pid == pid_to_wait && proc->status == BLOCKED;
}

int get_zombies_snapshots(int process_index, process_snapshot ps_array[], process *next_process) {
    linked_list_ADT zombie_children = next_process->zombie_children;
    begin(zombie_children);
    while (has_next(zombie_children))
        load_snapshot(&ps_array[process_index++], next(zombie_children));
    return process_index;
}
