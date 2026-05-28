#ifndef INTERFACE_H
#define INTERFACE_H

#include <engine/engine.h>
#include <input/Input.h>
#include <font/gfxFont.h>
#include <font/sysfont.h>
#include <stdlib.h>
#include <string.h>

// Evitar conflictos con declaraciones cruzadas de audio si ya existen
extern Cmixer mixer;               

// ========================================================================
// 1. MACROS DE CONFIGURACIÓN GEOMÉTRICA (Visibles para todo el sistema)
// ========================================================================
#define HUD_SYSTEM_TOP       4
#define HUD_LINE_Y           16

// Panel Izquierdo (Categorías)
#define CAT_PANEL_X          8
#define CAT_PANEL_Y          26
#define CAT_PANEL_W          112
#define CAT_PANEL_H          178  
#define CAT_LINE_HEIGHT      12
#define CAT_MAX_VISIBLE      6

// Panel Derecho Container (Grid)
#define GRID_PANEL_X         126
#define GRID_PANEL_Y         CAT_PANEL_Y
#define GRID_PANEL_W         186
#define GRID_PANEL_H         CAT_PANEL_H

// Slots Internos del Grid
#define GRID_COLS            2
#define GRID_ROWS            3
#define GRID_CELL_W          82
#define GRID_CELL_H          44

#define MAX_TESTS_PER_CATEGORY 12
#define MAX_CATEGORIES         6  

// ========================================================================
// 2. ENUMS Y ENTRADAS DE DATOS DE LA UI
// ========================================================================
enum TestStatus { STATUS_IDLE, STATUS_READY, STATUS_RUNNING, STATUS_PASS, STATUS_FAIL };
enum UIFocus { FOCUS_CATEGORIES, FOCUS_MAIN_GRID };

typedef void (*TestAction)(void); 

struct TestModule {
    const char* title;
    SDL_Surface* ico;
    TestAction action; 
    TestStatus status;
    float animOffset;
};

struct CategoryNode {
    const char* name;
    TestModule sub_tests[MAX_TESTS_PER_CATEGORY];
    int total_tests;
};

struct UINgContext {
    UIFocus focus;
    int selected_category;
    int category_scroll_offset;
    int selected_test_in_category; 
    int grid_row_offset;           
};

// Declaración adelantada (Prototype) para que los renderizadores la conozcan
void draw_advanced_panel(SDL_Surface* s, int x, int y, int w, int h, u32 color_borde, u32 color_fondo, const char* title);

// ========================================================================
// 3. FUNCIONES DE INTERFAZ GRÁFICA MODULAR
// ========================================================================
inline void render_left_panel(SDL_Surface* dest, const UINgContext& ctx, CategoryNode* tree, int total_cats, const char* clock_text) {
    u32 bg_panel = color_rgb(10, 14, 20);
    u32 border_panel = (ctx.focus == FOCUS_CATEGORIES) ? color_rgb(0, 255, 255) : color_rgb(45, 55, 70);
    
    draw_advanced_panel(dest, CAT_PANEL_X, CAT_PANEL_Y, CAT_PANEL_W, CAT_PANEL_H, border_panel, bg_panel, "CATEGORIES");

    for (int i = 0; i < CAT_MAX_VISIBLE; i++) {
        int idx = ctx.category_scroll_offset + i;
        if (idx >= total_cats) break;

        int draw_y = CAT_PANEL_Y + 16 + (i * CAT_LINE_HEIGHT);
        bool is_selected = (idx == ctx.selected_category);
        bool has_focus = (ctx.focus == FOCUS_CATEGORIES);

        if (is_selected) {
            u32 color = has_focus ? color_rgb(255, 230, 0) : color_rgb(0, 180, 240);
            sysfontDrawString(CAT_PANEL_X + 6, draw_y, ">", color);
            sysfontDrawString(CAT_PANEL_X + 14, draw_y, tree[idx].name, color);
        } else {
            sysfontDrawString(CAT_PANEL_X + 14, draw_y, tree[idx].name, color_rgb(130, 135, 145));
        }
    }

    // Barra de scroll vertical del panel izquierdo
    if (total_cats > CAT_MAX_VISIBLE) {
        int track_h = CAT_MAX_VISIBLE * CAT_LINE_HEIGHT;
        int track_y = CAT_PANEL_Y + 16;
        int track_x = CAT_PANEL_X + CAT_PANEL_W - 6;
        fill_rect(dest, track_x, track_y, 2, track_h, color_rgb(25, 30, 40));
        
        int thumb_h = (CAT_MAX_VISIBLE * track_h) / total_cats;
        int thumb_y = track_y + (ctx.category_scroll_offset * (track_h - thumb_h) / (total_cats - CAT_MAX_VISIBLE));
        fill_rect(dest, track_x, thumb_y, 2, thumb_h, color_rgb(0, 255, 255));
    }

    // Bloque estático inferior de Info del Sistema
    int info_start_y = CAT_PANEL_Y + 115;
    fill_rect(dest, CAT_PANEL_X + 6, info_start_y, CAT_PANEL_W - 12, 1, color_rgb(35, 45, 60));

    sysfontDrawString(CAT_PANEL_X + 8, info_start_y + 6, "CPU: 100%", color_rgb(255, 255, 255));
    sysfontDrawString(CAT_PANEL_X + 8, info_start_y + 16, "RAM: OK", color_rgb(255, 255, 255));
    sysfontDrawString(CAT_PANEL_X + 8, info_start_y + 26, "FPS: 60.0", color_rgb(255, 255, 255));
    
    char up_buf[20];
    snprintf(up_buf, sizeof(up_buf), "UP: %s", clock_text);
    sysfontDrawString(CAT_PANEL_X + 8, info_start_y + 36, up_buf, color_rgb(160, 165, 170));
}

