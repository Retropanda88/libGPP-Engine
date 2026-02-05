/*
 * libGPP-Engine - A lightweight static game engine for retro consoles.
 * Copyright (c) 2025 Andrés Ruiz Pérez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 or version 3.
 * https://www.gnu.org/licenses/
 */

#ifndef VIDEO_H_
#define VIDEO_H_

#include <SDL/SDL.h>
#include <../include/engine/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Pointer to the video surface.
 *
 * This external variable points to the main video surface used for rendering graphics.
 * Depending on the platform, this surface may represent the video memory used to
 * display graphics on the screen.
 *
 * @note This variable must be initialized before use and must point to a valid surface
 *       to avoid undefined behavior.
 */
extern SDL_Surface *vram;
extern SDL_Surface *logic;

/**
 * @brief Pointer to the framebuffer.
 *
 * This external variable is a pointer to a 32-bit array representing the framebuffer.
 * Each 32-bit value corresponds to a pixel on the screen. The color format depends
 * on the current screen configuration.
 *
 * @note Ensure this pointer is correctly assigned to a valid memory area before use.
 *       It is commonly used for direct pixel manipulation.
 */
#if defined(PSP_BUILD)
extern u16 *fb ;
#else
extern u32 *fb;
#endif

/**
 * @brief Initializes the system with an optional message.
 *
 * This function performs system initialization. It receives a `msg` parameter that may
 * provide configuration information, status messages, or initialization flags.
 *
 * @param msg Optional message or configuration string. Can be NULL.
 * @return 0 on success, any non-zero value on failure.
 */
int Init_Sistem(const char *msg);

/**
 * @brief Sets the video resolution.
 *
 * This function configures the video mode using `width` and `height` in pixels.
 *
 * @param width Screen width in pixels.
 * @param height Screen height in pixels.
 * @return 0 on success, non-zero value on error.
 */
int Set_Video(void);

/**
 * @brief Retrieves the current video resolution.
 *
 * Stores the width and height of the active video mode.
 *
 * @param width Pointer to store screen width.
 * @param height Pointer to store screen height.
 */
void get_wh_video_mode(int *width, int *height);

/**
 * @brief Shuts down the system safely.
 *
 * Closes active processes and frees allocated resources.
 *
 * @note Always call this before exiting the program to avoid data loss.
 */
void shoutdown_sistem();

/**
 * @brief Turns off the video system.
 *
 * Releases video resources such as allocated memory or screen settings.
 */
void off_video();

/**
 * @brief Renders all graphics to the screen.
 *
 * Usually called every frame to update the display contents.
 */
void Render();

/**
 * @brief Clears the screen to black.
 *
 * Removes all previous graphics from the screen.
 */
void cls();

/**
 * @brief Clears the screen with a custom RGB color.
 *
 * @param r Red component (0–255)
 * @param g Green component (0–255)
 * @param b Blue component (0–255)
 */
void cls_rgb(u8 r, u8 g, u8 b);

/**
 * @brief Builds an ARGB color from RGB components.
 *
 * Alpha channel is set to 255 (fully opaque).
 *
 * @return 32-bit ARGB color.
 */
u32 color_rgb(u8 r, u8 g, u8 b);

/**
 * @brief Synchronizes the framerate to the desired frequency.
 *
 * @param frecuencia Target FPS.
 */
//void Fps_sincronizar(int frecuencia);
void Fps_sincronizar(u32 frame_ms);

#ifdef __cplusplus
}
#endif

#endif
