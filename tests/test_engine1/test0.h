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

#ifndef TEST0_H_
#define TEST0_H_

#include <engine/engine.h>

void TEST0()
{
    Uint32 last_time   = SDL_GetTicks();
    Uint32 frame_count = 0;
    float  fps         = 0.0f;

    const int triangles_per_frame = 5;
    const int max_triangles = 10000;

    int total_triangles = 0;

    while (total_triangles < max_triangles)
    {
        Uint32 start = SDL_GetTicks();

        cls_rgb(0, 0, 0);

        for (int i = 0;
             i < triangles_per_frame && total_triangles < max_triangles;
             i++)
        {
            int x0 = rand() % 320;
            int y0 = rand() % 240;
            int x1 = rand() % 320;
            int y1 = rand() % 240;
            int x2 = rand() % 320;
            int y2 = rand() % 240;

            u32 c = color_rgb(rand() & 255,
                              rand() & 255,
                              rand() & 255);

            draw_triangle(logic,
                          x0, y0,
                          x1, y1,
                          x2, y2,
                          c);

            total_triangles++;
        }

        Uint32 draw_time = SDL_GetTicks() - start;

        /* FPS */
        frame_count++;
        Uint32 now = SDL_GetTicks();
        if (now - last_time >= 1000)
        {
            fps = (frame_count * 1000.0f) / (now - last_time);
            frame_count = 0;
            last_time = now;
        }

        /* Franja azul (se mantiene) */
        fill_rect(logic, 0, 0, 320, 15, color_rgb(0, 0, 255));
        print_f(2, 2, color_rgb(255, 255, 255),
                "TEST0 | FPS: %.2f | Draw: %u ms ",
                fps, draw_time, total_triangles);

        Render();
    }
    cls();
}

#endif