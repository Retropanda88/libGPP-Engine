/*
 ============================================================================
  Test 3 - Alpha Fade Stress Test
  Author: Andrés Ruiz Pérez
  Description:
      Alpha blending stress test for libGPP-Engine.
      This test performs:
        - Surface creation with a rectangle-pattern texture
        - Fullscreen rendering
        - Dynamic alpha fade-in and fade-out
        - Frame synchronization

      Validates alpha blending performance and stability on:
          - Android
          - PSP
          - PS2
          - GameCube
 ============================================================================

 ============================================================================
  NOTA IMPORTANTE SOBRE ALPHA BLENDING Y MEMORIA (CONSOLAS)

  Este test utiliza alpha blending dinámico sobre una superficie de pantalla
  completa. En plataformas embebidas como PSP, PS2 y GameCube:

  - El alpha blending por superficie puede ser costoso, especialmente cuando
    se actualiza el valor de alpha cada frame.
  - El uso de superficies grandes con alpha puede impactar fuertemente el
    rendimiento si el backend no soporta alpha por hardware.
  - La creación y destrucción de superficies dinámicas con alpha debe evitarse
    en bucles frecuentes, ya que puede agravar problemas de fragmentación
    de memoria.

  Este comportamiento no se considera un bug del motor, sino una limitación
  del hardware y de los backends gráficos de cada plataforma.

  RECOMENDACIONES:
  - Reutilizar superficies persistentes para efectos de fade
  - Evitar múltiples superficies fullscreen con alpha activo
  - Considerar fades por overlay o quad dedicado cuando sea posible

 ============================================================================
*/

#ifndef TEST3_H_
#define TEST3_H_

#include <engine/engine.h>

void TEST3()
{
    u8 r = 0, g = 255, b = 0;

    print(30, 100, "Starting Test 3: Alpha Fade Test", color_rgb(r, g, b));
    Render();
    SDL_Delay(2000);
    cls();

    /* Obtener resolución REAL del video */
    int screen_w, screen_h;
    get_wh_video_mode(&screen_w, &screen_h);

    /* Colores aleatorios */
    u8 r1 = rand() & 0xFF;
    u8 g1 = rand() & 0xFF;
    u8 b1 = rand() & 0xFF;

    u8 r2 = rand() & 0xFF;
    u8 g2 = rand() & 0xFF;
    u8 b2 = rand() & 0xFF;

    const int block_size = 5;

    /* Control de alpha */
    u8 alpha = 1;
    int step = 1;
    int done = 0;

    /* Superficie fullscreen reutilizable */
    SDL_Surface *s = create_surface(screen_w, screen_h, SDL_SWSURFACE);
    if (!s)
    {
        print(60, 100, "ERROR: Failed to create surface", color_rgb(255, 0, 0));
        Render();
        SDL_Delay(2000);
        return;
    }

    /* Generar textura SOLO UNA VEZ */
    fill_textureRect(
        s,
        screen_w,
        screen_h,
        color_rgb(r1, g1, b1),
        color_rgb(r2, g2, b2),
        block_size
    );

    /* =========================
       FPS / Draw metrics
       ========================= */
    Uint32 last_time   = SDL_GetTicks();
    Uint32 frame_count = 0;
    float  fps         = 0.0f;

    /* Fade-in / Fade-out */
    while (!done)
    {
        Uint32 start = SDL_GetTicks();

        apply_alpha(s, alpha);
        cls();
        draw_surface(s, 0, 0);

        /* FPS y draw_time */
        Uint32 draw_time = SDL_GetTicks() - start;
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
        print_f(0, 2, color_rgb(255,255,255),
                "TEST3 | FPS: %.2f | Draw: %u ms | Alpha: %d",
                fps, draw_time, alpha);

        Render();

        /* Actualizar alpha */
        alpha += step;
        if (alpha >= 255) step = -1;
        if (alpha <= 1)   done = 1;
    }

    SDL_FreeSurface(s);

    cls();
    print(50, 100, "Test 3 completed successfully", color_rgb(r, g, b));
    Render();
    SDL_Delay(2000);
    cls();
}


#endif