#include <shell.h>
#include <stdint.h>
#include <stdlib.h>
#include <syscalls.h>

#define WHITE 0xffffff

int
main()
{
	//uint32_t status = shell_init();

	// process_initialization p;
	// char ** args;
	// p.args=args;
	// p.name="Shell";
	// p.unkillable=0;
	// p.file_descriptors;
	// p.code=0;
	// p.priority=5;
	
	

	uint32_t status = 0;
	asm_show_cursor(0);
	puts("\nShell finished executing.\n", WHITE);
	return status;
}
