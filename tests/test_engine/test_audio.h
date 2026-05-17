#ifndef AUDIO_TEST_H
#define AUDIO_TEST_H

#include <engine/engine.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern SDL_Surface *logic;
extern Cmixer mixer;

#define LIST_VISIBLE 7 

// Estructura dinámica alineada a 16 bytes para la PS2
struct __attribute__((aligned(16))) AudioList {
    char (*names)[FS_MAX_NAME]; // Puntero a un bloque dinámico de nombres
    CSample* samples;           // Arreglo dinámico de muestras de audio
    bool* loaded;               // Arreglo dinámico de estados de carga
    int count;                  // Cantidad total de archivos encontrados
    int selected;
    int scroll;
};

// Ubicamos los descriptores de las listas en la sección de datos estáticos
static AudioList musicList __attribute__((aligned(16)));
static AudioList sfxList __attribute__((aligned(16)));

// ============================================================================
// GESTIÓN DINÁMICA DE MEMORIA (SE EJECUTA AL INICIO / CIERRE DE LA APP)
// ============================================================================

void unload_and_clear_dynamic(AudioList* list) {
    if (!list) return;
    
    // 1. Descargar las muestras de audio que estaban en RAM
    if (list->samples && list->loaded) {
        for (int i = 0; i < list->count; i++) {
            if (list->loaded[i]) {
                list->samples[i].setActive(false);
                list->samples[i].close();
            }
        }
    }

    // 2. Liberar la memoria asignada dinámicamente
    if (list->names)   { free(list->names);   list->names = NULL; }
    if (list->samples) { free(list->samples); list->samples = NULL; }
    if (list->loaded)  { free(list->loaded);  list->loaded = NULL; }

    list->count = 0;
    list->selected = 0;
    list->scroll = 0;
}

void scan_and_preload_dynamic(const char* path, AudioList* list, bool preload) {
    FS_DIR dir;
    FS_DIRENT ent;
    
    list->count = 0;
    list->names = NULL;
    list->samples = NULL;
    list->loaded = NULL;
    
    if (fs_opendir(&dir, path) == 0) {
        // Primer paso: Contar cuántos archivos válidos hay para reservar la memoria exacta
        int allocCount = 0;
        while (fs_readdir(&dir, &ent) == 0) {
            if (!ent.is_dir && strstr(ent.name, ".wav")) {
                allocCount++;
            }
        }
        
        if (allocCount > 0) {
            list->count = allocCount;
            // Asignamos los bloques de memoria dinámicos con el tamaño exacto
            list->names   = (char (*)[FS_MAX_NAME])malloc(allocCount * FS_MAX_NAME);
            list->samples = (CSample*)malloc(allocCount * sizeof(CSample));
            list->loaded  = (bool*)malloc(allocCount * sizeof(bool));
            
            // Inicializar por seguridad para evitar basura en memoria
            memset(list->names, 0, allocCount * FS_MAX_NAME);
            memset(list->loaded, 0, allocCount * sizeof(bool));
            // Invocar constructor por defecto de C++ en el bloque asignado por malloc para los CSample
            for (int i = 0; i < allocCount; i++) {
                new (&list->samples[i]) CSample();
            }

            // Segundo paso: Resetear el directorio y rellenar los datos
            fs_closedir(&dir);
            if (fs_opendir(&dir, path) == 0) {
                int index = 0;
                while (fs_readdir(&dir, &ent) == 0 && index < allocCount) {
                    if (!ent.is_dir && strstr(ent.name, ".wav")) {
                        strncpy(list->names[index], ent.name, FS_MAX_NAME - 1);
                        list->names[index][FS_MAX_NAME - 1] = '\0';
                        
                        if (preload) {
                            char fullPath[512] __attribute__((aligned(16)));
                            snprintf(fullPath, sizeof(fullPath), "%s/%s", path, ent.name);
                            if (list->samples[index].Load(fullPath)) {
                                list->loaded[index] = true;
                            }
                        }
                        index++;
                    }
                }
            }
        }
        fs_closedir(&dir);
    }
}

