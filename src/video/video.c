/* * libGPP-Engine - A lightweight static game engine for retro consoles.
 * Copyright (c) 2025 Andrés Ruiz Pérez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 or version 3.
 * https://www.gnu.org/licenses/
 * */

#include <stdio.h>
#include <SDL/SDL.h>
#include <engine/types.h>
#include <video/video.h>
#include <string.h>
#include <font/font.h>

#include <engine/surface_tools.h>
#include <engine/startup_png.h>

#ifdef PSP_BUILD
#include <engine/psp_sdk.h>
#endif

#ifdef PS2_BUILD
#include <kernel.h>
/* Declaración externa para evitar el warning de fs_init */
extern int fs_init(void);
#endif

// framebuffer pointer
SDL_Surface *vram = NULL;
SDL_Surface *logic = NULL;

/* ⚡ Usamos void* aquí para evitar conflictos con la cabecera video.h 
   y que ee-gcc compile sin importar si hereda u16 o u32 de forma externa. */
void *fb = NULL;

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
 */
int Init_Sistem(const char *msg)
{
	// Inicialización específica para PSP
#ifdef PSP_BUILD
	PSP_SetupCallbacks();
#endif

#ifdef PS2_BUILD
	printf("change thread priority for audio\n");
	int main_id = GetThreadId();
	ChangeThreadPriority(main_id, 72);
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
   pasamos argumentos de ancho y alto 
 */
int Set_Video(const int w, const int h)
{
#if defined(PSP_BUILD)
	/* Framebuffer REAL del PSP (480x272 RGB565) */
	vram = SDL_SetVideoMode(480, 272, 16, SDL_SWSURFACE);
	Scale_Init();				// inicializa tablas del escalador
#else
	/* Otras plataformas usan framebuffer lógico directo (PS2 hereda 16 bits aquí) */
	vram = SDL_SetVideoMode(w, h, 16, SDL_SWSURFACE);
#endif

	if (!vram)
	{
		printf("SetVideoMode error: %s\n", SDL_GetError());
		return -1;
	}

	SDL_ShowCursor(SDL_FALSE);

	/* ---------- Surface lógica ---------- */

#if defined(PSP_BUILD) || defined(PS2_BUILD)
	/* ⚡ OPTIMIZACIÓN: PS2 y PSP usan 16 bits directos sin conversión por software */
	logic = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 16,	// RGB565 nativo
								 vram->format->Rmask, vram->format->Gmask, vram->format->Bmask, 0);
#else
	/* Las demás plataformas (Android, GC, PC) conservan sus 32 bits originales */
	logic = SDL_CreateRGBSurface(SDL_SWSURFACE,
								 w, h,
								 16,
								 vram->format->Rmask,
								 vram->format->Gmask, vram->format->Bmask, vram->format->Amask);
#endif

	if (!logic)
	{
		printf("Logic surface error\n");
		return -1;
	}

	// Asignamos el puntero de pixeles al fb genérico
	fb = logic->pixels;

	return 0;
}

void startup()
{
	SDL_Surface *temp = NULL;

	// cargar imagen desde memoria
	temp = load_texture_from_mem((u8*)startup_data, startup_size);

	if (!temp)
		return;

	// ==========================
	// FADE IN
	// ==========================
	int alpha;
	for (alpha = 0; alpha <= 255; alpha += 5)
	{
		SDL_SetAlpha(temp, SDL_SRCALPHA, alpha);

		cls();
		draw_surface(temp, 0, 0);
		Render();
		Fps_sincronizar(10);
	}

	// pequeña pausa
	int i;
	for (i = 0; i < 40; i++)
	{
		cls();
		draw_surface(temp, 0, 0);
		Render();
		Fps_sincronizar(60);
	}

	// ==========================
	// FADE OUT
	// ==========================
	for (alpha = 255; alpha >= 0; alpha -= 5)
	{
		SDL_SetAlpha(temp, SDL_SRCALPHA, alpha);

		cls();
		draw_surface(temp, 0, 0);
		Render();
		Fps_sincronizar(10);
	}

	SDL_FreeSurface(temp);
	SDL_Delay(1000);
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
 */
void Render(void)
{
#if defined(PSP_BUILD)
	// Escalado software 320x240 -> 480x272 exclusivo de PSP
	Scale_320x240_to_480x272(logic, vram);
#else
	// Copia directa limpia (En PS2 ahora es instantáneo al ser 16-bits contra 16-bits)
	SDL_BlitSurface(logic, NULL, vram, NULL);
#endif

	SDL_Flip(vram);
}

/**
 * @brief Clears the screen (fills it with black).
 */
void cls()
{
	Uint8 *pixels = (Uint8 *) logic->pixels;
	int szScreen = logic->pitch * logic->h;

	memset(pixels, 0, szScreen);
}

/**
 * @brief Clears the screen with a specific RGB color.
 */
void cls_rgb(u8 r, u8 g, u8 b)
{
	if (!logic)
		return;
	SDL_FillRect(logic, 0, SDL_MapRGB(logic->format, r, g, b));
}

/**
 * @brief Creates a color from RGB components.
 */
u32 color_rgb(u8 r, u8 g, u8 b)
{
	if (!logic)
		return 0;
	return SDL_MapRGB(logic->format, r, g, b);
}

/**
 * @brief Synchronizes rendering to a constant FPS.
 */
void Fps_sincronizar(u32 frame_ms)
{
	static u32 last_time = 0;
	
	if (last_time == 0) {
		last_time = SDL_GetTicks();
		return;
	}

	while ((SDL_GetTicks() - last_time) < frame_ms) 
	{
		SDL_Delay(1); 
	}

	last_time += frame_ms;

	if ((SDL_GetTicks() - last_time) > frame_ms) {
		last_time = SDL_GetTicks();
	}
}