inline void render_main_grid(SDL_Surface* dest, const UINgContext& ctx, CategoryNode* current_cat) {
    u32 bg_panel = color_rgb(10, 14, 20);
    u32 border_panel = (ctx.focus == FOCUS_MAIN_GRID) ? color_rgb(255, 255, 0) : color_rgb(45, 55, 70);
    
    draw_advanced_panel(dest, GRID_PANEL_X, GRID_PANEL_Y+5, GRID_PANEL_W, GRID_PANEL_H, border_panel, bg_panel, "TESTS");

    if (current_cat->total_tests == 0) {
        sysfontDrawString(GRID_PANEL_X + 15, GRID_PANEL_Y + 30, "NO TESTS AVAILABLE", color_rgb(110, 115, 120));
        return;
    }

    for (int row = 0; row < GRID_ROWS; row++) {
        int real_row = ctx.grid_row_offset + row;

        for (int col = 0; col < GRID_COLS; col++) {
            int test_idx = (real_row * GRID_COLS) + col;
            if (test_idx >= current_cat->total_tests) break;

            TestModule* it = &current_cat->sub_tests[test_idx];

            int cell_x = GRID_PANEL_X + 8 + (col * (GRID_CELL_W + 6));
            int cell_y = GRID_PANEL_Y + 18 + (row * (GRID_CELL_H + 6));

            bool is_selected = (test_idx == ctx.selected_test_in_category);
            bool has_focus = (ctx.focus == FOCUS_MAIN_GRID);

            float target_offset = (is_selected && has_focus) ? 3.0f : 0.0f;
            it->animOffset += (target_offset - it->animOffset) * 0.2f;
            int final_x = cell_x + (int)it->animOffset;

            u32 bg_color = (is_selected && has_focus) ? color_rgb(20, 35, 70) : color_rgb(15, 20, 28);
            u32 border_color = (is_selected && has_focus) ? color_rgb(0, 255, 255) : color_rgb(45, 50, 60);

            if (is_selected && !has_focus) {
                bg_color = color_rgb(15, 24, 40);
                border_color = color_rgb(0, 120, 180);
            }

            draw_advanced_panel(dest, final_x, cell_y, GRID_CELL_W, GRID_CELL_H, border_color, bg_color, NULL);

            if (it->ico) {
                int ico_y = cell_y + (GRID_CELL_H / 2) - (it->ico->h / 2);
                draw_surface(it->ico, final_x + 6, ico_y);
            }

            u32 status_color = color_rgb(140, 145, 150);
            const char* status_text = "READY";
            
            if (it->status == STATUS_RUNNING) { status_text = "RUN";   status_color = color_rgb(0, 255, 255); }
            if (it->status == STATUS_PASS)    { status_text = "PASS";  status_color = color_rgb(0, 255, 0); }
            if (it->status == STATUS_FAIL)    { status_text = "FAIL";  status_color = color_rgb(255, 0, 0); }

            u32 final_text_color = is_selected ? color_rgb(255, 255, 0) : status_color;
            sysfontDrawString(final_x + 36, cell_y + (GRID_CELL_H / 2) - 4, status_text, final_text_color);
        }
    }

    // Barra de scroll vertical para el Grid de Tests
    int total_rows = (current_cat->total_tests + GRID_COLS - 1) / GRID_COLS;
    if (total_rows > GRID_ROWS) {
        int track_x = GRID_PANEL_X + GRID_PANEL_W - 6;
        int track_h = GRID_PANEL_H - 28;
        int track_y = GRID_PANEL_Y + 18;
        
        fill_rect(dest, track_x, track_y, 2, track_h, color_rgb(25, 30, 40));
        int thumb_h = (GRID_ROWS * track_h) / total_rows;
        int thumb_y = track_y + (ctx.grid_row_offset * (track_h - thumb_h) / (total_rows - GRID_ROWS));
        fill_rect(dest, track_x, thumb_y, 2, thumb_h, color_rgb(255, 255, 0));
    }
}

