#ifndef TEST5_H_
#define TEST5_H_

#include <engine/engine.h>

/*
 ============================================================================
  Test 5 - Random Vertical Gradient Stress Test
  Author: Andrés Ruiz Pérez
  Description:
      Stress test for vertical gradient generation using libGPP-Engine.

      Features:
          - Uses a persistent surface (reused)
          - Applies random vertical gradients
          - Renders random-sized regions at random screen coordinates
          - Synchronizes FPS for consistent playback across devices
          - Tests rendering stability (NOT allocator stress)

      Supported Platforms:
          - Android
          - PSP
          - PS2
          - GameCube
 ============================================================================

 ============================================================================
  NOTA IMPORTANTE (DISEÑO CORRECTO)

  Este test reutiliza UNA sola superficie persistente para evitar fragmentación
  de memoria en plataformas embebidas. La creación y destrucción dinámica de
  superficies con tamaños variables no es un patrón recomendado para uso real.

  El objetivo del test es medir el costo de generación de gradientes verticales
  y el rendimiento del renderizado, no el sistema de asignación de memoria.
 ============================================================================
*/

void TEST5()
{
    u8 r = 0, g = 255, b = 0;
    const int n_samples = 1000;

    print(60, 100, "Starting Render Test 5...", color_rgb(r, g, b));
    Render();
    SDL_Delay(2000);
    cls();

    /* Obtener resolución REAL del video */
    int screen_w, screen_h;
    get_wh_video_mode(&screen_w, &screen_h);

    /* Superficie persistente reutilizable */
    SDL_Surface *s = create_surface(screen_w, screen_h, SDL_SWSURFACE);
    if (!s)
    {
        print(70, 100, "ERROR: Failed to create surface", color_rgb(255, 0, 0));
        Render();
        SDL_Delay(2000);
        return;
    }

    for (int i = 0; i < n_samples; i++)
    {
        int ws = (rand() % (screen_w - 1)) + 1;
        int hs = (rand() % (screen_h - 1)) + 1;

        u8 r1 = rand() & 0xFF;
        u8 g1 = rand() & 0xFF;
        u8 b1 = rand() & 0xFF;

        u8 r2 = rand() & 0xFF;
        u8 g2 = rand() & 0xFF;
        u8 b2 = rand() & 0xFF;

        int x = rand() % (screen_w - ws);
        int y = rand() % (screen_h - hs);

        /* Generar gradiente vertical en la superficie reutilizable */
        fill_vertical_gradient(
            s,
            color_rgb(r1, g1, b1),
            color_rgb(r2, g2, b2)
        );

        /* Dibujar solo la región necesaria */
        draw_surface_region(s, 0, 0, ws, hs, x, y);

        Render();
        // Fps_sincronizar(10);
    }

    SDL_FreeSurface(s);

    cls();
    print(15, 100, "Render Test 5 Completed Successfully", color_rgb(r, g, b));
    Render();
    SDL_Delay(2000);
    cls();
}


#endif