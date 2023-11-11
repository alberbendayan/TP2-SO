#include <keyboard.h>
#include <stdint.h>
#include <time.h>
#include <scheduler.h>

enum irq
{
	TIMER = 0,
	KEYBOARD
};

int
irq_dispatcher(uint64_t irq)
{
	switch (irq) {
		case TIMER: {
			timer_handler();
			return 0;
		} break;

		case KEYBOARD: {
			keyboard_interruption();
			return keyboard_handler();
		} break;
	}
	return 0;
}
