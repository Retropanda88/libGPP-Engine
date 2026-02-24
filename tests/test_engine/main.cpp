#include <engine/Engine.h>
#include <input/Input.h>
#include <input/inputTypes.h>

//#include "pad_jpg.h"
#include "snes_png.h"

SDL_Surface *img = NULL;
SDL_Surface *set = NULL;

int markerX = -100;
int markerY = -100;

void load_surface()
{
    img = load_texture_from_mem((u8 *) snes_data, snes_size);

    set = create_surface(13, 13, SDL_SWSURFACE);
    fill_radial_gradient(set, color_rgb(20, 100, 100), color_rgb(0, 255, 100));
    apply_alpha(set, 180);
}

void update_game()
{
    // Reset
    markerX = -100;
    markerY = -100;

    // Flechas
    if (Input::isDown(0, BUTTON_LEFT))
    {
        markerX = 62;
        markerY = 123;
    }
    if (Input::isDown(0, BUTTON_RIGHT))
    {
        markerX = 92;
        markerY = 123;
    }
    if (Input::isDown(0, BUTTON_UP))
    {
        markerX = 78;
        markerY = 110;
    }
    if (Input::isDown(0, BUTTON_DOWN))
    {
        markerX = 78;
        markerY = 135;
    }

    // Botones
    if (Input::isDown(0, BUTTON_A))
    {
        markerX = 250;
        markerY = 122;
    }
    if (Input::isDown(0, BUTTON_B))
    {
       
        markerX = 225;
        markerY = 140;
    }
    if (Input::isDown(0, BUTTON_X))
    {
        markerX = 226;
        markerY = 104;
    }
    if (Input::isDown(0, BUTTON_Y))
    {
        markerX = 201;
        markerY = 123;
    }
    
     if (Input::isDown(0, BUTTON_L1))
    {
        markerX = 70;
        markerY = 60;
    }
    
     if (Input::isDown(0, BUTTON_R1))
    {
        markerX = 232;
        markerY = 60;
    }
    
       if (Input::isDown(0, BUTTON_START))
    {
        markerX = 158;
        markerY = 128;
    }
}

void render_game()
{
	cls();

	// Dibujar imagen
	SDL_Surface *temp = rotozoom_create(img, 0, 0.8);
	rotozoom_set_position(temp, 160, 120);

	// Dibujar texto
	gfxFont font;
	font.init();
	font.draw(logic, LARGE_FONT, 100, 180, "joystick test");
	font.draw(logic, LARGE_FONT, 105, 200, "press button");

	// Dibujar marcador verde
	if (markerX >= 0)
	{
		//fill_rect(logic, markerX, markerY, 20, 20, color_rgb(0, 255, 0));
		draw_surface(set, markerX, markerY);  

	}

	Render();

	rotozoom_destroy(temp);
}

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

#if defined(ANDROID_BUILD) || defined(PC_BUILD)
		SDL_Delay(16);
#endif
	}

	off_video();
	shoutdown_sistem();
	return 0;
}
