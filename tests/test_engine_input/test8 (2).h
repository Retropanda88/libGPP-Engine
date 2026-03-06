#ifndef TEST8_H_
#define TEST8_H_

#include <engine/engine.h>
#include "img_jpeg.h"

/*
 ============================================================================
  Test 8 - RotoZoom Stress Test
  Author: Andrés Ruiz Pérez

  Description:
      High-level rendering stress test for libGPP-Engine.
      This test evaluates the performance and stability of the
      rotozoom pipeline under dynamic transformations.

      Features:
          - Image loading from memory
          - Continuous rotation and scaling
          - Randomized transformation bursts
          - Stress on CPU, memory allocation, and pixel processing

      Supported Platforms:
          - Android
          - PSP
          - PS2
          - GameCube
 ============================================================================
*/

void TEST8()
{
    u8 r = 0, g = 255, b = 0;

    print(0, 100, "Starting Render Test 8: RotoZoom Stress Test",
          color_rgb(r, g, b));
    Render();
    SDL_Delay(2000);
    cls();

    int sw, sh;
    get_wh_video_mode(&sw, &sh);

    SDL_Surface *img = load_texture_from_mem((u8 *)img_data, img_size);
    if (!img)
    {
        print(10, 100, "Error: Failed to load image from memory",
              color_rgb(255, 0, 0));
        Render();
        SDL_Delay(2000);
        cls();
        return;
    }

    /* Preview inicial */
    draw_surface(img, 10, 10);
    Render();
    SDL_Delay(2000);
    cls();

    /* ================= Random rotozoom burst ================= */
    for (int i = 0; i < 500; i++)
    {
        double angle = rand() % 360;
        double zoom  = ((rand() % 90) + 10) / 100.0;

        int x = rand() % sw;
        int y = rand() % sh;

        SDL_Surface *temp = rotozoom_create(img, angle, zoom);
        if (temp)
        {
            /* SOLO clamp de seguridad */
            if (x + temp->w > sw) x = sw - temp->w;
            if (y + temp->h > sh) y = sh - temp->h;
            if (x < 0) x = 0;
            if (y < 0) y = 0;

            rotozoom_set_position(temp, x, y);
            Render();
            rotozoom_destroy(temp);
        }

        cls();
    }

    /* ================= Animated rotozoom loop ================= */
    float angle = 0.0f;
    float zoom  = 0.5f;
    float d_angle = 0.5f;
    float d_zoom  = 0.01f;

    for (int i = 0; i < 600; i++)
    {
        SDL_Surface *temp = rotozoom_create(img, angle, zoom);
        if (temp)
        {
            /* EXACTAMENTE como lo tenías */
            int x = 120;
            int y = 100;


            rotozoom_set_position(temp, x, y);
            Render();
            SDL_FreeSurface(temp);
        }

        angle += d_angle;
        zoom  += d_zoom;

        if (zoom > 1.2f || zoom < 0.2f)
            d_zoom = -d_zoom;

        cls();
    }

    rotozoom_destroy(img);

    cls();
    print(10, 100, "Render Test 8 Completed Successfully",
          color_rgb(r, g, b));
    Render();
    SDL_Delay(2000);
    cls();
}


#endif