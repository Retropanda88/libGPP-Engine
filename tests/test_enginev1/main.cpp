#include <engine/engine.h>
#include <input/Input.h>
#include <audio/mixer.h>
#include <audio/sample.h>
#include <font/gfxFont.h>
#include <font/sysfont.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Sistema de tiempo unificado de libGPP
#include <system/gpp_time.h>

#include "test_graficos.h"
#include "test_audio.h"

#define MAX_TESTS 8
#define VISIBLE_ITEMS 4

extern SDL_Surface *logic;
gfxFont font;
Cmixer mixer;

float marqueeX = 320.0f;
const char *infoText = "DESARROLLADO POR RETROPANDA88 - USE ARRIBA/ABAJO PARA NAVEGAR - BOTON A PARA SELECCIONAR TEST";

typedef struct icon { SDL_Surface *icon; } icon;
typedef void (*TestAction) (void);

struct TestItem {
	const char *title;
	SDL_Surface *ico;
	TestAction action;			
	float animOffset;
};

// ========================================================================
// VARIABLES GLOBALES Y ESTRUCTURAS PERSISTENTES
// ========================================================================
AudioList musicList;
AudioList sfxList;
int global_volume = 64; 

// Superficies globales asignadas para el test de gráficos
SDL_Surface* surf_gradientes[4] = {NULL, NULL, NULL, NULL};
SDL_Surface* surf_mix[4]        = {NULL, NULL, NULL, NULL};

// BANCO DE MEMORIA RAM PARA SFX: Protege el hilo de audio de lecturas de almacenamiento
CSample* sfxPreloadedBank = NULL;

// Control de tiempo para el reloj de la UI (Globales para acceso desde otros módulos si es necesario)
GPP_DateTime systemClock;
Uint32 lastClockUpdate = 0;

// ========================================================================
// SISTEMA DE RENDERIZADO GLOBAL OMNIPRESENTE (El secreto del reloj)
// ========================================================================
void GPP_RenderFrame() {
    // 1. Actualizar el RTC del sistema cada 1 segundo de forma centralizada
    Uint32 currentTicks = SDL_GetTicks();
    if (currentTicks - lastClockUpdate >= 1000) {
        GPP_GetSystemDateTime(&systemClock);
        lastClockUpdate = currentTicks;
    }

    // 2. Capa superior (HUD): Renderiza el reloj sutil por encima de CUALQUIER escena activa
    // Posición fija y segura para pantallas CRT y portátiles (x=252, y=6)
    sysfontDrawString(252, 6, systemClock.text, 0x00B4B4FF);

    // 3. Volcado final de hardware original de libGPP
    Render();
}

// Dibuja contenedores con estética mecánica/Reploid usando las primitivas del motor
void draw_advanced_panel(SDL_Surface* s, int x, int y, int w, int h, u32 color_borde, u32 color_fondo) {
    // Fondo base
    fill_rect(s, x + 6, y, w - 6, h, color_fondo);
    fill_triangle_fast(s, x + 6, y, x, y + 6, x + 6, y + 6, color_fondo);
    fill_triangle_fast(s, x, y + 6, x, y + h, x + 6, y + h, color_fondo);
    
    // Líneas estructurales externas
    fill_rect(s, x + 6, y, w - 6, 1, color_borde); // Top
    fill_rect(s, x, y + 6, 1, h - 6, color_borde); // Left
    fill_rect(s, x + w - 1, y, 1, h, color_borde); // Right
    fill_rect(s, x, y + h - 1, w, 1, color_borde); // Bottom
}

// ========================================================================
// FUNCIONES DE SOPORTE DE LA INTERFAZ
// ========================================================================
void render_background_gradient(SDL_Surface * s, u32 colorTop, u32 colorBottom) {
	fill_vertical_gradient(s, colorTop, colorBottom);
}

icon *quickLoad(SDL_Surface * sheet, int x, int y, int w, int h, float zoom) {
	icon *temp = (icon *) malloc(sizeof(icon));
	if (!temp) return NULL;
	temp->icon = cut_surface(sheet, x, y, w, h);
	SDL_Surface *scaled = rotozoom_create(temp->icon, 0.0f, zoom);
	SDL_FreeSurface(temp->icon);
	temp->icon = scaled;
	return temp;
}

