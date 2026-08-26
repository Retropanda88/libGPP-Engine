#ifndef GRAFICOS_TEST_H
#define GRAFICOS_TEST_H

#include <engine/engine.h>
#include <input/Input.h>
#include <stdio.h>

extern SDL_Surface *logic;
extern SDL_Surface* surf_gradientes[4];
extern SDL_Surface* surf_mix[4];

inline void run_graficos_test()
{
    printf(">> SUIT TEST VIDEO: NATIVO Y ESTABILIZADO\n");

    if (!logic) return;

    Input::update(); 

    bool testing = true;
    int modo = 0; 
    const int gridStep = 16;

    u32 negro    = color_rgb(0, 0, 0);
    u32 blanco   = color_rgb(255, 255, 255);
    u32 rojo     = color_rgb(255, 0, 0);
    u32 verde    = color_rgb(0, 255, 0);
    u32 amarillo = color_rgb(255, 255, 0);
    u32 cyan     = color_rgb(0, 255, 255);

    bool b_l = true; 
    bool r_l = true;
    bool l_l = true;

    while (testing)
    {
        Input::update();
        bool b = Input::isDown(0, BUTTON_B);
        bool r = Input::isPressed(0, BUTTON_RIGHT);
        bool l = Input::isPressed(0, BUTTON_LEFT);

        if (b && !b_l) {
            testing = false;
        }

        if (r && !r_l) {
            modo = (modo + 1) % 4;
        }
        if (l && !l_l) {
            modo = (modo - 1 + 4) % 4;
        }

        b_l = b;
        r_l = r;
        l_l = l;

        fill_rect(logic, 0, 0, logic->w, logic->h, negro);

        if (modo == 0) {
            // --- MODO 1: GEOMETRIA PURA ---
            for (int x = 0; x <= logic->w; x += gridStep) {
                u32 c = (x == 0 || x >= logic->w - 1) ? rojo : blanco;
                draw_line_fast(logic, x, 0, x, logic->h, c);
            }
            for (int i = 0; i <= logic->h; i += gridStep) {
                u32 c = (i == 0 || i >= logic->h - 1) ? rojo : blanco;
                draw_line_fast(logic, 0, i, logic->w, i, c);
            }
            print(10, 10, "[1/4] GEOMETRIA", rojo);
        } 
        else if (modo == 1) {
            // --- MODO 2: GRADIENTES ---
            int sy = 40;
            for(int i = 0; i < 4; i++) {
                if (surf_gradientes[i]) {
                    draw_surface(surf_gradientes[i], 40, sy + (i * 40));
                }
            }
            print(10, 10, "[2/4] GRADIENTES", cyan);
        }
        else if (modo == 2) {
            // --- MODO 3: CONTRASTE (LAGOM) ---
            int barW = 256; int stepW = barW / 32; int barH = 18;
            struct { u8 r, g, b; } canales[] = {{0,0,1}, {0,1,0}, {0,1,1}, {1,0,0}, {1,0,1}, {1,1,0}, {1,1,1}};
            for(int i = 0; i < 7; i++) {
                for(int step = 0; step < 32; step++) {
                    u8 val = (u8)((step * 255) / 31);
                    u32 c = color_rgb(val * canales[i].r, val * canales[i].g, val * canales[i].b);
                    fill_rect(logic, 32 + (step * stepW), 30 + (i * (barH + 4)), stepW, barH, c);
                }
            }
            print(10, 10, "[3/4] CONTRASTE", verde);
        }
        else {
            // --- MODO 4: MIX FINAL (CORREGIDO) ---
            u32 gris_osc = color_rgb(60, 60, 60);
            for (int x = 0; x <= logic->w; x += gridStep) draw_line_fast(logic, x, 0, x, logic->h, gris_osc);
            
            // CORRECCIÓN: Cambiado 'i <= logic->h' por 'y <= logic->h'
            for (int y = 0; y <= logic->h; y += gridStep) draw_line_fast(logic, 0, y, logic->w, y, gris_osc);

            const char* labels[] = {"RED", "GREEN", "BLUE", "WHITE"};
            u32 cols[] = {rojo, verde, color_rgb(0,0,255), blanco};
            
            for(int i = 0; i < 4; i++) {
                print(45, 45 + (i * 40), (char*)labels[i], cols[i]); 
                if (surf_mix[i]) {
                    draw_surface(surf_mix[i], 45, 55 + (i * 40));
                }
            }
            print(10, 10, "[4/4] MIX: CONVERGENCIA & COLOR", amarillo);
        }

        // UI INDICACIONES
        fill_rect(logic, 0, 215, logic->w, 25, color_rgb(30, 30, 30));
        fontsize(8, 8);
        print(15, 220, "IZQ / DER PARA NAVEGAR | B PARA SALIR", blanco);

        Render();
        Fps_sincronizar(60);
    }
}

#endif