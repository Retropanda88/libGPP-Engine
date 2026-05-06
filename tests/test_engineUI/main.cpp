#include <engine/engine.h>
#include <input/Input.h>

#define SCREEN_W 320
#define SCREEN_H 240

int main(int argc, char **argv)
{
    if (Init_Sistem("UI Menu Premium") < 0)
        return 1;

    if (Set_Video() < 0)
        return 1;

    Input::init();

    // 🎨 PALETA DE COLORES (Diseño Modern-Retro)
    u32 bgTop        = color_rgb(10, 12, 18);
    u32 bgBottom     = color_rgb(2, 4, 8);
    u32 panelBg      = color_rgb(18, 20, 28);
    u32 panelBorder  = color_rgb(40, 45, 60);
    u32 textNormal   = color_rgb(130, 140, 160);
    u32 textActive   = color_rgb(255, 255, 255);
    u32 accent       = color_rgb(0, 160, 255); // Azul brillante
    u32 selectorShad = color_rgb(30, 50, 80);

    // Preparar superficie de fondo con scanlines
    SDL_Surface *s = create_surface(SCREEN_W, SCREEN_H, 32);
    fill_vertical_gradient(s, bgTop, bgBottom);
    
    // Efecto de líneas de TV vieja (Scanlines)
    for(int i = 0; i < SCREEN_H; i += 3) {
        draw_line_fast(s, 0, i, SCREEN_W, i, color_rgb(5, 5, 10));
    }

    const char *items[] = {
        "START GAME",
        "OPTIONS",
        "LOAD GAME",
        "CREDITS",
        "EXIT"
    };

    int total = 5;
    int selected_index = 0;
    float selector_y = 80; // Posición inicial

    fontsize(8, 8);

    while (1)
    {
        // 🎮 INPUT
        Input::update();

        if (Input::isPressed(0, BUTTON_DOWN))
            selected_index = (selected_index + 1) % total;

        if (Input::isPressed(0, BUTTON_UP)) {
            selected_index--;
            if (selected_index < 0) selected_index = total - 1;
        }

        // 📍 LAYOUT Y LÓGICA SUAVE
        int px = 55, py = 75, pw = 210, ph = 95;
        int base_y = py + 12;
        int target_y = base_y + (selected_index * 16);

        // Tu movimiento fluido original
        selector_y += (target_y - selector_y) * 0.2f;

        // 🖥️ RENDERIZADO
        draw_surface(s, 0, 0); // Dibujar fondo pre-renderizado

        // 1. TÍTULO CON ESTILO
        print(20, 25, "LIBGPP SYSTEM", accent);
        draw_line_fast(s, 20, 37, 80, 37, accent); 
        print(20, 45, "MAIN INTERFACE", textNormal);

        // 2. PANEL PRINCIPAL (Sombra y Caja)
        fill_rect(s, px + 4, py + 4, pw, ph, color_rgb(0, 0, 0)); // Sombra
        fill_rect(s, px, py, pw, ph, panelBg);                   // Fondo panel
        
        // Bordes del panel
        draw_line_fast(s, px, py, px + pw, py, panelBorder);             // Top
        draw_line_fast(s, px, py + ph, px + pw, py + ph, panelBorder);   // Bottom
        draw_line_fast(s, px, py, px, py + ph, panelBorder);             // Left
        draw_line_fast(s, px + pw, py, px + pw, py + ph, panelBorder);   // Right

        // 3. SELECTOR FLUIDO (Cápsula de luz)
        fill_rect(s, px + 5, (int)selector_y - 2, pw - 10, 12, selectorShad); 
        fill_rect(s, px + 5, (int)selector_y - 1, pw - 10, 10, color_rgb(50, 90, 160));
        // Brillo superior del selector
        draw_line_fast(s, px + 6, (int)selector_y - 1, px + pw - 6, (int)selector_y - 1, color_rgb(120, 180, 255));

        // 4. DIBUJAR TEXTOS
        for (int i = 0; i < total; i++)
        {
            int y = base_y + (i * 16);
            
            if (i == selected_index) {
                // Seleccionado: Blanco y un poco desplazado a la derecha
                print(px + 18, y, items[i], textActive);
                // Indicador visual extra (flecha)
                print(px + 6, y, ">", accent);
            } else {
                // No seleccionado: Grisáceo
                print(px + 12, y, items[i], textNormal);
            }
        }

        // 5. BARRA INFERIOR DE ESTADO
        fill_rect(s, 0, 225, SCREEN_W, 15, color_rgb(5, 5, 10));
        print(10, 229, "READY - SELECT AN OPTION", color_rgb(80, 100, 120));

        Render();
        Fps_sincronizar(60);
    }

    return 0;
}
