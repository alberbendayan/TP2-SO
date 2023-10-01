#include <libc.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <stdint.h>

static void load_module(uint8_t** module, void* targetModuleAddress);
static uint32_t read_uint32(uint8_t** address);

void
load_modules(void* payload_start, void** module_addrs_target)
{
	int i;
	uint8_t* curr_module = (uint8_t*)payload_start;
	uint32_t module_count = read_uint32(&curr_module);

	for (i = 0; i < module_count; i++)
		load_module(&curr_module, module_addrs_target[i]);
}

static void
load_module(uint8_t** module, void* module_addr_target)
{
	uint32_t module_size = read_uint32(module);

	nc_print("  Will copy module at 0x");
	nc_print_hex((uint64_t)*module);
	nc_print(" to 0x");
	nc_print_hex((uint64_t)module_addr_target);
	nc_print(" (");
	nc_print_dec(module_size);
	nc_print(" bytes)");

	memcpy(module_addr_target, *module, module_size);
	*module += module_size;

	nc_print(" [Done]");
	nc_newline();
}

static uint32_t
read_uint32(uint8_t** addr)
{
	uint32_t result = *(uint32_t*)(*addr);
	*addr += sizeof(uint32_t);
	return result;
}
