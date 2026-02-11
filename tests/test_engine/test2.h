/* 
   ============================================================================
   Test 2 - Rectangle Pattern Stress Test Author: Andrés Ruiz Pérez
   Description: Performance test for libGPP-Engine on embedded platforms. This
   test stresses the renderer by: - Creating surfaces with random dimensions -
   Generating rectangle-pattern textures with random colors - Using random
   rectangle sizes - Rendering at random screen positions - Synchronizing frame
   rate

   Designed to validate rendering stability and performance.

   Supported platforms: - Android - PSP - PS2 - GameCube
   ============================================================================

   ============================================================================
   NOTA IMPORTANTE SOBRE MEMORIA Y SUPERFICIES (CONSOLAS)

   Aunque este test crea superficies dinámicamente, en plataformas embebidas
   como PSP, PS2 y GameCube la creación y destrucción repetida de superficies
   con tamaños variables puede provocar fragmentación de memoria y fallos
   de asignación.

   Este comportamiento no se considera un bug del motor, sino una limitación
   del hardware y de los backends SDL en consolas.

   Para pruebas de rendimiento estables se recomienda:
   - Reutilizar superficies persistentes
   - Usar pools de superficies
   - Evitar alloc/free por frame

   ============================================================================
*/

#ifndef TEST2_H_
#define TEST2_H_

#include <engine/engine.h>

void TEST2()
{
    u8 r = 0, g = 255, b = 0;
    const int n_samples = 1000;

    int width, height;
    get_wh_video_mode(&width, &height);

    SDL_Surface *s = create_surface(width, height, SDL_SWSURFACE);
    if (!s)
        return;

    SDL_Rect src, dst;

    Uint32 last_time   = SDL_GetTicks();
    Uint32 frame_count = 0;
    float  fps         = 0.0f;

    for (int i = 0; i < n_samples; i++)
    {
        Uint32 start = SDL_GetTicks();

        int ws = (rand() % (width  - 1)) + 1;
        int hs = (rand() % (height - 1)) + 1;

        int x = rand() % (width  - ws);
        int y = rand() % (height - hs);

        int size = (rand() % 9) + 1;

        u8 r1 = rand() & 255;
        u8 g1 = rand() & 255;
        u8 b1 = rand() & 255;

        u8 r2 = rand() & 255;
        u8 g2 = rand() & 255;
        u8 b2 = rand() & 255;

        SDL_Rect clear = { 0, 0, ws, hs };
        SDL_FillRect(s, &clear, 0);

        fill_textureRect(
            s, ws, hs,
            color_rgb(r1, g1, b1),
            color_rgb(r2, g2, b2),
            size
        );

        src.x = src.y = 0;
        src.w = ws;
        src.h = hs;

        dst.x = x;
        dst.y = y;
        dst.w = ws;
        dst.h = hs;

        SDL_BlitSurface(s, &src, logic, &dst);

        Uint32 draw_time = SDL_GetTicks() - start;

        /* FPS realtime */
        frame_count++;
        Uint32 now = SDL_GetTicks();
        if (now - last_time >= 1000)
        {
            fps = (frame_count * 1000.0f) / (now - last_time);
            frame_count = 0;
            last_time = now;
        }

        /* ===== Franja azul ===== */
        fill_rect(logic, 0, 0, 320, 15, color_rgb(0, 0, 255));
        print_f(2, 2, color_rgb(255,255,255),
                "TEST2 | FPS: %.2f | Draw: %u ms | Frame: %d",
                fps, draw_time, i + 1);

        Render();
    }

    SDL_FreeSurface(s);

    cls();
    print(40, 100, "Test 2 completed successfully", color_rgb(r, g, b));
    Render();
    SDL_Delay(2000);
    cls();
}


#endif