inline void actualizar_navegacion_ui(UINgContext& ctx, bool u, bool d, bool l, bool r, bool a, int total_cats, CategoryNode* tree, CSample* sfxMove, int volume) {
    if (ctx.focus == FOCUS_CATEGORIES && r && tree[ctx.selected_category].total_tests > 0) {
        ctx.focus = FOCUS_MAIN_GRID;
        ctx.selected_test_in_category = 0;
        ctx.grid_row_offset = 0;
        if (sfxMove) mixer.playChannel(sfxMove, 0, volume);
        return;
    }
    if (ctx.focus == FOCUS_MAIN_GRID && l) {
        ctx.focus = FOCUS_CATEGORIES;
        if (sfxMove) mixer.playChannel(sfxMove, 0, volume);
        return;
    }

    if (ctx.focus == FOCUS_CATEGORIES) {
        if (d && ctx.selected_category < total_cats - 1) {
            ctx.selected_category++;
            if (ctx.selected_category >= ctx.category_scroll_offset + CAT_MAX_VISIBLE) {
                ctx.category_scroll_offset++;
            }
            if (sfxMove) mixer.playChannel(sfxMove, 0, volume);
        }
        if (u && ctx.selected_category > 0) {
            ctx.selected_category--;
            if (ctx.selected_category < ctx.category_scroll_offset) {
                ctx.category_scroll_offset--;
            }
            if (sfxMove) mixer.playChannel(sfxMove, 0, volume);
        }
    }
    else if (ctx.focus == FOCUS_MAIN_GRID) {
        int total_tests = tree[ctx.selected_category].total_tests;
        int current_test = ctx.selected_test_in_category;

        if (d && (current_test + GRID_COLS < total_tests)) {
            ctx.selected_test_in_category += GRID_COLS;
            if ((ctx.selected_test_in_category / GRID_COLS) >= ctx.grid_row_offset + GRID_ROWS) {
                ctx.grid_row_offset++;
            }
            if (sfxMove) mixer.playChannel(sfxMove, 0, volume);
        }
        if (u && (current_test - GRID_COLS >= 0)) {
            ctx.selected_test_in_category -= GRID_COLS;
            if ((ctx.selected_test_in_category / GRID_COLS) < ctx.grid_row_offset) {
                ctx.grid_row_offset--;
            }
            if (sfxMove) mixer.playChannel(sfxMove, 0, volume);
        }
        if (r && ((current_test % GRID_COLS) < GRID_COLS - 1) && (current_test + 1 < total_tests)) {
            ctx.selected_test_in_category++;
            if (sfxMove) mixer.playChannel(sfxMove, 0, volume);
        }
    }
}

#endif
