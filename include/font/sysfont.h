/*
 * libGPP-Engine - A lightweight static game engine for retro consoles.
 * Copyright (c) 2025 Andrés Ruiz Pérez
 *
 * sysfont - Motor de renderizado de texto integrado basado en matriz de caracteres.
 */

#ifndef GPP_SYSFONT_H_
#define GPP_SYSFONT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void sysfontDisplayChar(int x, int y, uint8_t c, unsigned int color);
void sysfontDrawString(int x, int y, const char *string, unsigned int color);
void sysfontPrintF(int x, int y, unsigned int color, const char *str, ...);
void sysfontDrawRect(int x, int y, int w, int h, unsigned int color);

#ifdef __cplusplus
}
#endif

#endif /* GPP_SYSFONT_H_ */
