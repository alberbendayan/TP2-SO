#include <keyboard.h>
#include <stdint.h>
#include <time.h>

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
			return keyboard_handler();
		} break;
	}
	return 0;
}
