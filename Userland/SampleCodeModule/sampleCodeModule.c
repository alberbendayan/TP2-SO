#include <shell.h>
#include <stdint.h>
#include <stdlib.h>
#include <syscalls.h>

#define WHITE 0xffffff

int
main()
{
	uint32_t status = shell_init();
	asm_show_cursor(0);
	puts("\nShell finished executing.\n", WHITE);
	return status;
}