void init_audio_test_resources() {
    unload_and_clear_dynamic(&musicList);
    unload_and_clear_dynamic(&sfxList);
    scan_and_preload_dynamic("music", &musicList, false); // Streaming
    scan_and_preload_dynamic("sfx", &sfxList, true);      // Precarga a RAM
}

void free_audio_test_resources() {
    mixer.stopMusic();
    mixer.stopAll();
    unload_and_clear_dynamic(&musicList);
    unload_and_clear_dynamic(&sfxList);
}

// ============================================================================
// BUCLE DE EJECUCIÓN (Bucle puro sin lecturas a disco e interfaz idéntica)
// ============================================================================

void run_audio_test() {
    mixer.stopMusic();
    mixer.stopAll();

    musicList.selected = 0; musicList.scroll = 0;
    sfxList.selected = 0;   sfxList.scroll = 0;

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
                char path[512] __attribute__((aligned(16)));
                snprintf(path, sizeof(path), "music/%s", cur->names[cur->selected]);
                mixer.playMusic(path, true);
            } else {
                if (cur->loaded && cur->loaded[cur->selected]) {
                    mixer.playChannel(&cur->samples[cur->selected], false, volume);
                }
            }
        }

        // --- INTERFAZ GRÁFICA ORIGINAL AL 100% ---
        fill_vertical_gradient(logic, color_rgb(10, 15, 30), color_rgb(20, 30, 50));
        fontsize(8, 8); 
        print(15, 10, "AUDIO EXPLORER", color_rgb(0, 255, 255));
        fill_rect(logic, 15, 22, 290, 1, color_rgb(0, 255, 255));

        for (int col = 0; col < 2; col++) {
            AudioList* lst = (col == 0) ? &musicList : &sfxList;
            int startX = (col == 0) ? 20 : 170;
            for (int i = 0; i < LIST_VISIBLE; i++) {
                int idx = lst->scroll + i;
                if (idx < lst->count && lst->names) {
                    int py = 55 + (i * 18);
                    if (activeCol == col && idx == lst->selected) {
                        fill_rect(logic, startX - 4, py - 2, 135, 14, color_rgb(40, 60, 120));
                        int nLen = strlen(lst->names[idx]);
                        if (nLen > 15) {
                            marqueeTimer += 0.15f;
                            if (marqueeTimer > 1.0f) { marqueeTimer = 0.0f; charOffset++; if (charOffset > nLen - 5) charOffset = 0; }
                            char db[17]; strncpy(db, &lst->names[idx][charOffset], 16); db[16] = '\0';
                            print(startX, py, db, color_rgb(255, 255, 255));
                        } else print(startX, py, lst->names[idx], color_rgb(255, 255, 255));
                    } else {
                        char sn[16];
                        if (strlen(lst->names[idx]) > 15) {
                            strncpy(sn, lst->names[idx], 12); sn[12] = '\0'; strcat(sn, "...");
                            print(startX, py, sn, color_rgb(130, 130, 140));
                        } else print(startX, py, lst->names[idx], color_rgb(130, 130, 140));
                    }
                }
            }
            // Scrollbars proporcionales dinámicas
            if (lst->count > LIST_VISIBLE) {
                int scrollX = (col == 0) ? 158 : 312;
                int barH = LIST_VISIBLE * 18 - 4;
                fill_rect(logic, scrollX, 55, 2, barH, color_rgb(40, 40, 50));
                int cursorH = (LIST_VISIBLE * barH) / lst->count;
                int cursorY = 55 + (lst->scroll * (barH - (cursorH < 6 ? 6 : cursorH))) / (lst->count - LIST_VISIBLE);
                fill_rect(logic, scrollX, cursorY, 2, (cursorH < 6 ? 6 : cursorH), (activeCol == col) ? color_rgb(0, 255, 255) : color_rgb(80, 80, 90));
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
}

#endif
