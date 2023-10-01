#include <libasm.h>
#include <pong.h>
#include <stdlib.h>
#include <syscalls.h>

// dimensions
#define BAR_WIDTH 10
#define BAR_HEIGHT 80
#define BALL_SIZE 10
#define BALL_SPEED 15
#define MOV_DIFF 20
#define BORDER 4

// sound
#define SOUND_COUNT 1

// index for player movement state
enum player_movement
{
	P1_UP = 0,
	P1_DOWN,
	P2_UP,
	P2_DOWN
};

typedef enum
{
	QUIT = 0,
	RUNNING,
	RELOAD,
	WINNER
} State;

typedef struct
{
	uint32_t width, height;
	uint32_t font_width, font_height;
	uint32_t fg, bg;
} Window;

typedef struct
{
	uint32_t x, y;
	int8_t movement;
	uint32_t score;
} Player;

typedef struct
{
	uint32_t x, y;
	uint32_t speed_x, speed_y;
} Ball;

static State state;
static Window window;
static Player p1, p2;
static Ball ball;

static uint8_t buttons[4] = { 0 };
static char buff[2];
static uint8_t sound_count;

// messages
static char title[] = "PONG - THE GAME";
static char subtitle[] = "PRESS ENTER TO START";
static char p1score[] = "Player 1: ";
static char p2score[] = "Player 2: ";
static char p1winner[] = "PLAYER 1 WINS!!!";
static char p2winner[] = "PLAYER 2 WINS!!!";
static uint32_t title_len = sizeof(title) / sizeof(title[0]);
static uint32_t subtitle_len = sizeof(subtitle) / sizeof(subtitle[0]);
static uint32_t p2score_len = sizeof(p2score) / sizeof(p2score[0]);
static uint32_t p1winner_len = sizeof(p1winner) / sizeof(p1winner[0]);
static uint32_t p2winner_len = sizeof(p2winner) / sizeof(p2winner[0]);

static void game_loop();
static void intro_message();
static void print_winner(char* msg, uint32_t len);
static void process_key(uint8_t key, uint8_t state);
static void draw_window();
static void init_players();
static void init_ball();
static void update_players();
static void update_ball();
static void draw_players();
static void draw_player(Player* p);
static void draw_ball();

void
start_game(uint32_t fg, uint32_t bg)
{
	// hide cursor
	asm_show_cursor(0);

	// initialize the window
	window.fg = fg;
	window.bg = bg;
	asm_winprops(&window.width, &window.height, &window.font_width, &window.font_height);

	// run until QUIT is set
	do {
		game_loop();
	} while (state == RELOAD);

	// if there's a winner print the message
	if (state == WINNER)
		print_winner(p1.score == 5 ? p1winner : p2winner, p1.score == 5 ? p1winner_len : p2winner_len);

	// show the cursor again
	asm_show_cursor(1);
}

static void
game_loop()
{
	// play a sound for the player who scored
	if (state == RELOAD) {
		asm_sound(600, 0.2 * 18);
		asm_sound(900, 0.3 * 18);
	}

	asm_clear(window.bg);
	draw_window();
	init_players();
	init_ball();

	state = RUNNING;
	intro_message();
	asm_draw(window.width / 2, 0, 1, window.height, window.fg);

	for (int i = 0; i < 4; i++)
		buttons[i] = RELEASED;

	uint8_t c, c_status;
	while (state == RUNNING) {
		c = getchar(&c_status);
		process_key(c, c_status);
		if (asm_ticked()) {
			update_players();
			draw_players();
			update_ball();
			draw_ball();

			if (sound_count == 0)
				asm_sound(0, 0);
			else
				sound_count--;

			if (p1.score == 5 || p2.score == 5)
				state = WINNER;
		}
	}
}

static void
intro_message()
{
	uint32_t ypos = window.height / (2 * window.font_height);
	asm_cursor((window.width / window.font_width - title_len) / 2, ypos - 4, window.fg);
	puts(title, window.fg);
	asm_cursor((window.width / window.font_width - subtitle_len) / 2, ypos - 3, window.fg);
	puts(subtitle, window.fg);

	uint8_t c, c_status;
	while (!((c = getchar(&c_status)) == '\n' && c_status == PRESSED)) {
		if (c == 'q') {
			state = QUIT;
			break;
		}
	}

	asm_draw((window.width - (subtitle_len + 1) * window.font_width) / 2,
	         window.height / 2 - 4 * window.font_height,
	         subtitle_len * window.font_height,
	         2 * window.font_height,
	         window.bg);
}

static void
print_winner(char* msg, uint32_t len)
{
	draw_players();

	asm_cursor((window.width / window.font_width - len) / 2, window.height / (2 * window.font_height), window.fg);
	puts(msg, window.fg);

	asm_sound(800, 0.1 * 18);
	asm_sleep(0.2 * 18);
	asm_sound(800, 0.1 * 18);
	asm_sleep(0.1 * 18);
	asm_sound(1000, 0.3 * 18);

	uint8_t c_status;
	while (!(getchar(&c_status) == '\n' && c_status == PRESSED)) {}
}

static void
process_key(uint8_t key, uint8_t status)
{
	if (key >= 'A' && key <= 'Z')
		key += 'a' - 'A';

	switch (key) {
		case 'w': {
			buttons[P1_UP] = status;
		} break;

		case 's': {
			buttons[P1_DOWN] = status;
		} break;

		case 'i': {
			buttons[P2_UP] = status;
		} break;

		case 'k': {
			buttons[P2_DOWN] = status;
		} break;

		case 'q': {
			state = QUIT;
		} break;
	}
}

