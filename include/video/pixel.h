/*
 * libGPP-Engine - A lightweight static game engine for retro consoles.
 * Copyright (c) 2025 Andrés Ruiz Pérez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 or version 3.
 * https://www.gnu.org/licenses/
 */

#ifndef PIXEL_H_
#define PIXEL_H_


#include <SDL/SDL.h>
#include <engine/types.h>

#ifdef __cplusplus

extern "C" {

#endif


/**
 * @brief Dibuja un píxel en una superficie.
 *
 * Esta función establece el valor de un píxel en una posición `(x, y)` en la superficie `src` con un color dado `c`.
 * El color se pasa como un valor de 32 bits, donde los componentes de color se organizan de acuerdo al formato de la superficie.
 *
 * @param src Puntero a la superficie en la que se dibujará el píxel.
 * @param x Coordenada X del píxel a dibujar.
 * @param y Coordenada Y del píxel a dibujar.
 * @param c Color del píxel a dibujar (valor de 32 bits en formato de la superficie).
 *
 * @note La función no realiza ninguna comprobación de límites de los valores `x` y `y`. Si las coordenadas están fuera de los límites de la superficie, el comportamiento será indefinido.
 */
void pixel(SDL_Surface *src, int x, int y, u32 c);

/**
 * @brief Obtiene el color de un píxel en una superficie.
 *
 * Esta función obtiene el valor del color de un píxel en una posición `(x, y)` en la superficie `src`.
 * El valor devuelto es un color de 32 bits que representa el píxel en el formato de la superficie.
 *
 * @param src Puntero a la superficie de la que se leerá el píxel.
 * @param x Coordenada X del píxel a obtener.
 * @param y Coordenada Y del píxel a obtener.
 *
 * @return El color del píxel en formato de 32 bits. Si las coordenadas están fuera de los límites de la superficie, se devuelve 0.
 *
 * @note La función no realiza ninguna comprobación de límites de los valores `x` y `y`. Si las coordenadas están fuera de los límites de la superficie, se devolverá 0 como valor predeterminado.
 */
Uint32 get_pixel(SDL_Surface *src, int x, int y);





#ifdef __cplusplus
}
#endif

#endif