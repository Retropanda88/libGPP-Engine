#include <engine/engine.h>
#include <input/Input.h>
#include <string.h>
#include <math.h>

#define SCREEN_W 320
#define SCREEN_H 240
#define TOTAL_BTNS 3

// ===============================
// CALLBACKS
// ===============================
typedef void (*Action)();
void onJugar() { printf("PLAY\n"); }
void onOpt()   { printf("SETTINGS\n"); }
void onExit()  { exit(0); }

// ===============================
// ESTRUCTURA
// ===============================
struct UIButton {
    int x, y, w, h;
    const char* text;
    int state;
    bool selected;
    Action onClick;
};

UIButton buttons[TOTAL_BTNS] = {
    {90, 70,  140, 32, "INICIAR FASE", 0, false, onJugar},
    {90, 115, 140, 32, "CONFIGURAR",   0, false, onOpt},
    {90, 160, 140, 32, "ABANDONAR",    0, false, onExit}
};

int selected_index = 0;
bool up_last = false, down_last = false, a_last = false;

// ===============================
// DIBUJO ASIMÉTRICO (TRAPECIO)
// ===============================
void draw_trapezoid(SDL_Surface* s, int x, int y, int w, int h, u32 color) {
    int slant = 15; // Grado de inclinación
    
    // Cuerpo rectangular desplazado
    fill_rect(s, x + slant, y, w - slant, h, color);
    
    // Triángulo lateral para crear la inclinación
    fill_triangle_fast(s, 
        x + slant, y,       // Punta arriba
        x, y + h,           // Punta izquierda
        x + slant, y + h,   // Esquina interna inferior
        color);
}

// ===============================
// LÓGICA
// ===============================
void UI_Update() {
    bool up   = Input::isPressed(0, BUTTON_UP);
    bool down = Input::isPressed(0, BUTTON_DOWN);
    bool a    = Input::isPressed(0, BUTTON_A);

    if (down && !down_last) selected_index = (selected_index + 1) % TOTAL_BTNS;
    if (up && !up_last)     selected_index = (selected_index - 1 + TOTAL_BTNS) % TOTAL_BTNS;

    bool click = (a && !a_last);
    down_last = down; up_last = up; a_last = a;

    for (int i = 0; i < TOTAL_BTNS; i++) {
        buttons[i].selected = (i == selected_index);
        if (buttons[i].selected) {
            if (a) {
                buttons[i].state = 2;
                if (click && buttons[i].onClick) buttons[i].onClick();
            } else buttons[i].state = 1;
        } else buttons[i].state = 0;
    }
}

// ===============================
// RENDERIZADO ASIMÉTRICO
// ===============================
void UIButton_Render(SDL_Surface* s, UIButton* b) {
    u32 c_body, c_border;
    int shift_x = 0;

    if (b->state == 2) {      // CLICK
        c_body = color_rgb(255, 255, 255);
        c_border = color_rgb(200, 200, 200);
        shift_x = 5; // Desplazamiento lateral al pulsar
    } else if (b->state == 1) { // SELECT
        c_body = color_rgb(0, 80, 150);
        c_border = color_rgb(0, 255, 255);
    } else {                  // NORMAL
        c_body = color_rgb(20, 20, 30);
        c_border = color_rgb(60, 60, 80);
    }

    // 1. Borde exterior (2px más grande)
    draw_trapezoid(s, b->x - 2 + shift_x, b->y - 2, b->w + 4, b->h + 4, c_border);
    
    // 2. Cuerpo
    draw_trapezoid(s, b->x + shift_x, b->y, b->w, b->h, c_body);

    // 3. Detalle: línea de fin de botón
    fill_rect(s, b->x + b->w - 4 + shift_x, b->y + 4, 2, b->h - 8, c_border);

    // 4. Texto
    fontsize(8, 8);
    u32 c_txt = (b->state == 2) ? color_rgb(0,0,0) : color_rgb(255,255,255);
    print(b->x + 25 + shift_x, b->y + 12, b->text, c_txt);
}

void UI_Render(SDL_Surface* s) {
    for (int i = 0; i < TOTAL_BTNS; i++) UIButton_Render(s, &buttons[i]);
}

// ===============================
// MAIN
// ===============================
int main(int argc, char **argv) {
    if (Init_Sistem("Engine Custom UI") < 0) return 1;
    if (Set_Video() < 0) return 1;

    Input::init();
    SDL_Surface *screen = SDL_GetVideoSurface();
    SDL_Surface *bg = create_surface(SCREEN_W, SCREEN_H, 32);
    fill_vertical_gradient(bg, color_rgb(5, 5, 15), color_rgb(20, 30, 60));

    while (1) {
        Input::update();
        UI_Update();
        draw_surface(bg, 0, 0);
        UI_Render(screen);
        Render();
        Fps_sincronizar(60);
    }
    return 0;
}
