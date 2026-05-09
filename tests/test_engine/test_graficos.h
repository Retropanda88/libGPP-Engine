#ifndef GRAFICOS_TEST_H
#define GRAFICOS_TEST_H

#include <engine/engine.h>
#include <input/Input.h>
//#include <SDL.h>
#include <stdio.h>

extern SDL_Surface *logic;

inline void run_graficos_test()
{
    printf(">> SUIT TEST VIDEO: FINALIZADO E INTEGRADO\n");

    if (!logic) return;

    bool testing = true;
    int modo = 0; // 0: Geometria, 1: Gradientes, 2: Contraste, 3: Mix Final
    const int gridStep = 16;

    u32 negro    = color_rgb(0, 0, 0);
    u32 blanco   = color_rgb(255, 255, 255);
    u32 rojo     = color_rgb(255, 0, 0);
    u32 verde    = color_rgb(0, 255, 0);
    u32 azul     = color_rgb(0, 0, 255);
    u32 amarillo = color_rgb(255, 255, 0);
    u32 cyan     = color_rgb(0, 255, 255);

    while (testing)
    {
        Input::update();

        if (Input::isPressed(0, BUTTON_B)) testing = false;
        
        if (Input::isPressed(0, BUTTON_RIGHT)) {
            modo++;
            if (modo > 3) modo = 0;
        }
        if (Input::isPressed(0, BUTTON_LEFT)) {
            modo--;
            if (modo < 0) modo = 3;
        }

        fill_rect(logic, 0, 0, logic->w, logic->h, negro);

        if (modo == 0) {
            // --- MODO 1: GEOMETRIA PURA ---
            for (int x = 0; x <= logic->w; x += gridStep) {
                u32 c = (x == 0 || x >= logic->w - 1) ? rojo : blanco;
                draw_line_fast(logic, x, 0, x, logic->h, c);
            }
            for (int y = 0; y <= logic->h; y += gridStep) {
                u32 c = (y == 0 || y >= logic->h - 1) ? rojo : blanco;
                draw_line_fast(logic, 0, y, logic->w, y, c);
            }
            print(10, 10, "[1/4] GEOMETRIA", rojo);
        } 
        else if (modo == 1) {
            // --- MODO 2: GRADIENTES ---
            int bh = 30; int sy = 40;
            u32 colores[] = {blanco, rojo, verde, azul};
            for(int i=0; i<4; i++) {
                SDL_Surface* s = create_surface(240, bh, 0);
                fill_horizontal_gradient(s, negro, colores[i]);
                draw_surface(s, 40, sy + (i*40));
                SDL_FreeSurface(s);
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
            // --- MODO 4: MIX FINAL (REJILLA + BARRAS) ---
            // Dibujar rejilla de fondo en gris oscuro
            u32 gris_osc = color_rgb(60, 60, 60);
            for (int x = 0; x <= logic->w; x += gridStep) draw_line_fast(logic, x, 0, x, logic->h, gris_osc);
            for (int y = 0; y <= logic->h; y += gridStep) draw_line_fast(logic, 0, y, logic->w, y, gris_osc);

            // Barras con etiquetas
            const char* labels[] = {"RED", "GREEN", "BLUE", "WHITE"};
            u32 cols[] = {rojo, verde, azul, blanco};
            for(int i=0; i<4; i++) {
                print(45, 45 + (i*40), (char*)labels[i], cols[i]); // Etiquetas
                SDL_Surface* s = create_surface(220, 32, 0);
                fill_horizontal_gradient(s, negro, cols[i]);
                draw_surface(s, 45, 55 + (i*40));
                SDL_FreeSurface(s);
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

    SDL_Event e;
    while (SDL_PollEvent(&e));
}

#endif
