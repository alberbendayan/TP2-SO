/*
 * Estas funciones están definidas en libasm/libasm.asm y utilizan
 * syscalls para sus respectivas llamadas.
 */

#ifndef SYSCALLS_H
#define SYSCALLS_H

#define RELEASED 0
#define PRESSED 1

#include <stdint.h>
#include <stdlib.h>

/*
 * Devuele el caracter presionado por teclado y el estado (presionado o soltado).
 */
extern uint8_t asm_getchar(uint8_t* state);

/*
 * Imprime un caracter por pantalla.
 */
extern void asm_putchar(uint8_t c, uint32_t color);

/*
 * Dibuja un rectángulo determinado por las coordenadas y sus dimensiones.
 */
extern void asm_draw(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);

/*
 * Limpia la pantalla y  posiciona el cusor en el origen.
 */
extern uint8_t asm_clear(uint32_t color);

/*
 * Posiciona el cursor en las coordenadas elegidas.
 */
extern void asm_cursor(uint32_t x, uint32_t y, uint32_t color);

/*
 * Muestra u oculta el cursor.
 */
extern void asm_show_cursor(uint8_t show);

/*
 * Guarda en los punteros las dimensiones de la ventana en pixeles y
 * en caracteres (tamaño de font).
 */
extern void asm_winprops(uint32_t* width, uint32_t* height, uint32_t* font_width, uint32_t* font_height);

/*
 * Retorna el estado del tick. Si saltó desde la última vez que se
 * preguntó devuelve true, sino false.
 */
extern uint8_t asm_ticked();

/*
 * Genera una demora de cierta cantidad de ticks.
 */
extern void asm_sleep(uint32_t ticks);

/*
 * Imprime los registros guardados al momento de realizar la snapshot.
 * La snapshot se realiza presionando Ctrl+r.
 */
extern void asm_printreg(uint32_t color);

/*
 * Imprime fecha y hora actual.
 */
extern void asm_datetime(uint32_t color);

/*
 * Emite un sonido con una cierta frecuencia durante cierto tiempo (en ticks).
 */
extern void asm_sound(uint32_t freq, uint32_t duration);

/*
 * Malloc
 */
extern void* asm_malloc(uint64_t size);

/*
 * free
 */
extern void asm_free(void* mem_block);

/*
 * Tamaño del heap
 */
extern uint64_t asm_total_heap();

/*
 * Tamaño del heap sin usar
 */
extern uint64_t asm_free_heap();

/*
 * Tamaño del heap usado
 */
extern uint64_t asm_used_heap();

/*
 * Inicializa un proceso
 */
extern uint64_t asm_init_process(process_initialization* p);

/*
 * Acaba con un proceso
 */
extern uint64_t asm_kill_process(uint16_t pid, uint32_t ret_value);

/*
 * Acaba con el proceso en ejecución
 */
extern uint64_t asm_kill_current_process(uint32_t ret_value);


/*
 * Acaba con el proceso en ejecución
 */
extern char* asm_get_snapshots_info();

extern uint64_t sys_get_current_id();

extern uint64_t asm_block_process(int pid);

extern uint64_t asm_unblock_process(int pid);

extern uint64_t asm_set_priority(int pid, int new_prio);

extern uint64_t asm_yield();

extern uint64_t asm_sem_open(uint32_t id, uint32_t init_val);

extern uint64_t asm_sem_wait(uint32_t id);

extern uint64_t asm_sem_post(uint32_t id);

extern uint64_t asm_sem_close(uint32_t id);

extern uint64_t asm_waiting_for_pid(uint32_t my_id,uint32_t id);

extern uint64_t asm_wait_pid(uint32_t my_id);

#endif
