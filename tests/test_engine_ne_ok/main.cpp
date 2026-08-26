#include <engine/engine.h>
#include <input/Input.h>
#include <audio/mixer.h>
#include <audio/sample.h>
#include <font/gfxFont.h>
#include <font/sysfont.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nes_emu.h"

#include <system/gpp_time.h>

// 1. Cabeceras de UI y Gestor de Layout HUD unificado
#include "interfaces.h" 
#include "layout_mgr.h"

// 2. Cabeceras de los módulos de pruebas nativos de la Suite
#include "test_graficos.h"
#include "test_audio.h"
void run_nes_emulator();

// ========================================================================
// VARIABLES GLOBALES (Definición real en memoria para el Linker)
// ========================================================================
extern SDL_Surface *logic;
gfxFont font;

// Definiciones de Audio (Buscadas por test_audio.h y la UI)
Cmixer mixer;
int global_volume = 64;
AudioList musicList;
AudioList sfxList;
CSample* sfxPreloadedBank = NULL;

// Definiciones de Gráficos (Buscadas por test_graficos.h y la UI)
SDL_Surface* surf_gradientes[4] = {NULL, NULL, NULL, NULL};
SDL_Surface* surf_mix[4]        = {NULL, NULL, NULL, NULL};

// Variables de Control del Marquee Deslizante Inferior
float marqueeX = 320.0f;
const char *infoText = "[A] SELECT   [B] BACK   [X] OPTIONS   -   GPP-ENGINE TEST SUITE NATIVA";

// Reloj de Sistema y Contexto de la UI
GPP_DateTime systemClock;
Uint32 lastClockUpdate = 0;
UINgContext uiContext = { FOCUS_CATEGORIES, 0, 0, 0, 0 };

// ========================================================================
// CAPA DE COMPATIBILIDAD DE ASSETS NATIVOS (Carga & Escalado)
// ========================================================================
typedef struct icon_t { SDL_Surface *icon; } icon_t;

icon_t *quickLoad(SDL_Surface * sheet, int x, int y, int w, int h, float zoom) {
    icon_t *temp = (icon_t *) malloc(sizeof(icon_t));
    if (!temp) return NULL;
    temp->icon = cut_surface(sheet, x, y, w, h);
    SDL_Surface *scaled = rotozoom_create(temp->icon, 0.0f, zoom);
    SDL_FreeSurface(temp->icon);
    temp->icon = scaled;
    return temp;
}

void cargar_listas_fijas() {
    FS_DIR dir; FS_DIRENT ent; char pathBuffer[256];
    musicList.count = 0; musicList.selected = 0; musicList.scroll = 0; musicList.names = NULL;
    sfxList.count = 0;   sfxList.selected = 0;   sfxList.scroll = 0;   sfxList.names = NULL;

    if (fs_opendir(&dir, "music") == 0) {
        int totalFiles = 0;
        while (fs_readdir(&dir, &ent) == 0) { if (!ent.is_dir && strstr(ent.name, ".wav")) totalFiles++; }
        fs_closedir(&dir);
        if (totalFiles > 0) {
            musicList.count = totalFiles;
            musicList.names = (char (*)[MAX_NAME_LEN])malloc(totalFiles * MAX_NAME_LEN);
            if (musicList.names != NULL) {
                memset(musicList.names, 0, totalFiles * MAX_NAME_LEN);
                if (fs_opendir(&dir, "music") == 0) {
                    int index = 0;
                    while (fs_readdir(&dir, &ent) == 0 && index < totalFiles) {
                        if (!ent.is_dir && strstr(ent.name, ".wav")) { strncpy(musicList.names[index], ent.name, MAX_NAME_LEN - 1); index++; }
                    }
                    fs_closedir(&dir);
                }
            }
        }
    }
    if (fs_opendir(&dir, "sfx") == 0) {
        int totalFiles = 0;
        while (fs_readdir(&dir, &ent) == 0) { if (!ent.is_dir && strstr(ent.name, ".wav")) totalFiles++; }
        fs_closedir(&dir);
        if (totalFiles > 0) {
            sfxList.count = totalFiles;
            sfxList.names = (char (*)[MAX_NAME_LEN])malloc(totalFiles * MAX_NAME_LEN);
            sfxPreloadedBank = new CSample[totalFiles];
            if (sfxList.names != NULL && sfxPreloadedBank != NULL) {
                memset(sfxList.names, 0, totalFiles * MAX_NAME_LEN);
                if (fs_opendir(&dir, "sfx") == 0) {
                    int index = 0;
                    while (fs_readdir(&dir, &ent) == 0 && index < totalFiles) {
                        if (!ent.is_dir && strstr(ent.name, ".wav")) {
                            strncpy(sfxList.names[index], ent.name, MAX_NAME_LEN - 1);
                            snprintf(pathBuffer, sizeof(pathBuffer), "sfx/%s", ent.name);
                            sfxPreloadedBank[index].Load(pathBuffer);
                            index++;
                        }
                    }
                    fs_closedir(&dir);
                }
            }
        }
    }
}

