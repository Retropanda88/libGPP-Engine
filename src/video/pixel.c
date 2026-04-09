/*
 * libGPP-Engine - A lightweight static game engine for retro consoles.
 * Copyright (c) 2025 Andrés Ruiz Pérez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 or version 3.
 * https://www.gnu.org/licenses/
 */


#include <video/pixel.h>
#include <engine/types.h>

void pixel(SDL_Surface *src, int x, int y, u32 c)
{
    if (!src || x < 0 || y < 0 || x >= src->w || y >= src->h)
        return;

    Uint8 *row = (Uint8 *)src->pixels + y * src->pitch;

#if defined(PSP_BUILD)
    ((Uint16 *)row)[x] = (Uint16)c;   // RGB565
#else
    ((Uint32 *)row)[x] = c;           // ARGB8888
#endif
}


u32 get_pixel(SDL_Surface *src, int x, int y)
{
    if (!src || x < 0 || y < 0 || x >= src->w || y >= src->h)
        return 0;

    Uint8 *row = (Uint8 *)src->pixels + y * src->pitch;

#if defined(PSP_BUILD)
    return ((Uint16 *)row)[x];
#else
    return ((Uint32 *)row)[x];
#endif
}


