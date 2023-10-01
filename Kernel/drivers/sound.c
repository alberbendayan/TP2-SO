#include <libasm.h>
#include <sound.h>
#include <time.h>

void
sd_play(uint32_t freq, uint32_t time)
{
	if (freq != 0)
		asm_sound(1193180 / freq);
	if (time != 0)
		ti_sleep(time);
	if (!((freq == 0) ^ (time == 0)))
		asm_nosound();
}
