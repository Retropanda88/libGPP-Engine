#ifndef TEST9_H_
#define TEST9_H_

#include <engine/engine.h>
#include "sheet_bmp.h"

/* 
   ============================================================================
   Test 9 - Sprite Sheet Stress Test Author: Andrés Ruiz Pérez

   Description: Sprite animation stress test using an 8x6 sprite sheet. The
   sheet is loaded as a temporary Sprite, sliced into independent animated
   sprites, and then released.

   Sheet layout: - Rows : 8 - Columns : 6 - Frame : 32x32 pixels

   Supported Platforms: - Android - PSP - PS2 - GameCube
   ============================================================================ */

#define SPRITE_SIZE   32
#define SHEET_COLS    6
#define SHEET_ROWS    8

Sprite *spr[SHEET_ROWS] = { NULL };

void TEST9()
{
    u8 r = 0, g = 255, b = 0;

    print(60, 100, "Starting Render Test 9:", color_rgb(r, g, b));
    print(40, 112, "Sprite Sheet 8x6 Animation", color_rgb(r, g, b));
    Render();
    SDL_Delay(2000);
    cls();

    int sw, sh;
    get_wh_video_mode(&sw, &sh);

    /* Load full sheet */
    SDL_Surface *sheetSurface =
        load_texture_from_mem((u8 *)sheet_data, sheet_size);

    if (!sheetSurface)
    {
        print(10, 100, "Error: Failed to load sprite sheet",
              color_rgb(255, 0, 0));
        Render();
        SDL_Delay(2000);
        cls();
        return;
    }

    Sprite *sheet = new Sprite(sheetSurface, 1, 1);
    sheet->setTransparency(255, 0, 255);

    /* Slice sprite sheet (one animation per row) */
    for (int row = 0; row < SHEET_ROWS; row++)
    {
        SDL_Surface *rowSurface =
            sheet->getRect(
                0,
                row * SPRITE_SIZE,
                SPRITE_SIZE * SHEET_COLS,
                SPRITE_SIZE
            );

        spr[row] = new Sprite(rowSurface, SHEET_COLS, 8);
        spr[row]->setTransparency(255, 0, 255);
    }

    delete sheet;

    /* ================= Animation loop ================= */
    for (int frame = 0; frame < 400; frame++)
    {
        cls();

        for (int i = 0; i < SHEET_ROWS; i++)
        {
            int x = 16 + i * 36;
            int y = 100;

            /* SOLO protección de límites */
            if (x + SPRITE_SIZE > sw) x = sw - SPRITE_SIZE;
            if (y + SPRITE_SIZE > sh) y = sh - SPRITE_SIZE;
            if (x < 0) x = 0;
            if (y < 0) y = 0;

            spr[i]->animate()->draw(logic, x, y);
        }

        Render();
        //Fps_sincronizar(60);
    }

    /* Cleanup */
    for (int i = 0; i < SHEET_ROWS; i++)
    {
        delete spr[i];
        spr[i] = NULL;
    }

    cls();
    print(15, 100, "Render Test 9 Completed Successfully",
          color_rgb(r, g, b));
    Render();
    SDL_Delay(2000);
    cls();
}


#endif
