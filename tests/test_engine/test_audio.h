#ifndef AUDIO_TEST_H
#define AUDIO_TEST_H

#include <engine/engine.h>
#include <input/Input.h>
#include <audio/mixer.h>
#include <audio/sample.h>
#include <font/gfxFont.h>
#include <filesystem/fs.h>
#include <stdio.h>
#include <string.h>

extern SDL_Surface *logic;
extern Cmixer mixer;

#define MAX_FILES 64
#define LIST_VISIBLE 7 

struct AudioList {
    char names[MAX_FILES][FS_MAX_NAME];
    int count;
    int selected;
    int scroll;
};

void scan_directory(const char* path, AudioList* list) {
    FS_DIR dir;
    FS_DIRENT ent;
    list->count = 0; list->selected = 0; list->scroll = 0;
    if (fs_opendir(&dir, path) == 0) {
        while (fs_readdir(&dir, &ent) == 0 && list->count < MAX_FILES) {
            if (!ent.is_dir && strstr(ent.name, ".wav")) {
                strncpy(list->names[list->count], ent.name, FS_MAX_NAME);
                list->count++;
            }
        }
        fs_closedir(&dir);
    }
}

void run_audio_test() {
    AudioList musicList, sfxList;
    scan_directory("music", &musicList);
    scan_directory("sfx", &sfxList);

    int activeCol = 0; 
    int volume = 64;
    bool exiting = false;
    
    // Variables para el Marquee dinámico
    float marqueeTimer = 0.0f;
    int charOffset = 0;

    bool u_l=0, d_l=0, l_l=0, r_l=0, a_l=0;

    while (!exiting) {
        Input::update();
        bool u = Input::isDown(0, BUTTON_UP), d = Input::isDown(0, BUTTON_DOWN);
        bool l = Input::isDown(0, BUTTON_LEFT), r = Input::isDown(0, BUTTON_RIGHT);
        bool a = Input::isDown(0, BUTTON_A), b = Input::isDown(0, BUTTON_B);

        AudioList* cur = (activeCol == 0) ? &musicList : &sfxList;

        if (b) exiting = true;
        
        // Cambio de columna
        if (l && !l_l) { activeCol = 0; marqueeTimer = 0; charOffset = 0; }
        if (r && !r_l) { activeCol = 1; marqueeTimer = 0; charOffset = 0; }

        // Navegación
        if ((d && !d_l) || (u && !u_l)) {
            marqueeTimer = 0; 
            charOffset = 0;
            if (d && !d_l && cur->count > 0) cur->selected = (cur->selected + 1) % cur->count;
            if (u && !u_l && cur->count > 0) cur->selected = (cur->selected - 1 + cur->count) % cur->count;
        }

        if (Input::isDown(0, BUTTON_L1) && volume > 0) volume--;
        if (Input::isDown(0, BUTTON_R1) && volume < 128) volume++;

        if (cur->selected >= cur->scroll + LIST_VISIBLE) cur->scroll = cur->selected - LIST_VISIBLE + 1;
        if (cur->selected < cur->scroll) cur->scroll = cur->selected;

        if (a && !a_l && cur->count > 0) {
            char path[512];
            sprintf(path, "%s/%s", (activeCol == 0 ? "music" : "sfx"), cur->names[cur->selected]);
            if (activeCol == 0) mixer.playMusic(path, true);
            else {
                static CSample s; 
                if(s.Load(path)) mixer.playChannel(&s, 0, volume);
            }
        }

        // --- RENDER ---
        fill_vertical_gradient(logic, color_rgb(10, 15, 30), color_rgb(20, 30, 50));
        fontsize(8, 8); 
        print(15, 10, "AUDIO EXPLORER", color_rgb(0, 255, 255));
        fill_rect(logic, 15, 22, 290, 1, color_rgb(0, 255, 255));

        for (int col = 0; col < 2; col++) {
            AudioList* lst = (col == 0) ? &musicList : &sfxList;
            int startX = (col == 0) ? 20 : 170;

            for (int i = 0; i < LIST_VISIBLE; i++) {
                int idx = lst->scroll + i;
                if (idx < lst->count) {
                    int py = 55 + (i * 18);
                    if (activeCol == col && idx == lst->selected) {
                        // 1. RECUADRO SELECCIÓN
                        fill_rect(logic, startX - 4, py - 2, 135, 14, color_rgb(40, 60, 120));
                        
                        // 2. LÓGICA DE MARQUEE (VENTANA DINÁMICA)
                        int nameLen = strlen(lst->names[idx]);
                        if (nameLen > 15) { // Si el nombre es largo, lo movemos
                            marqueeTimer += 0.15f; // Velocidad del scroll
                            if (marqueeTimer > 1.0f) {
                                marqueeTimer = 0.0f;
                                charOffset++;
                                if (charOffset > nameLen - 5) charOffset = 0; // Reinicia al llegar al final
                            }
                            char displayBuffer[17];
                            strncpy(displayBuffer, &lst->names[idx][charOffset], 16);
                            displayBuffer[16] = '\0';
                            print(startX, py, displayBuffer, color_rgb(255, 255, 255));
                        } else {
                            print(startX, py, lst->names[idx], color_rgb(255, 255, 255));
                        }
                    } else {
                        // Texto truncado para los no seleccionados (Máximo 15 caracteres)
                        char shortName[17];
                        if (strlen(lst->names[idx]) > 15) {
                            strncpy(shortName, lst->names[idx], 12);
                            shortName[12] = '\0';
                            strcat(shortName, "...");
                            print(startX, py, shortName, color_rgb(130, 130, 140));
                        } else {
                            print(startX, py, lst->names[idx], color_rgb(130, 130, 140));
                        }
                    }
                }
            }

            // Scrollbar (Mantenemos la de 2px de ancho)
            if (lst->count > LIST_VISIBLE) {
                int scrollX = (col == 0) ? 158 : 312;
                int barH = LIST_VISIBLE * 18 - 4;
                fill_rect(logic, scrollX, 55, 2, barH, color_rgb(40, 40, 50));
                int cursorH = (LIST_VISIBLE * barH) / lst->count;
                if (cursorH < 6) cursorH = 6;
                int cursorY = 55 + (lst->scroll * (barH - cursorH)) / (lst->count - LIST_VISIBLE);
                fill_rect(logic, scrollX, cursorY, 2, cursorH, (activeCol == col) ? color_rgb(0, 255, 255) : color_rgb(80, 80, 90));
            }
        }

        // Barra Volumen
        fill_rect(logic, 0, 210, 320, 30, color_rgb(5, 5, 10));
        print(15, 218, "VOL", color_rgb(0, 255, 0));
        fill_rect(logic, 45, 221, 100, 6, color_rgb(40, 40, 40));
        fill_rect(logic, 45, 221, (volume * 100 / 128), 6, color_rgb(0, 255, 0));
        
        u_l=u; d_l=d; l_l=l; r_l=r; a_l=a;
        Render();
        Fps_sincronizar(60);
    }
}
#endif
