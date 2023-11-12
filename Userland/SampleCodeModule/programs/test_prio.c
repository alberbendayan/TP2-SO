#include "my_syscall.h"
#include "test_util.h"

#include <stdint.h>
#include <tests.h>
#include <stdlib.h>

#define MINOR_WAIT 1000000  // TODO: Change this value to prevent a process from flooding the screen
#define WAIT \
	10000000  // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 0   
#define MEDIUM 1   
#define HIGHEST 2  
int64_t prio[TOTAL_PROCESSES] = { LOWEST, MEDIUM, HIGHEST };

void
test_prio()
{
	puts("Test priorities",0x00ff00);
	int64_t pids[TOTAL_PROCESSES];
	char* argv[] = { 0 };
	uint64_t i;

	
	for (i = 0; i < TOTAL_PROCESSES; i++) {
		pids[i] = my_create_process(endless_loop_print, 0, argv);

	}

	puts("\nPRE CHANGING PRIORITIES...\n", 0xff0000);
	bussy_wait(WAIT);
	puts("\nCHANGING PRIORITIES...\n", 0xff0000);

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_nice(pids[i], prio[i]);

	bussy_wait(WAIT);
	puts("\nBLOCKING...\n", 0xff0000);

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_block(pids[i]);

	puts("CHANGING PRIORITIES WHILE BLOCKED...\n", 0xff0000);

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_nice(pids[i], MEDIUM);

	puts("UNBLOCKING...\n", 0xff0000);

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_unblock(pids[i]);

	bussy_wait(WAIT);
	puts("\nKILLING...\n", 0xff0000);

	for (i = 0; i < TOTAL_PROCESSES; i++)
		my_kill(pids[i]);

	puts("\nFINISHED\n", 0xff0000);
}
