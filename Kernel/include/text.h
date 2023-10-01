#ifndef TEXT_H
#define TEXT_H

#include <stdint.h>

/*
 * Imprime en pantalla un caracter
 */
void tx_put_char(char c, uint32_t color);

/*
 * Imprime en pantalla un string
 */
void tx_put_word(char* str, uint32_t color);

/*
 * Posiciona el cursor en las coordenadas indicadas y si está habilitado
 * renderiza el un '_' para indicar la posición.
 */
void tx_set_cursor(uint32_t x, uint32_t y, uint32_t color);

/*
 * Permite mostrar y ocultar el cursor.
 */
void tx_show_cursor(uint8_t show);

/*
 * Limpia la pantalla y posiciona el cursor en el origen (0, 0).
 */
void tx_clear(uint32_t color);

#endif
