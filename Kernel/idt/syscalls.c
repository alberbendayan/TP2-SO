#include <exceptions.h>
#include <font.h>
#include <keyboard.h>
#include <libasm.h>
#include <memoryManagement.h>
#include <pipesADT.h>
#include <process.h>
#include <rtc.h>
#include <scheduler.h>
#include <semaphore.h>
#include <sound.h>
#include <syscalls.h>
#include <text.h>
#include <time.h>
#include <video.h>

#define REGS_SIZE 20
#define EOF -1


static uint64_t write(uint64_t pid, uint64_t fd, char *source_buffer, uint64_t len);
static uint64_t read(uint64_t fd, char *destination_buffer, uint64_t len);

enum syscalls
{
	// i/o interaction
	SYS_READ = 1,
	SYS_WRITE,

	// drawing
	SYS_DRAW,
	SYS_CLEAR,
	SYS_CURSOR,
	SYS_SHOW_CURSOR,

	// properties
	SYS_WINPROPS,

	// system
	SYS_TICKS,
	SYS_SLEEP,
	SYS_REGS,
	SYS_RTC,
	SYS_SOUND,

	// memory
	SYS_MALLOC,
	SYS_FREE,
	SYS_TOTAL_HEAP,
	SYS_FREE_HEAP,
	SYS_USED_HEAP,

	// process
	SYS_CREATE_PROCESS,
	SYS_KILL_PROCESS,
	SYS_KILL_CURRENT_PROCESS,
	SYS_GET_SNAPSHOTS_INFO,
	SYS_GET_CURRENT_ID,
	SYS_BLOCK_PROCESS,
	SYS_UNBLOCK_PROCESS,
	SYS_SET_PRIORITY,
	SYS_YIELD,

	// semaphore
	SYS_SEM_OPEN,
	SYS_SEM_WAIT,
	SYS_SEM_POST,
	SYS_SEM_CLOSE,

	// Pipes
	SYS_PIPE_OPEN,
	SYS_PIPE_OPEN_FOR_PID,
	SYS_PIPE_CLOSE,
	SYS_PIPE_CLOSE_FOR_PID,
	SYS_READ_PIPE,
	SYS_WRITE_PIPE,

	SYS_WAITING_FOR_PID,
	SYS_WAIT_PID,

	SYS_GET_LAST_FREE_PIPE,
	SYS_GET_FDS

};

static uint8_t regs_flag = 0;
static uint64_t registers[REGS_SIZE];

