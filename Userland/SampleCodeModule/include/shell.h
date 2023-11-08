#ifndef SHELL_H
#define SHELL_H

#include <stdint.h>

// colors
typedef struct
{
	volatile uint32_t fg, bg, output, prompt, error;
} Color;
static Color color = { .fg = 0xffffff, .bg = 0x000000, .output = 0xa0a0a0, .prompt = 0x00ff00, .error = 0xff0000 };

/*
 * Inicializa y corre el shell.
 */
uint32_t shell_init();




#endif
