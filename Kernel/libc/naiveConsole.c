#include <libasm.h>
#include <libc.h>
#include <naiveConsole.h>

static char buffer[64] = { '0' };
static uint8_t* const video = (uint8_t*)0xB8000;
static const uint8_t color = 0x07;
static const uint32_t width = 80;
static const uint32_t height = 25;

static uint8_t* curr_video = (uint8_t*)0xB8000;
static uint8_t curr_color = 0x07;

enum time_fmt
{
	SECONDS = 0,
	MINUTES = 2,
	HOURS = 4,
	WEEKDAY = 6,
	DAY,
	MONTH,
	YEAR
};

static uint32_t uintToBase(uint64_t value, char* buffer, uint32_t base);
static int format(int n);

void
nc_print(const char* string)
{
	int i;
	for (i = 0; string[i] != 0; i++)
		nc_putchar(string[i]);
}

void
nc_putchar(char character)
{
	*curr_video++ = character;
	*curr_video++ = curr_color;
}

void
nc_newline()
{
	do {
		nc_putchar(' ');
	} while ((uint64_t)(curr_video - video) % (width * 2) != 0);
}

void
nc_print_dec(uint64_t value)
{
	nc_print_base(value, 10);
}

void
nc_print_hex(uint64_t value)
{
	nc_print_base(value, 16);
}

void
nc_print_bin(uint64_t value)
{
	nc_print_base(value, 2);
}

void
nc_print_base(uint64_t value, uint32_t base)
{
	uintToBase(value, buffer, base);
	nc_print(buffer);
}

void
nc_print_time()
{
	nc_print("Datetime: ");
	nc_print_dec(format(asm_rtc_gettime(YEAR)));
	nc_print("/");
	nc_print_dec(format(asm_rtc_gettime(MONTH)));
	nc_print("/");
	nc_print_dec(format(asm_rtc_gettime(DAY)));
	nc_print(" ");
	nc_print_dec(format(asm_rtc_gettime(HOURS)));
	nc_print(":");
	nc_print_dec(format(asm_rtc_gettime(MINUTES)));
	nc_print(":");
	nc_print_dec(format(asm_rtc_gettime(SECONDS)));
	nc_newline();
}

void
nc_backspace()
{
	*(--curr_video) = color;
	*(--curr_video) = ' ';
}

void
nc_set_color(unsigned char fg, unsigned char bg)
{
	curr_color = (bg << 4) | (fg & 0x0F);
}

void
nc_clear()
{
	int i;
	for (i = 0; i < height * width; i++) {
		video[i * 2] = ' ';
		video[i * 2 - 1] = color;
	}
	curr_video = video;
	nc_clear_color();
}

void
nc_clear_color()
{
	curr_color = color;
}

static uint32_t
uintToBase(uint64_t value, char* buff, uint32_t base)
{
	char* p = buff;
	char *p1, *p2;
	uint32_t digits = 0;

	// Calculate characters for each digit
	do {
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	} while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	// Reverse string in buffer.
	p1 = buff;
	p2 = p - 1;
	while (p1 < p2) {
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}

static int
format(int n)
{
	int dec = (n & 240) >> 4;
	int units = n & 15;
	return dec * 10 + units;
}
