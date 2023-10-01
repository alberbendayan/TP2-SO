#include <font.h>
#include <libc.h>
#include <rtc.h>
#include <text.h>
#include <video.h>

#define WIDTH vd_get_winwidth() / CHAR_WIDTH
#define HEIGHT vd_get_winheight() / CHAR_HEIGHT

static uint32_t curr_x = 0, curr_y = 0;
static uint8_t show_cursor = 1;

static void cursor(uint32_t color);
static void enter();

void
tx_put_char(char c, uint32_t color)
{
	switch (c) {
		case '\b': {
			if (curr_x == 0 && curr_y == 0)
				return;
			vd_put_char(' ', curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT, color);
			if (curr_x == 0) {
				curr_x = WIDTH;
				curr_y--;
			}
			curr_x--;
		} break;

		case '\n': {
			vd_put_char(' ', curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT, color);
			enter();
		} break;

		case '\t': {
			vd_put_char(' ', curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT, color);
			curr_x += 8;
			if (curr_x >= WIDTH) {
				uint32_t aux = curr_x - WIDTH;
				enter();
				curr_x = aux;
			}
		} break;

		default: {
			vd_put_char(c, curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT, color);
			curr_x++;
			if (curr_x >= WIDTH)
				enter();
		} break;
	}
	cursor(color);
}

void
tx_put_word(char* str, uint32_t color)
{
	while (*str != 0)
		tx_put_char(*str++, color);
}

void
tx_set_cursor(uint32_t x, uint32_t y, uint32_t color)
{
	if (x >= WIDTH || x < 0 || y >= HEIGHT || y < 0)
		return;
	if (show_cursor)
		vd_put_char(' ', curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT, color);
	curr_x = x;
	curr_y = y;
	cursor(color);
}

void
tx_show_cursor(uint8_t show)
{
	show_cursor = show;
}

void
tx_clear(uint32_t color)
{
	vd_clear(color);
	curr_x = 0;
	curr_y = 0;
	cursor(color);
}

static void
cursor(uint32_t color)
{
	if (show_cursor)
		vd_draw_cursor(curr_x * CHAR_WIDTH, curr_y * CHAR_HEIGHT, color);
}

static void
enter()
{
	curr_x = 0;
	if (curr_y + 1 >= HEIGHT)
		vd_scroll_up();
	else
		curr_y++;
}
