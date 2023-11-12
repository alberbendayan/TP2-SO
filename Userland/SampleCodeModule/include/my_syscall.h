#ifndef MY_SYSCALLS_H
#define MY_SYSCALLS_H

#include <stdint.h>

int64_t my_getpid();
int64_t my_create_process(char *name, uint64_t argc, char *argv[]);
int64_t my_nice(uint64_t pid, uint64_t newPrio);
int64_t my_kill(uint64_t pid);
int64_t my_block(uint64_t pid);
int64_t my_unblock(uint64_t pid);
int64_t my_sem_open(uint32_t sem_id, uint64_t initialValue);
int64_t my_sem_wait(uint32_t sem_id);
int64_t my_sem_post(uint32_t sem_id);
int64_t my_sem_close(uint32_t sem_id);
int64_t my_yield();
int64_t my_wait(int64_t pid);
int64_t my_malloc(int64_t size);

#endif