static void
draw_window()
{
	asm_draw(0, 0, BORDER, window.height, window.fg);
	asm_draw(BORDER, 0, window.width - BORDER, BORDER, window.fg);
	asm_draw(window.width - BORDER, 0, BORDER, window.height, window.fg);
	asm_draw(BORDER, window.height - BORDER, window.width - BORDER, BORDER, window.fg);
}

// PLAYERS LOGIC
static void
init_players()
{
	p1.x = 20;
	p2.x = window.width - BAR_WIDTH - p1.x;
	p1.y = p2.y = (window.height - BAR_HEIGHT) / 2;
	p1.movement = p2.movement = 0;
	if (state != RELOAD)
		p1.score = p2.score = 0;
	asm_draw(p1.x, p1.y, BAR_WIDTH, BAR_HEIGHT, window.fg);
	asm_draw(p2.x, p2.y, BAR_WIDTH, BAR_HEIGHT, window.fg);
	draw_players();
}

static void
update_players()
{
	if (buttons[P1_UP] == PRESSED && buttons[P1_DOWN] == RELEASED)
		p1.movement = -1;
	if (buttons[P1_UP] == RELEASED && buttons[P1_DOWN] == PRESSED)
		p1.movement = 1;
	if (buttons[P1_UP] == RELEASED && buttons[P1_DOWN] == RELEASED)
		p1.movement = 0;
	if (buttons[P2_UP] == PRESSED && buttons[P2_DOWN] == RELEASED)
		p2.movement = -1;
	if (buttons[P2_UP] == RELEASED && buttons[P2_DOWN] == PRESSED)
		p2.movement = 1;
	if (buttons[P2_UP] == RELEASED && buttons[P2_DOWN] == RELEASED)
		p2.movement = 0;
}

static void
draw_players()
{
	draw_player(&p1);
	asm_cursor(0, 0, window.fg);
	uint_to_base(p1.score, buff, 10);
	puts(p1score, window.fg);
	puts(buff, window.fg);

	draw_player(&p2);
	asm_cursor(window.width / window.font_width - p2score_len, 0, window.fg);
	uint_to_base(p2.score, buff, 10);
	puts(p2score, window.fg);
	puts(buff, window.fg);
}

static void
draw_player(Player* p)
{
	if (p->movement == 1) {
		if (p->y + BAR_HEIGHT >= window.height - MOV_DIFF)
			return;
		asm_draw(p->x, p->y, BAR_WIDTH, MOV_DIFF, window.bg);
		asm_draw(p->x, p->y + BAR_HEIGHT, BAR_WIDTH, MOV_DIFF, window.fg);
	} else if (p->movement == -1) {
		if (p->y < MOV_DIFF)
			return;
		asm_draw(p->x, p->y + BAR_HEIGHT - MOV_DIFF, BAR_WIDTH, MOV_DIFF, window.bg);
		asm_draw(p->x, p->y - MOV_DIFF, BAR_WIDTH, MOV_DIFF, window.fg);
	}

	p->y += MOV_DIFF * p->movement;
}

// BALL LOGIC
static void
init_ball()
{
	ball.x = (window.width - BALL_SIZE) / 2;
	ball.y = (window.height - BALL_SIZE) / 2;
	ball.speed_x = ball.speed_y = BALL_SPEED;
	asm_draw(ball.x, ball.y, BALL_SIZE, BALL_SIZE, window.fg);
}

static void
update_ball()
{
	// rebota en los limites horizontales
	if (ball.y + ball.speed_y <= BORDER || ball.y + ball.speed_y >= window.height - BORDER) {
		ball.speed_y *= -1;
		asm_sound(100, 0);
		sound_count = SOUND_COUNT;
	}

	// dentro o fuera
	if (ball.x + ball.speed_x >= p1.x && ball.x + ball.speed_x + BALL_SIZE <= p2.x + BAR_WIDTH) {
		// rebota en algún jugador
		if ((ball.x + ball.speed_x <= p1.x + BAR_WIDTH && ball.y + ball.speed_y >= p1.y &&
		     ball.y + ball.speed_y <= p1.y + BAR_HEIGHT) ||
		    (ball.x + ball.speed_x + BALL_SIZE >= p2.x && ball.y + ball.speed_y >= p2.y &&
		     ball.y + ball.speed_y <= p2.y + BAR_HEIGHT)) {
			ball.speed_x *= -1;
			asm_sound(100, 0);
			sound_count = SOUND_COUNT;
		}
	} else {
		// se fue la pelota. hay que sumar puntos y reiniciar
		state = RELOAD;
		if (ball.x + ball.speed_x < p1.x)
			p2.score++;
		if (ball.x + ball.speed_x + BALL_SIZE > p2.x + BAR_WIDTH)
			p1.score++;
	}
}

static void
draw_ball()
{
	asm_draw(ball.x, ball.y, BALL_SIZE, BALL_SIZE, window.bg);
	if (ball.x <= window.width / 2 && ball.x + BALL_SIZE >= window.width / 2)
		asm_draw(window.width / 2, ball.y, 1, BALL_SIZE, window.fg);
	ball.x += ball.speed_x;
	ball.y += ball.speed_y;
	asm_draw(ball.x, ball.y, BALL_SIZE, BALL_SIZE, window.fg);
}
