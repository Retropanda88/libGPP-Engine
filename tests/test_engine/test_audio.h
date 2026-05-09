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
    float marqueePos = 0.0f; // Para el movimiento del texto
    
    bool u_l=0, d_l=0, l_l=0, r_l=0, a_l=0;

    while (!exiting) {
        Input::update();
        bool u = Input::isDown(0, BUTTON_UP), d = Input::isDown(0, BUTTON_DOWN);
        bool l = Input::isDown(0, BUTTON_LEFT), r = Input::isDown(0, BUTTON_RIGHT);
        bool a = Input::isDown(0, BUTTON_A), b = Input::isDown(0, BUTTON_B);

        AudioList* cur = (activeCol == 0) ? &musicList : &sfxList;

        if (b) exiting = true;
        if (l && !l_l) activeCol = 0;
        if (r && !r_l) activeCol = 1;

        // Si cambiamos de selección, reiniciamos el marquee
        if ((d && !d_l) || (u && !u_l)) {
            marqueePos = 0.0f;
            if (d && !d_l) cur->selected = (cur->selected + 1) % cur->count;
            if (u && !u_l) cur->selected = (cur->selected - 1 + cur->count) % cur->count;
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
                        // RECUADRO DE SELECCIÓN
                        fill_rect(logic, startX - 4, py - 2, 135, 14, color_rgb(40, 60, 120));
                        
                        // LÓGICA DE MARQUEE
                        int textLen = strlen(lst->names[idx]) * 8; // 8px por letra aprox
                        int limit = 120; // Ancho máximo antes de mover
                        
                        if (textLen > limit) {
                            marqueePos -= 0.5f;
                            if (marqueePos < -(textLen)) marqueePos = limit;
                            // Dibujamos con clipping (si tu engine lo permite) o simplemente offset
                            print(startX + (int)marqueePos, py, lst->names[idx], color_rgb(255, 255, 255));
                        } else {
                            print(startX, py, lst->names[idx], color_rgb(255, 255, 255));
                        }
                    } else {
                        // Texto estático truncado para no invadir otra columna
                        char shortName[18];
                        strncpy(shortName, lst->names[idx], 15);
                        shortName[15] = '.'; shortName[16] = '.'; shortName[17] = '\0';
                        print(startX, py, (strlen(lst->names[idx]) > 15) ? shortName : lst->names[idx], color_rgb(130, 130, 140));
                    }
                }
            }

            // Scrollbar (igual que el código anterior)
            if (lst->count > LIST_VISIBLE) {
                int scrollX = (col == 0) ? 158 : 310;
                int barH = LIST_VISIBLE * 18 - 4;
                fill_rect(logic, scrollX, 55, 2, barH, color_rgb(40, 40, 50));
                int cursorH = (LIST_VISIBLE * barH) / lst->count;
                int cursorY = 55 + (lst->scroll * (barH - cursorH)) / (lst->count - LIST_VISIBLE);
                fill_rect(logic, scrollX, cursorY, 2, cursorH, (activeCol == col) ? color_rgb(0, 255, 255) : color_rgb(80, 80, 90));
            }
        }

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