void inicializar_superficies_graficos() {
    u32 negro = color_rgb(0, 0, 0), blanco = color_rgb(255, 255, 255), rojo = color_rgb(255, 0, 0), verde = color_rgb(0, 255, 0), azul = color_rgb(0, 0, 255);
    u32 colores_gradientes[] = {blanco, rojo, verde, azul};
    for(int i = 0; i < 4; i++) {
        surf_gradientes[i] = create_surface(240, 30, 0);
        if (surf_gradientes[i]) fill_horizontal_gradient(surf_gradientes[i], negro, colores_gradientes[i]);
    }
    u32 colores_mix[] = {rojo, verde, azul, blanco};
    for(int i = 0; i < 4; i++) {
        surf_mix[i] = create_surface(220, 32, 0);
        if (surf_mix[i]) fill_horizontal_gradient(surf_mix[i], negro, colores_mix[i]);
    }
}

// ========================================================================
// CONTROL PRINCIPAL DE EJECUCIÓN (Bucle Core)
// ========================================================================
int main(int argc, char **argv) {
    (void)argc; (void)argv;

    // Inicialización de subsistemas nativos libGPP
    if (Init_Sistem("GPP Pro Suite - Reploid Edition") < 0) return 1;
    Set_Video();
    
    Input::init();
    font.init();
    mixer.init(44100, 2, 2048);

    cargar_listas_fijas();
    inicializar_superficies_graficos();

    // Setup del temporizador y fecha del sistema
    GPP_GetSystemDateTime(&systemClock);
    lastClockUpdate = SDL_GetTicks();

    // Efectos de sonido de la interfaz de usuario
    CSample sfxMove, sfxPush;

    //startup();

    sfxMove.Load("sfx/button.wav");
    sfxPush.Load("sfx/push.wav");

    // Carga de la Spritesheet de íconos de la suite
    SDL_Surface *sheet = load_img("gfx/icon.png");
    
    mixer.setMasterVolume(global_volume);
    mixer.playMusic("music/music.wav", true);
    
    // Extracción de sub-surfaces con escalado (Formato optimizado para el Grid derecho)
    icon_t *icons[8];
    icons[0] = quickLoad(sheet, 35, 5, 190, 200, 0.12);
    icons[1] = quickLoad(sheet, 250, 5, 190, 200, 0.12);
    icons[2] = quickLoad(sheet, 460, 5, 190, 200, 0.12);
    icons[3] = quickLoad(sheet, 680, 5, 190, 200, 0.12);
    icons[4] = quickLoad(sheet, 250, 230, 190, 210, 0.12);
    icons[5] = quickLoad(sheet, 35, 230, 190, 210, 0.12);
    icons[6] = quickLoad(sheet, 460, 240, 190, 200, 0.12);
    icons[7] = quickLoad(sheet, 680, 240, 190, 200, 0.12);

    // Mapeo del árbol jerárquico del menú Reploid
// Mapeo del árbol jerárquico del menú Reploid
// ========================================================================
// ÁRBOL JERÁRQUICO CORREGIDO (Asignación real de la función)
// ========================================================================
CategoryNode menu_tree[MAX_CATEGORIES] = {
    { "SYSTEM",   {{"CPU Stress", icons[6]->icon, NULL, STATUS_READY, 0.0f}, {"Credits", icons[7]->icon, NULL, STATUS_READY, 0.0f}}, 2 },
    { "AUDIO",    {{"Audio Core", icons[1]->icon, run_audio_test, STATUS_READY, 0.0f}}, 1 },
    { "VIDEO",    {
        {"Graphics", icons[0]->icon, run_graficos_test, STATUS_READY, 0.0f}, 
        {"Sprites", icons[4]->icon, NULL, STATUS_READY, 0.0f}, 
        {"NES Emulator", icons[5]->icon, run_nes_emulator, STATUS_READY, 0.0f} // <-- ¡AHORA SÍ! Cambiado el nombre y asignado el callback
    }, 3 },
    { "CONTROLS", {{"Input Pad", icons[2]->icon, NULL, STATUS_READY, 0.0f}}, 1 }
};
    int total_active_categories = 4;

    int d_l = 0, u_l = 0, l_l = 0, r_l = 0, a_l = 0;
    bool running = true; 

    while (running) {
        Input::update();
        bool d = Input::isPressed(0, BUTTON_DOWN), u = Input::isPressed(0, BUTTON_UP);
        bool l = Input::isPressed(0, BUTTON_LEFT), r = Input::isPressed(0, BUTTON_RIGHT), a = Input::isPressed(0, BUTTON_A);

        bool press_d = (d && !d_l), press_u = (u && !u_l), press_l = (l && !l_l), press_r = (r && !r_l), press_a = (a && !a_l);
        d_l = d; u_l = u; l_l = l; r_l = r; a_l = a;

        // Actualizar estados del reloj interno cada segundo
        Uint32 currentTicks = SDL_GetTicks();
        if (currentTicks - lastClockUpdate >= 1000) {
            GPP_GetSystemDateTime(&systemClock);
            lastClockUpdate = currentTicks;
        }

        // Lógica de navegación adaptativa
        actualizar_navegacion_ui(uiContext, press_u, press_d, press_l, press_r, press_a, total_active_categories, menu_tree, &sfxMove, global_volume);

        CategoryNode* active_cat = &menu_tree[uiContext.selected_category];
        
        // Ejecución de callbacks de test al pulsar botón A
        if (uiContext.focus == FOCUS_MAIN_GRID && press_a) {
            int t_idx = uiContext.selected_test_in_category;
            if (active_cat->sub_tests[t_idx].action != NULL) {
                mixer.playChannel(&sfxPush, 0, global_volume);
                active_cat->sub_tests[t_idx].status = STATUS_RUNNING;
                //bajamos volume si inicia el emulador de nes
                printf("%d\n",t_idx);
                if(t_idx == 2 || t_idx == 20){
                    for(int i=global_volume; i>0; i--){
                        mixer.setMasterVolume(i);
                        SDL_Delay(20);

                    }
                }
                active_cat->sub_tests[t_idx].action();
                SDL_Delay(1500);
                active_cat->sub_tests[t_idx].status = STATUS_PASS;
                mixer.setMasterVolume(global_volume); // Restaurar audio tras el test
            }
        }

        // --- PIPELINE DE RENDERIZADO VISUAL ---
        // 1. Limpieza con gradiente de fondo (Azul profundo de baja luminancia)
        fill_vertical_gradient(logic, color_rgb(8, 12, 22), color_rgb(20, 30, 50));

        // 2. Renderizado del HUD Superior Unificado (Status e información de Línea Magenta)
        render_unified_top_bar(logic, systemClock, marqueeX, infoText);

        // 3. Renderizado de Paneles Modulares Simétricos
        render_left_panel(logic, uiContext, menu_tree, total_active_categories, systemClock.text);
        render_main_grid(logic, uiContext, active_cat);

        // 4. Renderizado de la Consola Inferior Deslizante (Marquee)
        render_unified_marquee(logic, marqueeX, infoText);

        // 5. Presentación de frame y sincronización nativa a 60 FPS
        Render();
        Fps_sincronizar(60);
    }
    // --- LIBERACIÓN DE MEMORIA Y CLEANUP ---
    SDL_FreeSurface(sheet);
    for (int i = 0; i < 8; i++) { 
        if (icons[i]) { 
            if (icons[i]->icon) SDL_FreeSurface(icons[i]->icon); 
            free(icons[i]); 
        } 
    }
    
    if (musicList.names) {
        free(musicList.names);
    }
    if (sfxList.names) {
        free(sfxList.names);
    }
    if (sfxPreloadedBank) {
        delete[] sfxPreloadedBank;
    }
    
    for (int i = 0; i < 4; i++) { 
        if (surf_gradientes[i]) SDL_FreeSurface(surf_gradientes[i]); 
        if (surf_mix[i])        SDL_FreeSurface(surf_mix[i]); 
    }



    return 0;
}
