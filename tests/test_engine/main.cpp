#include <engine/Engine.h>
#include <input/Input.h>
#include <input/inputTypes.h>

#include "snes_png.h"

// =======================
// Variables Globales
// =======================

SDL_Surface *img = NULL;
SDL_Surface *img_scaled = NULL;
SDL_Surface *set = NULL;

gfxFont font;

int markerX = -100;
int markerY = -100;

const char* lastButton = "NONE";


// =======================
// Cargar recursos
// =======================

void load_surface()
{
    img = load_texture_from_mem((u8 *)snes_data, snes_size);

    // Escalar UNA sola vez (muy importante en PSP)
    img_scaled = rotozoom_create(img, 0, 0.8);

    // Crear marcador
    set = create_surface(13, 13, SDL_SWSURFACE);
    fill_radial_gradient(set, color_rgb(20, 100, 100), color_rgb(0, 255, 100));
    apply_alpha(set, 180);

    font.init();
}


// =======================
// Actualizar lógica
// =======================

void update_game()
{
    markerX = -100;
    markerY = -100;
    lastButton = "NONE";

    InputButton buttons[] = {
        BUTTON_LEFT, BUTTON_RIGHT, BUTTON_UP, BUTTON_DOWN,
        BUTTON_A, BUTTON_B, BUTTON_X, BUTTON_Y,
        BUTTON_L1, BUTTON_R1,
        BUTTON_START, BUTTON_SELECT
    };

    for (int i = 0; i < 12; i++)
    {
        if (Input::isDown(0, buttons[i]))
        {
            switch (buttons[i])
            {
                case BUTTON_LEFT:
                    markerX = 62; markerY = 123;
                    lastButton = "LEFT";
                    break;

                case BUTTON_RIGHT:
                    markerX = 92; markerY = 123;
                    lastButton = "RIGHT";
                    break;

                case BUTTON_UP:
                    markerX = 78; markerY = 110;
                    lastButton = "UP";
                    break;

                case BUTTON_DOWN:
                    markerX = 78; markerY = 135;
                    lastButton = "DOWN";
                    break;

                case BUTTON_A:
                    markerX = 250; markerY = 122;
                    lastButton = "A";
                    break;

                case BUTTON_B:
                    markerX = 225; markerY = 140;
                    lastButton = "B";
                    break;

                case BUTTON_X:
                    markerX = 226; markerY = 104;
                    lastButton = "X";
                    break;

                case BUTTON_Y:
                    markerX = 201; markerY = 123;
                    lastButton = "Y";
                    break;

                case BUTTON_L1:
                    markerX = 70; markerY = 60;
                    lastButton = "L1";
                    break;

                case BUTTON_R1:
                    markerX = 232; markerY = 60;
                    lastButton = "R1";
                    break;

                case BUTTON_START:
                    markerX = 158; markerY = 128;
                    lastButton = "START";
                    break;

                case BUTTON_SELECT:
                    markerX = 130; markerY = 128;
                    lastButton = "SELECT";
                    break;
            }
            break; // Solo detecta un botón por frame
        }
    }
}


// =======================
// Render
// =======================

void render_game()
{
    cls();

    // Dibujar imagen escalada
    rotozoom_set_position(img_scaled, 160, 120);

    // Texto
    font.draw(logic, LARGE_FONT, 60, 20, "JOYSTICK TEST");
    font.draw(logic, LARGE_FONT, 120, 180, lastButton);

    // Dibujar marcador
    if (markerX >= 0)
    {
        draw_surface(set, markerX, markerY);
    }

    Render();
    Fps_sincronizar(30);
}


// =======================
// Main
// =======================

int main(int argc, char **argv)
{
    if (Init_Sistem("libGPP-Engine Input Test") != 0)
        return 1;

    if (Set_Video() != 0)
        return 1;

    Input::init();
    load_surface();

    while (1)
    {
        Input::update();
        update_game();
        render_game();
    }

    return 0;
}