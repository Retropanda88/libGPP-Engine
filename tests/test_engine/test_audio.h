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
    CSample* samples[MAX_FILES]; 
    int count;
    int selected;
    int scroll;
};

// Limpieza profunda de memoria para evitar corrupción de textos en PS2
void unload_and_clear(AudioList* list) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (i < list->count && list->samples[i]) {
            list->samples[i]->close();
            delete list->samples[i];
            list->samples[i] = NULL;
        }
        memset(list->names[i], 0, FS_MAX_NAME);
    }
    list->count = 0;
}

void scan_and_preload(const char* path, AudioList* list, bool preload) {
    FS_DIR dir;
    FS_DIRENT ent;
    list->count = 0;
    if (fs_opendir(&dir, path) == 0) {
        while (fs_readdir(&dir, &ent) == 0 && list->count < MAX_FILES) {
            if (!ent.is_dir && strstr(ent.name, ".wav")) {
                strncpy(list->names[list->count], ent.name, FS_MAX_NAME);
                if (preload) {
                    char fullPath[512];
                    sprintf(fullPath, "%s/%s", path, ent.name);
                    list->samples[list->count] = new CSample();
                    list->samples[list->count]->Load(fullPath);
                } else {
                    list->samples[list->count] = NULL;
                }
                list->count++;
            }
        }
        fs_closedir(&dir);
    }
}

void run_audio_test() {
    // SEGURIDAD PS2: Detener audio antes de leer archivos
    mixer.stopMusic();
    mixer.stopAll();

    AudioList musicList, sfxList;
    scan_and_preload("music", &musicList, false);
    scan_and_preload("sfx", &sfxList, true);

    int activeCol = 0, volume = 64;
    bool exiting = false;
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

        if ((l && !l_l) || (r && !r_l) || (d && !d_l) || (u && !u_l)) {
            activeCol = l ? 0 : (r ? 1 : activeCol);
            marqueeTimer = 0.0f; charOffset = 0;
            if (d && !d_l && cur->count > 0) cur->selected = (cur->selected + 1) % cur->count;
            if (u && !u_l && cur->count > 0) cur->selected = (cur->selected - 1 + cur->count) % cur->count;
        }

        if (Input::isDown(0, BUTTON_L1) && volume > 2) volume -= 2;
        if (Input::isDown(0, BUTTON_R1) && volume < 126) volume += 2;
        mixer.setMasterVolume(volume);

        if (cur->selected >= cur->scroll + LIST_VISIBLE) cur->scroll = cur->selected - LIST_VISIBLE + 1;
        if (cur->selected < cur->scroll) cur->scroll = cur->selected;

        if (a && !a_l && cur->count > 0) {
            if (activeCol == 0) {
                mixer.stopMusic();
                char path[512];
                sprintf(path, "music/%s", cur->names[cur->selected]);
                mixer.playMusic(path, true);
            } else {
                if (cur->samples[cur->selected]) {
                    mixer.playChannel(cur->samples[cur->selected], false, volume);
                }
            }
        }

        // --- RENDERIZADO ORIGINAL MANTENIDO ---
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
                        fill_rect(logic, startX - 4, py - 2, 135, 14, color_rgb(40, 60, 120));
                        int nameLen = strlen(lst->names[idx]);
                        if (nameLen > 15) {
                            marqueeTimer += 0.15f;
                            if (marqueeTimer > 1.0f) { marqueeTimer = 0.0f; charOffset++; if (charOffset > nameLen - 5) charOffset = 0; }
                            char displayBuffer[32];
                            strncpy(displayBuffer, &lst->names[idx][charOffset], 16);
                            displayBuffer[16] = '\0';
                            print(startX, py, displayBuffer, color_rgb(255, 255, 255));
                        } else {
                            print(startX, py, lst->names[idx], color_rgb(255, 255, 255));
                        }
                    } else {
                        char shortName[32];
                        if (strlen(lst->names[idx]) > 15) {
                            strncpy(shortName, lst->names[idx], 12);
                            shortName[12] = '\0'; strcat(shortName, "...");
                            print(startX, py, shortName, color_rgb(130, 130, 140));
                        } else {
                            print(startX, py, lst->names[idx], color_rgb(130, 130, 140));
                        }
                    }
                }
            }

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

        fill_rect(logic, 0, 210, 320, 30, color_rgb(5, 5, 10));
        print(15, 218, "VOL", color_rgb(0, 255, 0));
        fill_rect(logic, 45, 221, 100, 6, color_rgb(40, 40, 40));
        fill_rect(logic, 45, 221, (volume * 100 / 128), 6, color_rgb(0, 255, 0));
        
        fontsize(6, 6);
        print(155, 221, "A:PLAY L1/R1:VOL ARROWS:NAV", color_rgb(140, 140, 150));
        fontsize(8, 8); 

        u_l=u; d_l=d; l_l=l; r_l=r; a_l=a;
        Render();
        Fps_sincronizar(60);
    }

    // LIMPIEZA FINAL
    unload_and_clear(&musicList);
    unload_and_clear(&sfxList);
}

#endif
