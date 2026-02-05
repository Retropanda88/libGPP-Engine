/* 
 * libGPP-Engine - A lightweight static game engine for retro consoles.
 * Copyright (c) 2025 Andrés Ruiz Pérez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 or version 3.
 * https://www.gnu.org/licenses/
 */

#ifndef SURFACE_TOOLS_H_
#define SURFACE_TOOLS_H_

#include <SDL/SDL.h>
#include <engine/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

	/* ========================================================= Image loading 
	   & surface creation
	   ========================================================= */

/**
 * @brief Load an image from file.
 */
	SDL_Surface *load_img(const char *file);

/**
 * @brief Load a texture from a memory buffer.
 */
	SDL_Surface *load_texture_from_mem(const u8 * buffer, int len);

/**
 * @brief Create an empty SDL surface.
 */
	SDL_Surface *create_surface(int w, int h, Uint32 flags);

	/* ========================================================= Transparency
	   & alpha ========================================================= */

/**
 * @brief Apply color-key transparency to a surface.
 */
	int apply_transparency(SDL_Surface * src, u8 r, u8 g, u8 b);

/**
 * @brief Apply global alpha to a surface.
 */
	void apply_alpha(SDL_Surface * src, u8 alpha);

	/* ========================================================= Basic drawing
	   ========================================================= */

/**
 * @brief Draw a surface at a given position.
 */
	void draw_surface(SDL_Surface * src, int x, int y);

/**
 * @brief Draw a region of a surface.
 */
	void draw_surface_region(SDL_Surface * src, int sx, int sy, int sw, int sh, int dx, int dy);

/**
 * @brief Fill a rectangle area.
 */
	void fill_rect(SDL_Surface * dst, int x, int y, int w, int h, u32 color);

	/* ========================================================= Texture &
	   patterns ========================================================= */

/**
 * @brief Fill surface with alternating colors.
 */
	void fill_texture(SDL_Surface * src, int w, int h, u32 c1, u32 c2);

/**
 * @brief Fill surface with block-based alternating colors.
 */
	void fill_textureRect(SDL_Surface * src, int w, int h, u32 c1, u32 c2, int block_size);

	/* ========================================================= Gradients
	   ========================================================= */

/**
 * @brief Fill surface with a horizontal gradient.
 */
	void fill_horizontal_gradient(SDL_Surface * surface, Uint32 color1, Uint32 color2);

/**
 * @brief Fill surface with a vertical gradient.
 */
	void fill_vertical_gradient(SDL_Surface * surface, Uint32 color1, Uint32 color2);

/**
 * @brief Fill surface with a radial gradient.
 */
	void fill_radial_gradient(SDL_Surface * surface, Uint32 color1, Uint32 color2);

	/* ========================================================= Rotozoom
	   ========================================================= */

/**
 * @brief Create a rotated and scaled surface.
 */
	SDL_Surface *rotozoom_create(SDL_Surface * src, double angle, double zoom);

/**
 * @brief Set position for a rotozoom surface.
 */
	void rotozoom_set_position(SDL_Surface * src, int x, int y);

/**
 * @brief Destroy a rotozoom surface.
 */
	void rotozoom_destroy(SDL_Surface * src);

	/* ========================================================= Utilities
	   ========================================================= */

/**
 * @brief Extract RGB components from a color.
 */
	void set_color(Uint32 color, SDL_PixelFormat * format, Uint8 * r, Uint8 * g, Uint8 * b);

/**
 * @brief Cut a rectangular region from a surface.
 */
	SDL_Surface *cut_surface(SDL_Surface * src, int x, int y, int w, int h);

/**
 * @brief Set logical position for a surface.
 */
	void set_surface(SDL_Surface * s, int x, int y);

	/* ========================================================= Lines
	   ========================================================= */

/**
 * @brief Draw a fast solid line.
 */
	void draw_line_fast(SDL_Surface * dst, int x0, int y0, int x1, int y1, u32 color);

/**
 * @brief Draw a gradient line.
 */
	void draw_line_gradient(SDL_Surface * dst, int x0, int y0, int x1, int y1, u32 c0, u32 c1);

	/* ========================================================= Triangles
	   ========================================================= */

/**
 * @brief Draw triangle outline.
 */
	void draw_triangle(SDL_Surface * dst,
					   int x0, int y0, int x1, int y1, int x2, int y2, u32 color);

/**
 * @brief Fill triangle with solid color.
 */
	void fill_triangle_fast(SDL_Surface * dst,
							int x0, int y0, int x1, int y1, int x2, int y2, u32 color);

/**
 * @brief Fill triangle with per-vertex color gradient.
 */
	void fill_triangle_gradient(SDL_Surface * dst,
								int x0, int y0, int r0, int g0, int b0,
								int x1, int y1, int r1, int g1, int b1,
								int x2, int y2, int r2, int g2, int b2);
								
								
	
void draw_line(int x0, int y0, int x1, int y1, u32 color);

#ifdef __cplusplus
}
#endif

#endif							/* SURFACE_TOOLS_H_ */
