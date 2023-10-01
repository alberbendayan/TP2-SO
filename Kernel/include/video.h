#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>

/*
 * Devuelve el ancho de la pantalla.
 */
uint16_t vd_get_winwidth();

/*
 * Devuelve el largo de la pantalla.
 */
uint16_t vd_get_winheight();

/*
 * Posiciona en memoria el código de color (RRGGBB) en las coordenadas
 * indicadas. "Dibuja un pixel en pantalla".
 */
void vd_put_pixel(uint32_t x, uint32_t y, uint32_t color);

/*
 * Renderiza un caracter en forma de bitmap en pantalla en las coordenadas indicadas.
 */
void vd_put_char(char c, uint32_t x, uint32_t y, uint32_t color);

/*
 * Dibuja un caracter que representa la forma de un cursor '_'.
 */
void vd_draw_cursor(uint32_t x, uint32_t y, uint32_t color);

/*
 * Dibuja un área concentrada por las coordenadas y sus dimensiones en un cierto color.
 */
void vd_draw(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);

/*
 * Mueve todas las líneas de texto para arriba una línea y limpia la última.
 */
void vd_scroll_up();

/*
 * Limpia la pantalla con el color indicado.
 */
void vd_clear(uint32_t bg);

/*
 * Imprime el logo del kernel y un mensaje de "Loading..."
 */
void vd_wallpaper(uint32_t size);

#endif