uint64_t
syscall_dispatcher(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9)
{
	switch (rdi) {
		case SYS_READ: {
			return kb_getchar((uint8_t*)rsi);
		} break;

		case SYS_WRITE: {
			tx_put_char(rsi, rdx);
		} break;

		case SYS_DRAW: {
			vd_draw(rsi, rdx, rcx, r8, r9);
		} break;

		case SYS_CLEAR: {
			tx_clear(rsi);
		} break;

		case SYS_CURSOR: {
			tx_set_cursor(rsi, rdx, rcx);
		} break;

		case SYS_SHOW_CURSOR: {
			tx_show_cursor(rsi);
		} break;

		case SYS_WINPROPS: {
			*((uint32_t*)rsi) = vd_get_winwidth();
			*((uint32_t*)rdx) = vd_get_winheight();
			*((uint32_t*)rcx) = CHAR_WIDTH;
			*((uint32_t*)r8) = CHAR_HEIGHT;
		} break;

		case SYS_TICKS: {
			return ti_ticked();
		} break;

		case SYS_SLEEP: {
			ti_sleep(rsi);
		} break;

		case SYS_REGS: {
			exc_printreg(regs_flag ? registers : (uint64_t*)0, rsi);
		} break;

		case SYS_RTC: {
			rtc_datetime(rsi);
		} break;

		case SYS_SOUND: {
			sd_play(rsi, rdx);
		} break;

		case SYS_MALLOC: {
			return (uint64_t) mm_malloc(rsi);
		} break;

		case SYS_FREE: {
			mm_free((void*)rsi);
		} break;

		case SYS_TOTAL_HEAP: {
			return mm_heap_size(rsi);
		} break;

		case SYS_FREE_HEAP: {
			return mm_heap_left(rsi);
		} break;

		case SYS_USED_HEAP: {
			return mm_used_heap(rsi);
		} break;

		case SYS_CREATE_PROCESS: {
			return create_process((void *)rsi);
		} break;

		case SYS_KILL_PROCESS: {
			return kill_process(rsi, rdx);
		} break;

		case SYS_KILL_CURRENT_PROCESS: {
			return kill_current_process(rsi);
		} break;

		case SYS_GET_SNAPSHOTS_INFO: {
			return (uint64_t) get_snapshots_info();
		} break;

		case SYS_GET_CURRENT_ID: {
			return get_pid();
		} break;

		case SYS_BLOCK_PROCESS: {
			return block_process(rsi);
		} break;

		case SYS_UNBLOCK_PROCESS: {
			return unblock_process(rsi);
		} break;

		case SYS_SET_PRIORITY: {
			return set_priority(rsi, rdx);
		} break;

		case SYS_YIELD: {
			yield();
			return 0;
		} break;

		case SYS_SEM_OPEN: {
			return sem_open(rsi, rdx);
		} break;

		case SYS_SEM_WAIT: {
			return sem_wait(rsi);
		} break;

		case SYS_SEM_POST: {
			return sem_post(rsi);
		} break;

		case SYS_SEM_CLOSE: {
			return sem_close(rsi);
		} break;

		case SYS_PIPE_OPEN: {
			return pipe_open(rsi, rdx);
		} break;

		case SYS_PIPE_OPEN_FOR_PID: {
			return pipe_open_for_pid(rsi, rdx, rcx);
		} break;

		case SYS_PIPE_CLOSE: {
			return pipe_close(rsi);
		} break;

		case SYS_PIPE_CLOSE_FOR_PID: {
			return pipe_close_for_pid(rsi, rdx);
		} break;

		case SYS_READ_PIPE: {
			return read_pipe(rsi, (char *)rdx, rcx);
		} break;

		case SYS_WRITE_PIPE: {
			return write_pipe(rsi, rdx,(char *) rcx, r8);
		} break;

		case SYS_WAIT_PID: {
			return waitpid(rsi);
		} break;

		case SYS_GET_LAST_FREE_PIPE:{
			return get_last_free_pipe();
		}break;

		case SYS_GET_FDS:{
			return (uint64_t) get_file_descriptors();
		}break;
			
	}
	return 0;
}

void
save_registers(uint64_t* stack)
{
	regs_flag = 1;
	for (int i = 0; i < REGS_SIZE; i++) {
		registers[i] = stack[i];
	}
}



static uint64_t write(uint64_t pid, uint64_t fd, char *source_buffer, uint64_t len){
	if (fd == DEV_NULL)
	{
		return 0;
	}
	else if (fd < DEV_NULL)
	{
		return -1;
	}
	if (fd >= BUILT_IN_DESCRIPTORS)
		{
			return write_pipe(get_pid(), fd, source_buffer, len);
		}
	else if (fd == STDOUT || fd == STDERR) 
	{
		tx_put_word(source_buffer,0xffffff);
		return len;
	}
	return -1;
}

static uint64_t read(uint64_t fd, char *destination_buffer, uint64_t len){
	uint8_t *state;
	if (fd == DEV_NULL) 
	{
		destination_buffer[0] = EOF;
		return 0;
	}
	else if (fd < DEV_NULL)
	{
		return -1;
	}
	if (fd >= BUILT_IN_DESCRIPTORS) {
		return read_pipe(fd, destination_buffer, len);
	}
	else if (fd == STDIN) {
		for (uint64_t i = 0; i < len; i++) {
			destination_buffer[i] = kb_getchar(state);
			if ((int) destination_buffer[i] == EOF)
				return i + 1;
		}
		return len;
	}
	return -1;
}