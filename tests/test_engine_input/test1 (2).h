/* 
   ============================================================================
   Test 1 - Render Stress Test Author: Andrés Ruiz Pérez Description:
   Performance test for libGPP-Engine on embedded platforms. This test exercises 
   the renderer under heavy load by: - Creating surfaces with random dimensions
   - Generating random vertical gradients - Rendering at random screen positions
   - Synchronizing frame rate Useful as a fast stability and performance
   validation test.

   Supported platforms: - Android - PSP - PS2 - GameCube
   ============================================================================ */

#ifndef TEST1_H_
#define TEST1_H_

#include <engine/engine.h>

// Test 1 //////////////////////////////////////////////////
void TEST1()
{
    u8 r = 0, g = 255, b = 0;
    const int n_samples = 1000;

    int width, height;
    get_wh_video_mode(&width, &height);

    SDL_Surface *s = create_surface(width, height, SDL_SWSURFACE);
    if (!s)
        return;

    SDL_Rect src, dst;

    Uint32 start_time = SDL_GetTicks();
    Uint32 last_time  = start_time;
    Uint32 frame_count = 0;
    float fps = 0.0f;

    for (int i = 0; i < n_samples; i++)
    {
        Uint32 frame_start = SDL_GetTicks();

        int ws = (rand() % (width  - 1)) + 1;
        int hs = (rand() % (height - 1)) + 1;

        int x = rand() % (width  - ws);
        int y = rand() % (height - hs);

        fill_texture(
            s, ws, hs,
            color_rgb(rand() & 255, rand() & 255, rand() & 255),
            color_rgb(rand() & 255, rand() & 255, rand() & 255)
        );

        src.x = src.y = 0;
        src.w = ws;
        src.h = hs;

        dst.x = x;
        dst.y = y;
        dst.w = ws;
        dst.h = hs;

        SDL_BlitSurface(s, &src, logic, &dst);

        /* FPS realtime */
        frame_count++;
        Uint32 now = SDL_GetTicks();
        if (now - last_time >= 1000)
        {
            fps = (frame_count * 1000.0f) / (now - last_time);
            frame_count = 0;
            last_time = now;
        }

        Uint32 draw_time = SDL_GetTicks() - frame_start;

        /* ===== Overlay azul ===== */
        fill_rect(logic, 0, 0, 320, 15, color_rgb(0, 0, 255));
        print_f(2, 2, color_rgb(255,255,255),
                "TEST1 | FPS: %.2f | Draw: %u ms | Frame: %d",
                fps, draw_time, i + 1);

        Render();
    }

    Uint32 elapsed_ms = SDL_GetTicks() - start_time;

    SDL_FreeSurface(s);
    cls();
}

#endif

/*
= ===========================================================================
NOTA IMPORTANTE SOBRE MEMORIA Y SUPERFICIES (CONSOLAS)

Este test reutiliza UNA sola superficie para evitar fragmentación de memoria.
En plataformas embebidas como PSP, PS2 y GameCube:

- Crear y destruir muchas superficies dinámicamente (alloc/free por frame)
  puede provocar:
    * Fragmentación de memoria
    * Fallos de creación de superficies
    * Cuelgues o falta de memoria (OOM)

- SDL_HWSURFACE dinámico es especialmente peligroso, ya que suele usar
  VRAM o memoria alineada que NO se compacta correctamente.

RECOMENDACIÓN:
- Reutilizar superficies persistentes
- Usar pools de superficies si se requieren múltiples buffers
- Evitar tamaños variables creados en tiempo de ejecución

Este comportamiento NO es un bug del motor, sino una limitación del
hardware y de los backends SDL en consolas.
============================================================================
*/
