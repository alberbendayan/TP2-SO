#include <exceptions.h>
#include <font.h>
#include <idtLoader.h>
#include <interrupts.h>
#include <keyboard.h>
#include <libasm.h>
#include <libc.h>
#include <memoryManagement.h>
#include <moduleLoader.h>
#include <sound.h>
#include <stdint.h>
#include <text.h>
#include <time.h>
#include <video.h>
#include <scheduler.h>

#define BLACK 0x000000
#define WHITE 0xffffff

typedef int (*EntryPoint)();

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t end_of_kernel_bin;
extern uint8_t end_of_kernel;

static const uint64_t page_size = 0x1000;
static void* const sample_code_module_addr = (void*)0x400000;
static void* const sample_data_module_addr = (void*)0x500000;
static void* const heap_address = (void*)0x600000;
static void* const mm_struct_address = (void*)0x50000;

void
clear_bss(void* bss_addr, uint64_t bss_size)
{
	memset(bss_addr, 0, bss_size);
}

void*
get_stack_base()
{
	return (void*)((uint64_t)&end_of_kernel + page_size * 8  // The size of the stack itself, 32KiB
	               - sizeof(uint64_t));                      // Begin at the top of the stack
}

void*
init_kernel_binary()
{
	void* module_addrs[] = {
		sample_code_module_addr,
		sample_data_module_addr,
	};
	load_modules(&end_of_kernel_bin, module_addrs);
	clear_bss(&bss, &end_of_kernel - &bss);
	return get_stack_base();
}

int
main()
{
	idt_loader();
	mm_init(mm_struct_address, heap_address);
	create_scheduler();
	
	// print intro wallpaper and loading message
	vd_wallpaper(2);

	// play some nice sound
	/*ti_sleep(1 * 18);
	sd_play(800, 0.1 * 18);
	ti_sleep(0.2 * 18);
	sd_play(800, 0.1 * 18);
	ti_sleep(0.1 * 18);
	sd_play(1000, 0.3 * 18);
	ti_sleep(1 * 18);*/
	
	tx_clear(BLACK);

	// aca podemos testear si queremos
	/*tx_put_word("Testeando\n",WHITE);
	char c1[100],c2[100],c3[100];
	uint_to_base(mm_heap_size(),c1,10);
	uint_to_base(mm_heap_left(),c2,10);
	uint_to_base(mm_used_heap(),c3,10);

	tx_put_word("Heap size: ",WHITE);
	tx_put_word(c1,WHITE);
	tx_put_word("\n Heap left: ",WHITE);
	tx_put_word(c2,WHITE);
	tx_put_word("\n Used heap: ",WHITE);
	tx_put_word(c3,WHITE);
	tx_put_word("\n ",WHITE);

	char * p=mm_malloc(100000);
	char * p1=mm_malloc(1000);
	char * p2=mm_malloc(8050000);


	tx_put_word("Version 1\n",WHITE);

	uint_to_base(mm_heap_size(),c1,10);
	uint_to_base(mm_heap_left(),c2,10);
	uint_to_base(mm_used_heap(),c3,10);

	tx_put_word("Heap size: ",WHITE);
	tx_put_word(c1,WHITE);
	tx_put_word("\n Heap left: ",WHITE);
	tx_put_word(c2,WHITE);
	tx_put_word("\n Used heap: ",WHITE);
	tx_put_word(c3,WHITE);
	tx_put_word("\n ",WHITE);

	tx_put_word("Version 2\n",WHITE);
	mm_free(p1);

	uint_to_base(mm_heap_size(),c1,10);
	uint_to_base(mm_heap_left(),c2,10);
	uint_to_base(mm_used_heap(),c3,10);

	tx_put_word("Heap size: ",WHITE);
	tx_put_word(c1,WHITE);
	tx_put_word("\n Heap left: ",WHITE);
	tx_put_word(c2,WHITE);
	tx_put_word("\n Used heap: ",WHITE);
	tx_put_word(c3,WHITE);
	tx_put_word("\n ",WHITE);

	
	tx_put_word("Version 3\n",WHITE);
	char *p3=mm_malloc(10000000);

	uint_to_base(mm_heap_size(),c1,10);
	uint_to_base(mm_heap_left(),c2,10);
	uint_to_base(mm_used_heap(),c3,10);

	tx_put_word("Heap size: ",WHITE);
	tx_put_word(c1,WHITE);
	tx_put_word("\n Heap left: ",WHITE);
	tx_put_word(c2,WHITE);
	tx_put_word("\n Used heap: ",WHITE);
	tx_put_word(c3,WHITE);
	tx_put_word("\n ",WHITE);

	tx_put_word("Version 4\n",WHITE);
	char *p4=mm_malloc(10000000);

	uint_to_base(mm_heap_size(),c1,10);
	uint_to_base(mm_heap_left(),c2,10);
	uint_to_base(mm_used_heap(),c3,10);

	tx_put_word("Heap size: ",WHITE);
	tx_put_word(c1,WHITE);
	tx_put_word("\n Heap left: ",WHITE);
	tx_put_word(c2,WHITE);
	tx_put_word("\n Used heap: ",WHITE);
	tx_put_word(c3,WHITE);
	tx_put_word("\n ",WHITE);


	tx_put_word("Version 5\n",WHITE);
	char *p5=mm_malloc(100000000);

	uint_to_base(mm_heap_size(),c1,10);
	uint_to_base(mm_heap_left(),c2,10);
	uint_to_base(mm_used_heap(),c3,10);

	tx_put_word("Heap size: ",WHITE);
	tx_put_word(c1,WHITE);
	tx_put_word("\n Heap left: ",WHITE);
	tx_put_word(c2,WHITE);
	tx_put_word("\n Used heap: ",WHITE);
	tx_put_word(c3,WHITE);
	tx_put_word("\n ",WHITE);

	tx_put_word("Version 6\n",WHITE);
	mm_free(p);
	mm_free(p2);
	mm_free(p3);
	mm_free(p4);
	mm_free(p5);


	uint_to_base(mm_heap_size(),c1,10);
	uint_to_base(mm_heap_left(),c2,10);
	uint_to_base(mm_used_heap(),c3,10);

	tx_put_word("Heap size: ",WHITE);
	tx_put_word(c1,WHITE);
	tx_put_word("\n Heap left: ",WHITE);
	tx_put_word(c2,WHITE);
	tx_put_word("\n Used heap: ",WHITE);
	tx_put_word(c3,WHITE);
	tx_put_word("\n ",WHITE);*/

	process_initialization p;
	char**c={"./shell",NULL};
	int fd[3]={STDIN,STDOUT,STDERR};
	p.args=c;
	p.file_descriptors= fd;
	p.name="shell";
	p.unkillable=0;
	p.priority=5;

	create_process(&p);
	yield();
	while (1)
	{
		/* code */
	}
	

	// set the restore point in case of exceptions
	exc_set_restore_point((uint64_t)sample_code_module_addr, asm_getsp(), asm_getbp());

	uint32_t status = ((EntryPoint)sample_code_module_addr)();
	tx_put_word("Exit from Userland. Back in Kernel.", WHITE);
	return status;
}
