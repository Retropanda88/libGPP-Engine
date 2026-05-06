#include <engine/engine.h>
#include <input/Input.h>
#include <audio/mixer.h>
#include <audio/sample.h>
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_TESTS 8
#define VISIBLE_ITEMS 4 

// ==========================================================
// ESTRUCTURAS
// ==========================================================
typedef struct icon { SDL_Surface *icon; } icon;
typedef void (*TestAction)(void);

struct TestItem {
    const char* title;
    SDL_Surface* ico;
    TestAction action;
    float animOffset;
};

// ==========================================================
// FUNCIONES DE DIBUJO Y CARGA
// ==========================================================
void render_background_gradient(SDL_Surface* s, u32 colorTop, u32 colorBottom) {
    int r1 = (colorTop >> 16) & 0xFF;
    int g1 = (colorTop >> 8) & 0xFF;
    int b1 = colorTop & 0xFF;

    int r2 = (colorBottom >> 16) & 0xFF;
    int g2 = (colorBottom >> 8) & 0xFF;
    int b2 = colorBottom & 0xFF;

    for (int i = 0; i < s->h; i++) {
        float f = (float)i / (float)s->h;
        u8 r = (u8)(r1 + (r2 - r1) * f);
        u8 g = (u8)(g1 + (g2 - g1) * f);
        u8 b = (u8)(b1 + (b2 - b1) * f);
        fill_rect(s, 0, i, s->w, 1, color_rgb(r, g, b));
    }
}

icon* quickLoad(SDL_Surface* sheet, int x, int y) {
    icon* temp = (icon*)malloc(sizeof(icon));
    temp->icon = cut_surface(sheet, x, y, 240, 240);
    SDL_Surface* scaled = rotozoom_create(temp->icon, 0.0f, 0.12f);
    SDL_FreeSurface(temp->icon);
    temp->icon = scaled;
    return temp;
}

void renderItem(SDL_Surface* screen, TestItem* it, int x, int y, bool sel, bool prs) {
    int w = 210, h = 36;
    float target = sel ? 15.0f : 0.0f;
    it->animOffset += (target - it->animOffset) * 0.2f;
    int cx = x + (int)it->animOffset;

    u32 bg = prs ? color_rgb(255,255,255) : (sel ? color_rgb(40,60,120) : color_rgb(20,20,35));
    u32 acc = sel ? color_rgb(0, 255, 255) : color_rgb(60, 60, 80);

    fill_rect(screen, cx + 12, y, w - 12, h, bg);
    fill_triangle_fast(screen, cx + 12, y, cx, y + h, cx + 12, y + h, bg);
    
    if (it->ico) draw_surface(it->ico, cx - 2, y + (h/2) - (it->ico->h/2));
    
    print(cx + 42, y + 13, it->title, sel ? color_rgb(255,255,255) : color_rgb(160,160,170));
    fill_rect(screen, cx + w - 3, y + 4, 2, h - 8, acc);
}

// ==========================================================
// MAIN
// ==========================================================
int main(int argc, char **argv) {
    if (Init_Sistem("GPP UI Suite Pro") < 0) return 1;
    Set_Video(); 
    Input::init();
    
    Cmixer mixer; 
    mixer.init(44100, 2, 2048);
    mixer.playMusic("music/music.wav", true);
    
    // SFX: Navegación y Selección
    CSample sfxMove; 
    sfxMove.Load("sfx/button.wav");
    
    CSample sfxPush;
    sfxPush.Load("sfx/push.wav"); // Nuevo sonido de selección

    SDL_Surface* screen = SDL_GetVideoSurface();
    SDL_Surface* sheet = load_img("icon.png");

    icon* icons[MAX_TESTS];
    for(int i=0; i<4; i++) icons[i] = quickLoad(sheet, 10 + (i*255), 10);
    for(int i=0; i<4; i++) icons[i+4] = quickLoad(sheet, 10 + (i*255), 265);

    TestItem tests[MAX_TESTS] = {
        {"GRAFICOS",   icons[0]->icon, NULL, 0}, {"AUDIO",      icons[1]->icon, NULL, 0},
        {"INPUT PAD",  icons[2]->icon, NULL, 0}, {"FUENTES",    icons[3]->icon, NULL, 0},
        {"X-ENGINE",   icons[4]->icon, NULL, 0}, {"SPRITES",    icons[5]->icon, NULL, 0},
        {"STRESS CPU", icons[6]->icon, NULL, 0}, {"MEMORY MAP", icons[7]->icon, NULL, 0}
    };

    int sel = 0, scroll = 0, d_l = 0, u_l = 0, a_l = 0;

    while (1) {
        Input::update();
        bool d = Input::isPressed(0, BUTTON_DOWN);
        bool u = Input::isPressed(0, BUTTON_UP);
        bool a = Input::isPressed(0, BUTTON_A);

        // Navegación (Sonido button.wav)
        if (d && !d_l) { sel = (sel + 1) % MAX_TESTS; mixer.playChannel(&sfxMove, 0, 128); }
        if (u && !u_l) { sel = (sel - 1 + MAX_TESTS) % MAX_TESTS; mixer.playChannel(&sfxMove, 0, 128); }
        
        // Selección (Sonido push.wav)
        if (a && !a_l) { 
            mixer.playChannel(&sfxPush, 0, 128); 
            if(tests[sel].action) tests[sel].action(); 
        }

        d_l = d; u_l = u; a_l = a;

        // Scroll
        if (sel >= scroll + VISIBLE_ITEMS) scroll = sel - VISIBLE_ITEMS + 1;
        if (sel < scroll) scroll = sel;

        // Renderizado
        render_background_gradient(screen, color_rgb(10, 15, 30), color_rgb(35, 55, 95));

        fontsize(16, 16);
        print(20, 15, "TEST SUITE", color_rgb(0, 255, 255));
        fill_rect(screen, 20, 36, 110, 2, color_rgb(0, 255, 255));

        for (int i = 0; i < VISIBLE_ITEMS; i++) {
            int idx = scroll + i;
            if (idx < MAX_TESTS) {
                renderItem(screen, &tests[idx], 40, 55 + (i * 42), (idx == sel), (idx == sel && a));
            }
        }

        // Scrollbar
        int scrollH = 160; 
        int thumbH = scrollH / (MAX_TESTS - VISIBLE_ITEMS + 1); 
        int scrollY = 55 + (scroll * (scrollH - thumbH) / (MAX_TESTS - VISIBLE_ITEMS));
        
        fill_rect(screen, 305, 55, 3, scrollH, color_rgb(30, 35, 60)); 
        fill_rect(screen, 305, scrollY, 3, thumbH, color_rgb(0, 255, 255)); 

        Render(); 
        Fps_sincronizar(60);
    }

    return 0;
}
