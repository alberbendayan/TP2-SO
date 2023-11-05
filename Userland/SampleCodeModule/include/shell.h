#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>

/*
 * Inicializa y corre el shell.
 */
uint32_t shell_init();

typedef int (*main_function)(int argc, char** args);

typedef struct process_initialization
{
	main_function code;
	char** args;
	char* name;
	uint8_t priority;
	int16_t* file_descriptors;
	uint8_t unkillable;
} process_initialization;

#endif