void renderItem(SDL_Surface * s, TestItem * it, int x, int y, bool sel, bool prs) {
	int w = 210, h = 36;
	float target = sel ? 15.0f : 0.0f;
	it->animOffset += (target - it->animOffset) * 0.2f;
	int cx = x + (int)it->animOffset;

	u32 bg = prs ? color_rgb(255, 255, 255) : (sel ? color_rgb(40, 60, 120) : color_rgb(20, 20, 35));
	u32 acc = sel ? color_rgb(0, 255, 255) : color_rgb(60, 60, 80);

    // Render del ítem con el panel biselado integrado para mayor consistencia visual
    draw_advanced_panel(s, cx + 12, y, w - 12, h, acc, bg);
	if (it->ico) draw_surface(it->ico, cx - 2, y + (h / 2) - (it->ico->h / 2));

	print(cx + 42, y + 13, it->title, sel ? color_rgb(255, 255, 255) : color_rgb(160, 160, 170));
}

// ========================================================================
// SISTEMA DE ESCANEO DINÁMICO Y PRECARGA TOTAL EN RAM
// ========================================================================
void cargar_listas_fijas() {
    FS_DIR dir;
    FS_DIRENT ent;
    char pathBuffer[256];
    
    musicList.count = 0; musicList.selected = 0; musicList.scroll = 0; musicList.names = NULL;
    sfxList.count = 0;   sfxList.selected = 0;   sfxList.scroll = 0;   sfxList.names = NULL;

    if (fs_opendir(&dir, "music") == 0) {
        int totalFiles = 0;
        while (fs_readdir(&dir, &ent) == 0) {
            if (!ent.is_dir && strstr(ent.name, ".wav")) totalFiles++;
        }
        fs_closedir(&dir);

        if (totalFiles > 0) {
            musicList.count = totalFiles;
            musicList.names = (char (*)[MAX_NAME_LEN])malloc(totalFiles * MAX_NAME_LEN);
            if (musicList.names != NULL) {
                memset(musicList.names, 0, totalFiles * MAX_NAME_LEN);
                if (fs_opendir(&dir, "music") == 0) {
                    int index = 0;
                    while (fs_readdir(&dir, &ent) == 0 && index < totalFiles) {
                        if (!ent.is_dir && strstr(ent.name, ".wav")) {
                            strncpy(musicList.names[index], ent.name, MAX_NAME_LEN - 1);
                            index++;
                        }
                    }
                    fs_closedir(&dir);
                }
            }
        }
    }

    if (fs_opendir(&dir, "sfx") == 0) {
        int totalFiles = 0;
        while (fs_readdir(&dir, &ent) == 0) {
            if (!ent.is_dir && strstr(ent.name, ".wav")) totalFiles++;
        }
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
    u32 negro    = color_rgb(0, 0, 0);
    u32 blanco   = color_rgb(255, 255, 255);
    u32 rojo     = color_rgb(255, 0, 0);
    u32 verde    = color_rgb(0, 255, 0);
    u32 azul     = color_rgb(0, 0, 255);
    
    u32 colores_gradientes[] = {blanco, rojo, verde, azul};
    for(int i = 0; i < 4; i++) {
        surf_gradientes[i] = create_surface(240, 30, 0);
        if (surf_gradientes[i]) {
            fill_horizontal_gradient(surf_gradientes[i], negro, colores_gradientes[i]);
        }
    }

    u32 colores_mix[] = {rojo, verde, azul, blanco};
    for(int i = 0; i < 4; i++) {
        surf_mix[i] = create_surface(220, 32, 0);
        if (surf_mix[i]) {
            fill_horizontal_gradient(surf_mix[i], negro, colores_mix[i]);
        }
    }
}

// ========================================================================
// HILO PRINCIPAL
// ========================================================================
int main(int argc, char **argv) {
	(void)argc; (void)argv;

	if (Init_Sistem("GPP Pro Suite") < 0) return 1;
	Set_Video();
    
	Input::init();
	font.init();
	mixer.init(44100, 2, 2048);

	cargar_listas_fijas();
	inicializar_superficies_graficos();

	// Primera sincronización del reloj antes del ciclo
	GPP_GetSystemDateTime(&systemClock);
	lastClockUpdate = SDL_GetTicks();

	CSample sfxMove, sfxPush;
	sfxMove.Load("sfx/button.wav");
	sfxPush.Load("sfx/push.wav");

	SDL_Surface *sheet = load_img("gfx/icon.png");
	
	startup();
	
	mixer.setMasterVolume(global_volume);
	mixer.playMusic("music/music.wav", true);
	
	icon *icons[MAX_TESTS];
	icons[0] = quickLoad(sheet, 35, 5, 190, 200, 0.2);
	icons[1] = quickLoad(sheet, 250, 5, 190, 200, 0.2);
	icons[2] = quickLoad(sheet, 460, 5, 190, 200, 0.2);
	icons[3] = quickLoad(sheet, 680, 5, 190, 200, 0.2);
	icons[4] = quickLoad(sheet, 250, 230, 190, 210, 0.2);
	icons[5] = quickLoad(sheet, 35, 230, 190, 210, 0.2);
	icons[6] = quickLoad(sheet, 460, 240, 190, 200, 0.2);
	icons[7] = quickLoad(sheet, 680, 240, 190, 200, 0.2);

	TestItem tests[MAX_TESTS] = {
		{"Graphics", icons[0]->icon, run_graficos_test, 0.0f},
		{"Audio", icons[1]->icon, run_audio_test, 0.0f},
		{"Input Pad", icons[2]->icon, NULL, 0.0f},
		{"Fonts", icons[3]->icon, NULL, 0.0f},
		{"Sprites", icons[4]->icon, NULL, 0.0f},
		{"Gfx Engine", icons[5]->icon, NULL, 0.0f},
		{"CPU Stress", icons[6]->icon, NULL, 0.0f},
		{"Credits", icons[7]->icon, NULL, 0.0f}
	};

	int sel = 0, scroll = 0, d_l = 0, u_l = 0, a_l = 0;
	bool running = true; 

	while (running) {
		Input::update();
		bool d = Input::isPressed(0, BUTTON_DOWN);
		bool u = Input::isPressed(0, BUTTON_UP);
		bool a = Input::isPressed(0, BUTTON_A);

		if (d && !d_l) { sel = (sel + 1) % MAX_TESTS; mixer.playChannel(&sfxMove, 0, global_volume); }
		if (u && !u_l) { sel = (sel - 1 + MAX_TESTS) % MAX_TESTS; mixer.playChannel(&sfxMove, 0, global_volume); }

		if (a && !a_l) {
			mixer.playChannel(&sfxPush, 0, global_volume);
			if (tests[sel].action != NULL) {
				tests[sel].action();
				mixer.setMasterVolume(global_volume);
			}
		}
		d_l = d; u_l = u; a_l = a;

		if (sel >= scroll + VISIBLE_ITEMS) scroll = sel - VISIBLE_ITEMS + 1;
		if (sel < scroll) scroll = sel;

		render_background_gradient(logic, color_rgb(10, 15, 30), color_rgb(35, 55, 95));

		// === SECCIÓN DE INTERFAZ ORIGINAL RESTAURADA ===
		fontsize(16, 16);
		print(20, 15, "TEST SUITE", color_rgb(0, 255, 255));

		// Líneas tricolor originales de 210 píxeles intactas
		fill_rect(logic, 20, 36, 210, 2, color_rgb(0, 255, 255));
		fill_rect(logic, 20, 38, 210, 2, color_rgb(120, 66, 255));
		fill_rect(logic, 20, 40, 210, 2, color_rgb(255, 0, 0));

		for (int i = 0; i < VISIBLE_ITEMS; i++) {
			int idx = scroll + i;
			if (idx < MAX_TESTS) {
				renderItem(logic, &tests[idx], 40, 55 + (i * 42), (idx == sel), (idx == sel && a));
			}
		}

		fill_rect(logic, 305, 55, 3, 160, color_rgb(30, 35, 60));
		int sY = 55 + (scroll * (160 - 30) / (MAX_TESTS - VISIBLE_ITEMS));
		fill_rect(logic, 305, sY, 3, 30, color_rgb(0, 255, 255));
		// ==============================================

		fill_rect(logic, 0, 222, 320, 18, color_rgb(0, 0, 0));
		font.draw(logic, MMX_FONT, (int)marqueeX, 225, infoText);

		marqueeX -= 1.3f;
		if (marqueeX < -1000.0f) marqueeX = 320.0f;

		// Usa la envoltura global: calcula tiempo, dibuja el reloj y llama a Render() de hardware
		GPP_RenderFrame();
		Fps_sincronizar(60);
	}

	SDL_FreeSurface(sheet);
	for(int i = 0; i < MAX_TESTS; i++) {
		if(icons[i]) {
			if(icons[i]->icon) SDL_FreeSurface(icons[i]->icon);
			free(icons[i]);
		}
	}

    if (musicList.names) free(musicList.names);
    if (sfxList.names) free(sfxList.names);
    if (sfxPreloadedBank) delete[] sfxPreloadedBank;

    for(int i = 0; i < 4; i++) {
        if (surf_gradientes[i]) SDL_FreeSurface(surf_gradientes[i]);
        if (surf_mix[i])        SDL_FreeSurface(surf_mix[i]);
    }

	return 0;
}
