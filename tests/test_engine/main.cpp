#include <engine/Engine.h>
#include <input/Input.h>
#include <input/inputTypes.h>

#include "pad_jpg.h"

SDL_Surface *img = NULL;
SDL_Surface *set = NULL;

int markerX = -100;
int markerY = -100;

void load_surface()
{
    img = load_texture_from_mem((u8 *) pad_data, pad_size);

    set = create_surface(15, 15, SDL_SWSURFACE);
    fill_radial_gradient(set, color_rgb(25, 100, 100), color_rgb(0, 255, 100));
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
        markerX = 52;
        markerY = 88;
    }
    if (Input::isDown(0, BUTTON_RIGHT))
    {
        markerX = 88;
        markerY = 88;
    }
    if (Input::isDown(0, BUTTON_UP))
    {
        markerX = 72;
        markerY = 70;
    }
    if (Input::isDown(0, BUTTON_DOWN))
    {
        markerX = 72;
        markerY = 105;
    }

    // Botones
    if (Input::isDown(0, BUTTON_A))
    {
        markerX = 265;
        markerY = 90;
    }
    if (Input::isDown(0, BUTTON_B))
    {
       
        markerX = 238;
        markerY = 110;
    }
    if (Input::isDown(0, BUTTON_X))
    {
        markerX = 238;
        markerY = 68;
    }
    if (Input::isDown(0, BUTTON_Y))
    {
        markerX = 210;
        markerY = 87;
    }
    
     if (Input::isDown(0, BUTTON_L1))
    {
        markerX = 70;
        markerY = 28;
    }
    
     if (Input::isDown(0, BUTTON_R1))
    {
        markerX = 240;
        markerY = 28;
    }
    
       if (Input::isDown(0, BUTTON_START))
    {
        markerX = 160;
        markerY = 98;
    }
}

void render_game()
{
	cls();

	// Dibujar imagen
	SDL_Surface *temp = rotozoom_create(img, 0, 0.2);
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
