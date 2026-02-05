/* 
   ============================================================================
   libGPP-Engine Test Suite - Main Entry Point Author: Andrés Ruiz Pérez
   Description: Executes all rendering stress tests for libGPP-Engine. Designed
   for embedded platforms with 320x240 resolution.

   Tests included: - Test 1: Random texture allocation + gradients - Test 2:
   Random rectangle gradient textures - Test 3: Alpha blending / fade test -
   Test 4: Horizontal gradient surfaces - Test 5: Vertical gradient surfaces -
   Test 6: Radial gradient surfaces

   Supported Platforms: - Android, PSP, PS2, GameCube
   ============================================================================ */

#include <engine/engine.h>
#include <video/pixel.h>

void pixel_test(SDL_Surface *s)
{
    int x, y;

    if (SDL_MUSTLOCK(s))
        SDL_LockSurface(s);

    /* Borde superior e inferior */
    for (x = 0; x < 320; x++)
    {
        pixel(s, x, 0,color_rgb(255,255,255));
        pixel(s, x, 239, color_rgb(255,255,255));
    }

    /* Borde izquierdo y derecho */
    for (y = 0; y < 240; y++)
    {
        pixel(s, 0,   y, color_rgb(255, 0, 0));
        pixel(s, 319, y, color_rgb(255, 0, 0));
    }

    /* Diagonal */
    for (x = 0; x < 240; x++)
        pixel(s, x, x, color_rgb(0,255,0));

    /* Puntos de color */
    pixel(s, 160, 120, color_rgb(255,0,0)); // rojo
    pixel(s, 100, 100, color_rgb(0,0,255)); // azul
    pixel(s, 200, 140, color_rgb(0,255,255)); // amarillo

    if (SDL_MUSTLOCK(s))
        SDL_UnlockSurface(s);
}

int main(int argc, char **argv)
{
	// Initialize engine core
	if (Init_Sistem("libGPP-Engine Test Suite v01") != 0)
		return 1;

	// Initialize video mode
	if (Set_Video() != 0)
		return 1;

  
   pixel_test(logic);
   print(10,10,"hello este es un test de font ", color_rgb(255,0,0));
   Render();
   
   while(1);
   
   
	off_video();
	shoutdown_sistem();

	// End program gracefully
	return 0;
}
