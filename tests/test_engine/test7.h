#ifndef TEST7_H_
#define TEST7_H_

#include <engine/engine.h>

/*
 ============================================================================
  Test 7 - Framebuffer / Pixel Noise Stress Test
  Author: Andrés Ruiz Pérez

  NOTE:
  - On PSP / PS2 / GameCube this behaves like real framebuffer access.
  - On Android this uses an SDL software surface (emulated framebuffer).
 ============================================================================
*/

void TEST7()
{
    int count = 0;
    const int max = 1000;

    u8 r = 0, g = 255, b = 0;

    print(40, 100, "Test 7: Framebuffer Noise Test", color_rgb(r, g, b));
    Render();
    SDL_Delay(2000);
    cls();

    SDL_Surface *s = logic;
    if (!s)
        return;

    int width  = s->w;
    int height = s->h;

    while (count < max)
    {

#if defined(ANDROID_BUILD)
        /* ================= ANDROID / SAFE SDL ================= */

        SDL_LockSurface(s);

        Uint32 *fb = (Uint32 *)s->pixels;
        int pitch = s->pitch >> 2; // /4

        for (int y = 0; y < height; y++)
        {
            Uint32 *row = fb + y * pitch;
            for (int x = 0; x < width; x++)
            {
                row[x] = SDL_MapRGB(
                    s->format,
                    rand() & 255,
                    rand() & 255,
                    rand() & 255
                );
            }
        }

        SDL_UnlockSurface(s);

#elif defined(PSP_BUILD)
        /* ================= PSP (RGB565) ================= */

        Uint16 *fb = (Uint16 *)s->pixels;
        int pitch = s->pitch >> 1; // /2

        for (int y = 0; y < height; y++)
        {
            Uint16 *row = fb + y * pitch;
            for (int x = 0; x < width; x++)
            {
                int rr = rand() & 255;
                int gg = rand() & 255;
                int bb = rand() & 255;

                row[x] =
                    ((rr >> 3) << 11) |
                    ((gg >> 2) << 5)  |
                    ((bb >> 3));
            }
        }

#else
        /* ================= 32 BPP (PC / PS2 / GC) ================= */

        Uint32 *fb = (Uint32 *)s->pixels;
        int pitch = s->pitch >> 2; // /4
        Uint32 a = s->format->Amask;

        for (int y = 0; y < height; y++)
        {
            Uint32 *row = fb + y * pitch;
            for (int x = 0; x < width; x++)
            {
                int rr = rand() & 255;
                int gg = rand() & 255;
                int bb = rand() & 255;

                row[x] =
                    a |
                    (rr << s->format->Rshift) |
                    (gg << s->format->Gshift) |
                    (bb << s->format->Bshift);
            }
        }
#endif

        Render();
        count++;
    }

    cls();
    print(15, 100, "Render Test 7 Completed Successfully",
          color_rgb(0, 255, 0));
    Render();
    SDL_Delay(2000);
    cls();
}

#endif