#ifndef _SCHEDULER_H
#define _SCHEDULER_H
#include <process.h>
#include <stdint.h>

typedef struct scheduler_CDT *scheduler_ADT;

scheduler_ADT createScheduler();

uint16_t create_process(main_function code, char **args, char *name, uint8_t priority, int16_t file_descriptors[], uint8_t unkillable);

uint16_t getpid();

uint32_t process_is_alive(uint16_t pid);

void yield();

uint32_t set_priority(uint16_t pid, uint8_t new_priority);

int8_t set_status(uint16_t pid, uint8_t new_status);

void *schedule(void *prev_stack_pointer);

int32_t kill_current_process(int32_t ret_value);

int8_t changeFD(uint16_t pid, uint8_t position, int16_t new_fd);

#endif