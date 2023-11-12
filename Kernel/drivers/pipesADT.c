#include <defs.h>
#include <memoryManagement.h>
#include <pipesADT.h>
#include <process.h>
#include <scheduler.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_PIPES 4096

#define PIPE_ADDRESS (void *) 0x80000
// read = 0, write = 1 (esta en process el define)

typedef struct pipe
{
	char buffer[PIPE_SIZE];
	uint16_t start_position;
	uint16_t size;
	int16_t input_pid;
	int16_t output_pid;
	uint8_t is_blocking;

} pipe;

typedef struct pipe_CDT
{
	pipe* pipes[MAX_PIPES];
	uint16_t last_free;
	uint16_t qty;
} pipe_CDT;

static pipe* initialize_pipe();

pipe_ADT
create_pipe_manager()
{
	pipe_ADT pipe_adt = PIPE_ADDRESS;
	for (int i = 0; i < MAX_PIPES; i++) {
		pipe_adt->pipes[i] = NULL;
	}
	pipe_adt->qty = 0;
	pipe_adt->last_free = MAX_PIPES - 1;
	return pipe_adt;
}

int8_t
pipe_open(uint16_t id, uint8_t mode)
{
	int pid = get_pid();
	return pipe_open_for_pid(pid, id, mode);
}

int8_t
pipe_close(uint16_t id)
{
	int pid = get_pid();
	return pipe_close_for_pid(pid, id);
}

int16_t
get_last_free_pipe()
{
	pipe_ADT pipe_adt = PIPE_ADDRESS;
	if (pipe_adt->qty >= MAX_PIPES) {
		return -1;
	}
	while (pipe_adt->pipes[pipe_adt->last_free] != NULL) {
		pipe_adt->last_free = (pipe_adt->last_free + MAX_PIPES - 1) % MAX_PIPES;
	}
	pipe* my_pipe = initialize_pipe();
	pipe_adt->pipes[pipe_adt->last_free] = my_pipe;
	pipe_adt->qty++;
	return pipe_adt->last_free + BUILT_IN_DESCRIPTORS ;
}

static pipe*
initialize_pipe()
{
	pipe* my_pipe = (pipe*)mm_malloc(sizeof(pipe));

	if (my_pipe == NULL) {
		return NULL;
	}

	my_pipe->start_position = 0;
	my_pipe->size = 0;
	my_pipe->input_pid = -1;
	my_pipe->output_pid = -1;
	my_pipe->is_blocking = 0;

	for (int i = 0; i < PIPE_SIZE; i++) {
		my_pipe->buffer[i] = 0;
	}

	return my_pipe;
}

int8_t
pipe_open_for_pid(uint16_t pid, uint16_t id, uint8_t mode)
{
	int16_t index = (int16_t)id - BUILT_IN_DESCRIPTORS;  
	if (index < 0 || index >= MAX_PIPES) {
		return -1;
	}

	pipe_ADT pipe_adt = PIPE_ADDRESS;

	if (pipe_adt->pipes[index] == NULL) {
		pipe_adt->pipes[index] = initialize_pipe();

		if (pipe_adt->pipes[index] == NULL) {
			return -1;
		}
		pipe_adt->qty++;
	}

	

	if (mode == WRITE && pipe_adt->pipes[index]->input_pid < 0) {
		pipe_adt->pipes[index]->input_pid = pid;
	} else if (mode == READ && pipe_adt->pipes[index]->output_pid < 0) {
		pipe_adt->pipes[index]->output_pid = pid;
	} else {
		return -1;
	}
	return 0;
}

int8_t
pipe_close_for_pid(uint16_t pid, uint16_t id)
{
	pipe_ADT pipe_adt = PIPE_ADDRESS;

	int16_t index = (int16_t)id - BUILT_IN_DESCRIPTORS;  
	if (index < 0 || index >= MAX_PIPES) {
		return -1;
	}

	pipe* my_pipe = pipe_adt->pipes[index];

	if (my_pipe == NULL) {
		return -1;
	}

	if (pid == my_pipe->input_pid) {
		char eof = -1;
		write_pipe(pid, id, &eof, 1);  // le agrego un eof al final y el q lee lo cierra dsp d leer
	} else if (pid == my_pipe->output_pid) {
		mm_free(pipe_adt->pipes[index]);
		pipe_adt->pipes[index] = NULL;
		pipe_adt->qty--;
	} else {
		return -1;
	}
	return 0;
}

static pipe*
get_pipe_by_id(pipe_ADT pipe_adt, uint16_t id)
{
	int16_t index = (int16_t)id - BUILT_IN_DESCRIPTORS;  // mismo comentario de la resta
	if (index < 0 || index >= MAX_PIPES) {
		return NULL;
	}
	return pipe_adt->pipes[index];
}

int64_t
write_pipe(uint16_t pid, uint16_t id, char* source_buffer, uint64_t len)
{
	pipe_ADT pipe_adt = PIPE_ADDRESS;
	pipe* my_pipe = get_pipe_by_id(pipe_adt, id);
	if (my_pipe == NULL || my_pipe->input_pid != pid || len == 0) {
		return -1;
	}
	uint64_t written_bytes = 0;
	while (written_bytes < len &&
	       (int)my_pipe->buffer[((my_pipe)->start_position + (my_pipe)->size) % PIPE_SIZE] != -1) {
		if (my_pipe->size >= PIPE_SIZE) {
			my_pipe->is_blocking =  1;
			set_status((uint16_t)my_pipe->input_pid, BLOCKED);
			yield();
		}
		while (my_pipe->size < PIPE_SIZE && written_bytes < len) {
			my_pipe->buffer[(((my_pipe)->start_position + (my_pipe)->size) % PIPE_SIZE)] = source_buffer[written_bytes];
			if ((int)source_buffer[written_bytes++] == -1) {
				break;
			}
			my_pipe->size++;
		}
		if (my_pipe->is_blocking) {
			set_status((uint16_t) my_pipe->output_pid, READY);
			my_pipe->is_blocking = 0;
		}
	}
	return written_bytes;
}

int64_t
read_pipe(uint16_t id, char* destination_buffer, uint64_t len)
{
	pipe_ADT pipe_adt = PIPE_ADDRESS;
	pipe* my_pipe = get_pipe_by_id(pipe_adt, id);
	if (my_pipe == NULL || my_pipe->output_pid != get_pid() || len == 0) {
		return -1;
	}
	uint8_t flag_eof = 0;
	uint64_t read_bytes = 0;
	while (read_bytes < len && !flag_eof) {
		if (my_pipe->size == 0 && (int)my_pipe->buffer[my_pipe->start_position] != -1) {
			my_pipe->size = 1;
			set_status((uint16_t)my_pipe->output_pid, BLOCKED);
			yield();
		}

		while ((my_pipe->size > 0 || (int)my_pipe->buffer[my_pipe->start_position] == -1) && read_bytes < len) {
			destination_buffer[read_bytes] = my_pipe->buffer[my_pipe->start_position];
			if ((int)destination_buffer[read_bytes++] == -1) {
				flag_eof = 1;
				break;
			}
			my_pipe->size--;
			my_pipe->start_position = (my_pipe->start_position + 1) % PIPE_SIZE;
		}
		if (my_pipe->is_blocking) {
			set_status((uint16_t) my_pipe->input_pid, READY);
			my_pipe->is_blocking = 0;
		}
	}
	return read_bytes;
}