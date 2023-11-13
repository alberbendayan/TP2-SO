// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <interrupts.h>
#include <time.h>

static uint64_t ticks = 0, last_ticks = 0;

void
timer_handler()
{
	ticks++;
}

uint64_t
ti_ticks()
{
	return ticks;
}

uint64_t
ti_seconds()
{
	return ticks / 18;
}

uint8_t
ti_ticked()
{
	if (last_ticks == ticks)
		return 0;
	last_ticks = ticks;
	return 1;
}

void
ti_sleep(uint32_t ticks_wanted)
{
	uint32_t last = ticks;
	while (ticks - last < ticks_wanted) {
		asm_hlt();
	}
}
