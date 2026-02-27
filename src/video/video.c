/* 
 * libGPP-Engine - A lightweight static game engine for retro consoles.
 * Copyright (c) 2025 Andrés Ruiz Pérez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 or version 3.
 * https://www.gnu.org/licenses/
 */

#include <stdio.h>
#include <SDL/SDL.h>
#include <engine/types.h>
#include <video/video.h>
#include <string.h>
#include <font/font.h>

#ifdef PSP_BUILD
#include <engine/psp_sdk.h>
#endif

#ifdef PS2_BUILD
#include <kernel.h>
#endif

// framebuffer pointer
SDL_Surface *vram = NULL;
SDL_Surface *logic = NULL;

#if defined(PSP_BUILD)
u16 *fb = NULL;
#else
u32 *fb = NULL;
#endif

static u16 x_table[480];
static u16 y_table[272];
static int scale_init = 0;

void Scale_Init(void)
{
	int i;

	if (scale_init)
		return;

	for (i = 0; i < 480; i++)
		x_table[i] = (i * 320) / 480;

	for (i = 0; i < 272; i++)
		y_table[i] = (i * 240) / 272;

	scale_init = 1;
}

/**
 * @brief Initializes the system with an optional message.
 *
 * This function performs the system initialization.
 * It takes a `msg` parameter (const char*) that may contain additional
 * information for the initialization process (for example, a configuration
 * code or a flag for a specific initialization mode).
 *
 * The exact behavior depends on how the message is used.
 *
 * @param msg Pointer to a char string that can contain relevant initialization
 *            information. It may be NULL.
 *
 * @return 0 if the initialization was successful, or a non-zero error code if it failed.
 *
 * @note This function may require system resources such as memory or previous
 *       configurations. Ensure all prerequisites are met before calling.
 */
int Init_Sistem(const char *msg)
{
	// Inicialización específica para PSP
#ifdef PSP_BUILD
	PSP_SetupCallbacks();
#endif

	// Inicialización específica para PS2 en audio
#ifdef PS2_BUILD
	int main_id = GetThreadId();
	ChangeThreadPriority(main_id, 72);
	/* inicializar FS primero */
	//fs_init();
#endif

	// Inicializa SDL con video y temporizador
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		printf("error: %s\n", SDL_GetError());
		return -1;
	}

	// Imprime mensaje solo si no es NULL
	if (msg)
		printf("\n%s\n", msg);

	// Tamaño de fuente por defecto
	fontsize(8, 8);

#ifdef PS2_BUILD

	/* inicializar FS primero */
	fs_init();
#endif

	return 0;
}

/**
 * @brief Sets the video resolution.
 *
 * This function configures the video mode of the system, setting the
 * window/screen width and height in pixels.
 *
 * @param width  Screen width in pixels.
 * @param height Screen height in pixels.
 *
 * @return 0 if successful, or -1 on error.
 *
 * @note Ensure width and height are supported by the platform.
 */
int Set_Video(void)
{
#if defined(PSP_BUILD)

	/* Framebuffer REAL del PSP (480x272 RGB565) */
	vram = SDL_SetVideoMode(480, 272, 16, SDL_SWSURFACE);

	Scale_Init();				// inicializa tablas del escalador

#else

	/* Otras plataformas usan framebuffer lógico directo */
	vram = SDL_SetVideoMode(320, 240, 32, SDL_SWSURFACE);

#endif

	if (!vram)
	{
		printf("SetVideoMode error: %s\n", SDL_GetError());
		return -1;
	}

	SDL_ShowCursor(SDL_FALSE);

	/* ---------- Surface lógica SIEMPRE 320x240 ---------- */

#if defined(PSP_BUILD)

	logic = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 240, 16,	// RGB565
								 vram->format->Rmask, vram->format->Gmask, vram->format->Bmask, 0);

	fb = (u16 *) logic->pixels;	// CORRECTO para PSP

#else

	logic = SDL_CreateRGBSurface(SDL_SWSURFACE,
								 320, 240,
								 32,
								 vram->format->Rmask,
								 vram->format->Gmask, vram->format->Bmask, vram->format->Amask);

	fb = (u32 *) logic->pixels;	// CORRECTO para PS2/GC/Android

#endif

	if (!logic)
	{
		printf("Logic surface error\n");
		return -1;
	}

	return 0;
}


