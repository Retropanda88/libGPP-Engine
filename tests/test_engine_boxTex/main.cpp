#include <engine/engine.h>
#include <input/Input.h>
#include <string.h>

#define SCREEN_W 320
#define SCREEN_H 240

// --- VARIABLES DE CONTROL ---
float diag_w = 0;
float diag_h = 2;
int text_counter = 0;
int frame_delay = 0;
bool is_dialog_active = true;

const char* char_name = "DRACULA";
const char* char_msg  = "Que es un hombre? Sino una miserable pila de secretos.";

void Draw_MegaMan_Dialogue(SDL_Surface* logic) {
    const int FINAL_W = 260;
    const int FINAL_H = 75; 
    const int CENTER_X = SCREEN_W / 2; 
    const int CENTER_Y = 175;
    const int TEXT_SPEED = 2; 

    if (is_dialog_active) {
        if (diag_w < (float)FINAL_W - 0.5f) {
            diag_w += ((float)FINAL_W - diag_w) * 0.25f;
        } else {
            diag_w = (float)FINAL_W;
            if (diag_h < (float)FINAL_H - 0.5f) {
                diag_h += ((float)FINAL_H - diag_h) * 0.25f;
            } else {
                diag_h = (float)FINAL_H;
            }
        }
    }

    int cx = CENTER_X - (int)(diag_w / 2);
    int cy = CENTER_Y - (int)(diag_h / 2);

    if (diag_w > 5) {
        // 🎨 NUEVA PALETA DE COLORES
        u32 cl_bg     = color_rgb(200, 200, 210); // Gris suave (claro)
        u32 cl_brd    = color_rgb(255, 0, 0);     // Rojo puro para el borde
        u32 cl_txt    = color_rgb(20, 20, 30);    // Texto oscuro para contraste sobre gris
        u32 cl_name   = color_rgb(180, 0, 0);     // Nombre en rojo oscuro
        
        // Dibujar el panel principal (Gris suave)
        fill_rect(logic, cx, cy, (int)diag_w, (int)diag_h, cl_bg);
        
        // Bordes Rojos
        draw_line_fast(logic, cx, cy, cx + (int)diag_w, cy, cl_brd);
        draw_line_fast(logic, cx, cy + (int)diag_h, cx + (int)diag_w, cy + (int)diag_h, cl_brd);
        draw_line_fast(logic, cx, cy, cx, cy + (int)diag_h, cl_brd);
        draw_line_fast(logic, cx + (int)diag_w, cy, cx + (int)diag_w, cy + (int)diag_h, cl_brd);

        if (diag_h >= (float)FINAL_H) {
            fontsize(8, 8); 

            // Nombre del personaje
            print(cx + 5, cy - 12, char_name, cl_brd);

            int message_len = (int)strlen(char_msg);
            if (text_counter < message_len) {
                frame_delay++;
                if (frame_delay >= TEXT_SPEED) {
                    text_counter++;
                    frame_delay = 0;
                }
            }

            // Lógica de salto de línea (máximo 28-30 caracteres)
            char line1[64] = {0};
            char line2[64] = {0};
            int limit = 30; 
            
            for (int i = 0; i < text_counter; i++) {
                if (i < limit) {
                    line1[i] = char_msg[i];
                    line1[i+1] = '\0';
                } else {
                    line2[i - limit] = char_msg[i];
                    line2[i - limit + 1] = '\0';
                }
            }

            // Texto en color oscuro para que sea legible sobre el gris suave
            print(cx + 10, cy + 20, line1, cl_txt);
            if (text_counter > limit) {
                print(cx + 10, cy + 35, line2, cl_txt);
            }

            // Indicador de fin parpadeante
            if (text_counter == message_len) {
                static int blink = 0;
                if ((blink++ / 15) % 2 == 0) {
                    fill_triangle_fast(logic, 
                        cx + (int)diag_w - 15, cy + (int)diag_h - 12,
                        cx + (int)diag_w - 5,  cy + (int)diag_h - 12,
                        cx + (int)diag_w - 10, cy + (int)diag_h - 5, cl_brd);
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    if (Init_Sistem("Azul & Gris Dialog") < 0) return 1;
    if (Set_Video() < 0) return 1;

    Input::init();
    SDL_Surface *logic = SDL_GetVideoSurface();

    // 🌊 FONDO AZUL (Azul fuerte a Azul suave)
    SDL_Surface *bg = create_surface(SCREEN_W, SCREEN_H, 32);
    fill_vertical_gradient(bg, color_rgb(0, 0, 100), color_rgb(100, 150, 255));

    while (1) {
        Input::update();

        if (Input::isPressed(0, BUTTON_A) && text_counter == (int)strlen(char_msg)) {
            diag_w = 0; diag_h = 2; text_counter = 0;
        }

        draw_surface(bg, 0, 0);
        Draw_MegaMan_Dialogue(logic);

        Render();
        Fps_sincronizar(60);
    }
    return 0;
}
