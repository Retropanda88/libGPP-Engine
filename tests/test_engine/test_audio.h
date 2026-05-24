#ifndef AUDIO_TEST_H
#define AUDIO_TEST_H

#include <engine/engine.h>
#include <input/Input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern SDL_Surface *logic;
extern Cmixer mixer;

#define LIST_VISIBLE 9 
#define MAX_NAME_LEN 64

struct AudioList {
    char (*names)[MAX_NAME_LEN]; 
    int count;                  
    int selected;
    int scroll;
};

extern AudioList musicList;
extern AudioList sfxList;
extern int global_volume; 

// Referenciamos el banco de memoria de efectos externos cargados desde el main.cpp
extern CSample* sfxPreloadedBank;

inline void run_audio_test() {
    int activeCol = 0; // 0 = Música, 1 = SFX
    bool exiting = false;
    float marqueeTimer = 0.0f;
    int charOffset = 0;
    char pctText[32]; 
    
    bool u_l=true, d_l=true, l_l=true, r_l=true, a_l=true, b_l=true;
    static char safePath[256];

    while (!exiting) {
        Input::update();
        bool u = Input::isDown(0, BUTTON_UP), d = Input::isDown(0, BUTTON_DOWN);
        bool l = Input::isDown(0, BUTTON_LEFT), r = Input::isDown(0, BUTTON_RIGHT);
        bool a = Input::isDown(0, BUTTON_A), b = Input::isDown(0, BUTTON_B);
        
        AudioList* cur = (activeCol == 0) ? &musicList : &sfxList;

        if (b && !b_l) {
            exiting = true; 
        }

        // Cambiar entre pestañas de pantalla completa (Cero llamadas a disco)
        if ((l && !l_l) || (r && !r_l)) {
            activeCol = (activeCol == 0) ? 1 : 0;
            marqueeTimer = 0.0f; 
            charOffset = 0;
            cur = (activeCol == 0) ? &musicList : &sfxList;
        }

        // Navegación vertical pura en memoria RAM interna (Ultra veloz, sin tartamudeos)
        if (d && !d_l && cur->count > 0) {
            cur->selected = (cur->selected + 1) % cur->count;
            marqueeTimer = 0.0f; charOffset = 0;
        }
        if (u && !u_l && cur->count > 0) {
            cur->selected = (cur->selected - 1 + cur->count) % cur->count;
            marqueeTimer = 0.0f; charOffset = 0;
        }

        // Ajuste de volumen
        if (Input::isDown(0, BUTTON_L1) && global_volume > 0) global_volume -= 2;
        if (Input::isDown(0, BUTTON_R1) && global_volume < 128) global_volume += 2;
        mixer.setMasterVolume(global_volume);

        if (cur->selected >= cur->scroll + LIST_VISIBLE) cur->scroll = cur->selected - LIST_VISIBLE + 1;
        if (cur->selected < cur->scroll) cur->scroll = cur->selected;

        // REPRODUCCIÓN AISLADA SEGURA
        if (a && !a_l && cur->count > 0 && cur->names != NULL) {
            if (activeCol == 0) {
                // La música corre por Streaming directo de archivo (Hilo secundario controlado)
                snprintf(safePath, sizeof(safePath), "music/%s", cur->names[cur->selected]);
                mixer.stopMusic(); 
                mixer.playMusic(safePath, true); 
            } else {
                // ¡REPRODUCCIÓN RAM INMEDIATA! Ejecuta la muestra precargada sin abrir ningún archivo
                if (sfxPreloadedBank != NULL) {
                    mixer.playChannel(&sfxPreloadedBank[cur->selected], false, global_volume);
                }
            }
        }

        // ==================== INTERFAZ GRÁFICA ====================
        fill_vertical_gradient(logic, color_rgb(10, 15, 30), color_rgb(20, 30, 50));
        
        fontsize(8, 8); 
        if (activeCol == 0) {
            print(15, 10, "AUDIO EXPLORER -> [ BACKGROUND MUSIC ]", color_rgb(0, 255, 255));
        } else {
            print(15, 10, "AUDIO EXPLORER -> [ SOUND EFFECTS ]", color_rgb(255, 200, 0));
        }
        fill_rect(logic, 15, 22, 290, 1, color_rgb(60, 60, 80));

        int startX = 20;
        for (int i = 0; i < LIST_VISIBLE; i++) {
            int idx = cur->scroll + i;
            if (idx < cur->count && cur->names != NULL) {
                int py = 42 + (i * 18);
                
                if (idx == cur->selected) {
                    u32 barColor = (activeCol == 0) ? color_rgb(40, 60, 120) : color_rgb(130, 90, 20);
                    fill_rect(logic, startX - 4, py - 2, 265, 14, barColor);
                    
                    int nLen = strlen(cur->names[idx]);
                    if (nLen > 28) { 
                        marqueeTimer += 0.15f;
                        if (marqueeTimer > 1.0f) { marqueeTimer = 0.0f; charOffset++; if (charOffset > nLen - 10) charOffset = 0; }
                        char db[32]; strncpy(db, &cur->names[idx][charOffset], 29); db[29] = '\0';
                        print(startX, py, db, color_rgb(255, 255, 255));
                    } else {
                        print(startX, py, cur->names[idx], color_rgb(255, 255, 255));
                    }
                } else {
                    char sn[32];
                    if (strlen(cur->names[idx]) > 28) {
                        strncpy(sn, cur->names[idx], 25); sn[25] = '\0'; strcat(sn, "...");
                        print(startX, py, sn, color_rgb(130, 130, 140));
                    } else {
                        print(startX, py, cur->names[idx], color_rgb(130, 130, 140));
                    }
                }
            }
        }

        // BARRA DE DESPLAZAMIENTO (SCROLLBAR)
        int scrollAreaY = 40;
        int scrollAreaH = 155;
        int scrollbarX = 295;
        
        fill_rect(logic, scrollbarX, scrollAreaY, 4, scrollAreaH, color_rgb(30, 35, 60));
        
        if (cur->count > 0) {
            int barHeight = (cur->count > LIST_VISIBLE) ? (scrollAreaH * LIST_VISIBLE) / cur->count : scrollAreaH;
            if (barHeight < 15) barHeight = 15;
            
            int maxScrollIdx = (cur->count > LIST_VISIBLE) ? (cur->count - LIST_VISIBLE) : 1;
            int sY = scrollAreaY + (cur->scroll * (scrollAreaH - barHeight) / maxScrollIdx);
            
            u32 scrollColor = (activeCol == 0) ? color_rgb(0, 255, 255) : color_rgb(255, 200, 0);
            fill_rect(logic, scrollbarX, sY, 4, barHeight, scrollColor);
        }

        // INDICADOR DE PESTAÑAS
        fill_rect(logic, 145, 202, 10, 4, (activeCol == 0) ? color_rgb(0, 255, 255) : color_rgb(40, 40, 50));
        fill_rect(logic, 165, 202, 10, 4, (activeCol == 1) ? color_rgb(255, 200, 0) : color_rgb(40, 40, 50));

        // PANEL INFERIOR DE VOLUMEN
        fill_rect(logic, 0, 212, 320, 28, color_rgb(5, 5, 10));
        
        int porcentaje = (global_volume * 100) / 128;
        snprintf(pctText, sizeof(pctText), "VOLUME: %d%%", porcentaje);
        print(12, 220, pctText, color_rgb(0, 255, 0));
        
        fill_rect(logic, 115, 223, 50, 6, color_rgb(40, 40, 40));
        fill_rect(logic, 115, 223, (global_volume * 50 / 128), 6, color_rgb(0, 255, 0));
        
        fontsize(6, 6);
        print(175, 223, "A:PLAY L1/R1:VOL L/R:SWAP TABS", color_rgb(140, 140, 150));
        fontsize(8, 8); 

        u_l=u; d_l=d; l_l=l; r_l=r; a_l=a; b_l=b;
        Render();
        Fps_sincronizar(60);
    }
}
#endif