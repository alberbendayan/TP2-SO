#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <memoryManagement.h>

#define MAX_BLOCKED 64

typedef struct semaphore_CDT* semaphore_ADT;

void create_semaphore_adt();

uint32_t sem_open(uint32_t id, uint32_t init_val);

int sem_wait(uint32_t id);

int sem_post(uint32_t id);

int sem_close(uint32_t id);

#endif