void Scale_320x240_to_480x272(SDL_Surface * src, SDL_Surface * dst)
{
	int x, y;

	u16 *src_pixels = (u16 *) src->pixels;
	u16 *dst_pixels = (u16 *) dst->pixels;

	int src_pitch = src->pitch >> 1;	// 16bpp
	int dst_pitch = dst->pitch >> 1;

	for (y = 0; y < 272; y++)
	{
		u16 *src_row = src_pixels + y_table[y] * src_pitch;
		u16 *dst_row = dst_pixels + y * dst_pitch;

		for (x = 0; x < 480; x++)
		{
			dst_row[x] = src_row[x_table[x]];
		}
	}
}


/**
 * @brief Gets the current video resolution.
 *
 * @param width  Pointer where the screen width will be stored.
 * @param height Pointer where the screen height will be stored.
 */
void get_wh_video_mode(int *width, int *height)
{

	if (!width || !height)
		return;

	if (!logic)
	{
		printf("not set video mode\n");
		*width = 0;
		*height = 0;
		return;
	}

	*width = logic->w;
	*height = logic->h;
}

/**
 * @brief Safely shuts down the system.
 *
 * Closes all active SDL subsystems and frees resources.
 */
void shoutdown_sistem()
{
#ifdef PSP_BUILD
	// On PSP, SDL_Quit is not enough to exit the application
	PSP_Exit();
#else
	// On other platforms, SDL cleanup is sufficient
	SDL_Quit();
#endif
}

/**
 * @brief Turns off the video mode.
 *
 * Frees the video surface and releases video-related memory.
 */
void off_video()
{
	if (vram)
	{
		SDL_FreeSurface(vram);
		vram = NULL;
	}
	if (logic)
	{
		SDL_FreeSurface(logic);
		logic = NULL;
	}
	fb = NULL;
}

/**
 * @brief Renders the framebuffer to the screen.
 *
 * Updates the display, usually called every game loop frame.
 */
void Render(void)
{
#if defined(PSP_BUILD)

	// Escalado software 320x240 -> 480x272
	Scale_320x240_to_480x272(logic, vram);

#else

	// Copia directa (misma resolución)
	SDL_BlitSurface(logic, NULL, vram, NULL);

#endif

	SDL_Flip(vram);
}

/**
 * @brief Clears the screen (fills it with black).
 *
 * Writes 0x00000000 to every pixel in the framebuffer.
 */
void cls()
{
	Uint8 *fb = (Uint8 *) logic->pixels;
	int szScreen = logic->pitch * logic->h;

	memset(fb, 0, szScreen);
}

/**
 * @brief Clears the screen with a specific RGB color.
 *
 * @param r Red component (0–255)
 * @param g Green component (0–255)
 * @param b Blue component (0–255)
 */
void cls_rgb(u8 r, u8 g, u8 b)
{
	if (!logic)
		return;
	SDL_FillRect(logic, 0, SDL_MapRGB(logic->format, r, g, b));
}

/**
 * @brief Creates a 32-bit ARGB color from RGB components.
 *
 * Alpha is always 255 (opaque).
 *
 * @return Pixel value in ARGB32 format.
 */
u32 color_rgb(u8 r, u8 g, u8 b)
{
	if (!logic)
		return;
	return SDL_MapRGB(logic->format, r, g, b);
}

/**
 * @brief Synchronizes rendering to a constant FPS.
 *
 * @param frecuencia Frame interval in milliseconds (1000/FPS)
 */
/* void Fps_sincronizar(int frecuencia) { static int t; static int temp;
   static int t1 = 0;

   t = SDL_GetTicks(); if (t - t1 >= frecuencia) { temp = (t - t1) /
   frecuencia; t1 += temp * frecuencia; } else { SDL_Delay(frecuencia - (t -
   t1)); t1 += frecuencia; } } */

void Fps_sincronizar(u32 frame_ms)
{
	static u32 last_time = 0;
	u32 current_time = SDL_GetTicks();
	u32 elapsed = current_time - last_time;

	if (elapsed < frame_ms)
	{
		SDL_Delay(frame_ms - elapsed);
		last_time += frame_ms;
	}
	else
	{
		// Frame lento: saltamos para evitar acumulación
		last_time = current_time;
	}
}
