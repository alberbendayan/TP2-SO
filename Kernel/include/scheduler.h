#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#include <process.h>
#include <stdint.h>

typedef struct scheduler_CDT* scheduler_ADT;

scheduler_ADT create_scheduler();

uint16_t create_process(process_initialization* data);

uint16_t get_pid();

uint32_t process_is_alive(uint16_t pid);

void yield();

uint32_t set_priority(uint16_t pid, uint8_t new_priority);

int8_t set_status(uint16_t pid, uint8_t new_status);

void* schedule(void* prev_stack_pointer);

int32_t kill_current_process(int32_t ret_value);

int8_t change_FD(uint16_t pid, uint8_t old_fd, int16_t new_fd);

int32_t kill_process(uint16_t pid, int32_t ret_value);

process_status get_process_status(uint16_t pid);

void force_process(uint16_t pid);

linked_list_ADT get_all_proccesses_snapshot();

char* get_snapshots_info();

int32_t unblock_process(uint64_t pid);

int32_t block_process(uint64_t pid);

int32_t kill_foreground_process();

void keyboard_interruption();

int wait_process(uint16_t my_pid,uint16_t pid_to_wait);

int waitpid(uint16_t pid);

int * get_file_descriptors();
#endif