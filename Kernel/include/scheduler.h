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

#endif