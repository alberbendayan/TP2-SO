#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>

/*
 * Emite un sonido con cierta frecuencia durante cierto tiempo.
 * Si time es 0 se emite el sonido indefinidamente.
 * Si freq es 0 deja de emitir sonido.
 */
void sd_play(uint32_t freq, uint32_t time);

#endif
