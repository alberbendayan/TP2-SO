#ifndef _PIPE_MANAGER_H
#define _PIPE_MANAGER_H

#include <stdint.h>

#define PIPE_SIZE 4096

typedef struct pipe_CDT *pipe_ADT;

pipe_ADT create_pipe_manager();

// open
int8_t pipe_open(uint16_t id, uint8_t mode);
int8_t pipe_open_for_pid(uint16_t pid, uint16_t id, uint8_t mode);
// close
int8_t pipe_close(uint16_t id);
int8_t pipe_close_for_pid(uint16_t pid, uint16_t id);
// R&W
int64_t read_pipe(uint16_t id, char *destination_buffer, uint64_t len);
int64_t write_pipe(uint16_t pid, uint16_t id, char *source_buffer, uint64_t len);

#endif