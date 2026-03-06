#include <engine/engine.h>

#define NUM_TESTS 5

// ==============================
// Variables Globales
// ==============================
int done = 0;
SDL_Surface *temp = NULL;

const char *tests[NUM_TESTS] = {
    "Sound Test",
    "Font Test",
    "Input Test",
    "File System Test",
    "Sprite Test"
};

int selected = 0;


// ==============================
// Dibujar marco de la pantalla
// ==============================
void drawTable()
{
    u32 red = color_rgb(255, 0, 0);

    draw_line_fast(logic, 2, 2, 318, 2, red);
    draw_line_fast(logic, 2, 238, 318, 238, red);
    draw_line_fast(logic, 2, 2, 2, 238, red);
    draw_line_fast(logic, 318, 2, 318, 238, red);

    draw_line_fast(logic, 2, 25, 318, 25, red);
}


// ==============================
// Inicialización
// ==============================
void init()
{
    u32 color1 = color_rgb(255, 0, 0);
    u32 color2 = color_rgb(0, 0, 0);

    temp = create_surface(320, 240, SDL_SWSURFACE);

    fill_radial_gradient(temp, color1, color2);

    Input::init();
}


// ==============================
// Dibujar menú
// ==============================
void drawMenu()
{
    int y = 60;

    for(int i = 0; i < NUM_TESTS; i++)
    {
        u32 color = color_rgb(255,255,255);

        if(i == selected)
        {
            color = color_rgb(0,0,255);
            print_f(60, y, color, "> %s", tests[i]);
        }
        else
        {
            print_f(80, y, color, "%s", tests[i]);
        }

        y += 20;
    }
}


// ==============================
// Update
// ==============================
void update()
{
    Input::update();

    if(Input::isPressed(0, BUTTON_UP))
    {
        selected--;

        if(selected < 0)
            selected = NUM_TESTS - 1;
    }

    if(Input::isPressed(0, BUTTON_DOWN))
    {
        selected++;

        if(selected >= NUM_TESTS)
            selected = 0;
    }

    // salir del programa
    if(Input::isPressed(0, BUTTON_SELECT))
        done = 1;
}


// ==============================
// Dibujar frame
// ==============================
void drawFramewire()
{
    draw_surface(temp,0,0);

    drawTable();

    print_f(70,10,color_rgb(255,255,255),"libGGP-Engine DEMO");

    drawMenu();
}


// ==============================
// Programa principal
// ==============================
int main(int argc, char **argv)
{
    if (Init_Sistem("Engine test") < 0)
        return 1;

    if (Set_Video() < 0)
        return 1;

    init();

    while (!done)
    {
        update();

        drawFramewire();

        Render();
    }

    off_video();
    shoutdown_sistem();

    return 0;
}