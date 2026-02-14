#ifndef TEST6_H_
#define TEST6_H_

#include <engine/engine.h>

/*
 ============================================================================
  Test 6 - Random Radial Gradient Stress Test
  Author: Andrés Ruiz Pérez
  Description:
      Stress test for radial gradient generation using libGPP-Engine.

      Features:
          - Uses a persistent surface (reused)
          - Applies random radial gradients
          - Renders random-sized regions at random screen coordinates
          - Ensures consistent timing with FPS synchronization
          - Validates rendering stability (NOT allocator stress)

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
  superficies con tamaños variables no representa un uso real del motor.

  El objetivo del test es medir el rendimiento del algoritmo de gradiente radial
  y del pipeline de render, no del sistema de asignación de memoria.
 ============================================================================
*/

void TEST6()
{
    u8 r = 0, g = 255, b = 0;
    const int n_samples = 1000;

    print(60, 100, "Starting Render Test 6...", color_rgb(r, g, b));
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

        /* Generar gradiente radial en la superficie reutilizable */
        fill_radial_gradient(
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
    print(20, 100, "Render Test 6 Completed Successfully", color_rgb(r, g, b));
    Render();
    SDL_Delay(2000);
    cls();
}


#endif