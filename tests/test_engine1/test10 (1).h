#ifndef TEST10_H_
#define TEST10_H_

#include <engine/engine.h>
#include <engine/gfxFont.h>

/*
 ============================================================================
  Test 10 - Font Rendering Stress Test
  Author: Andrés Ruiz Pérez

  Description:
      Stress test focused on font rendering performance.
      Evaluates text drawing speed, alignment methods and
      font switching under continuous redraw.

      This test simulates heavy UI and RPG dialogue usage.

      Supported Platforms:
          - Android
          - PSP
          - PS2
          - GameCube
 ============================================================================
*/

#define MAX_LINES 8

void TEST10()
{
    u8 r = 0, g = 255, b = 0;

    print(30, 100, "Starting Render Test 10:",
          color_rgb(r, g, b));
    print(30, 112, "Font Stress Test",
          color_rgb(r, g, b));
    Render();
    SDL_Delay(2000);
    cls();

    int sw, sh;
    get_wh_video_mode(&sw, &sh);

    gfxFont font;
    if (!font.init())
    {
        print(10, 100, "Error: Failed to initialize fonts",
              color_rgb(255, 0, 0));
        Render();
        SDL_Delay(2000);
        cls();
        return;
    }

    /* Main test loop */
    for (int frame = 0; frame < 200; frame++)
    {
        cls();

        for (int i = 0; i < MAX_LINES; i++)
        {
            int y = 5 + i * font.getHeight();

            if (y + font.getHeight() > sh)
                break;

            font.drawf(
                logic,
                SMALL_FONT,
                10,
                y,
                "Frame %d - Line %d",
                frame,
                i
            );

            /* Right aligned to screen width */
            font.drawRightJustified(
                logic,
                MMX_FONT,
                sw - 10,
                y,
                "RPG UI TEST"
            );
        }

        /* Centered titles */
        font.drawCentered(
            logic,
            LARGE_FONT,
            sw >> 1,
            sh - 32,
            "LIBGPP-ENGINE"
        );

        font.drawCentered(
            logic,
            LARGE_FONT,
            sw >> 1,
            sh - 56,
            "-*Andres*-"
        );

        Render();
        //Fps_sincronizar(60);
    }

    cls();
    print(10, 100, "Render Test 10 Completed Successfully",
          color_rgb(0, 255, 0));
    Render();
    SDL_Delay(2000);
    cls();
}


#endif
