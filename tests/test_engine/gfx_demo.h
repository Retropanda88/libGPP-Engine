#ifndef GFX_DEMO_H
#define GFX_DEMO_H

#include "gfx/test8.h"


void gfx_demo()
{
	int exit = 0;
	
	TEST8();

	
	while (!exit)
	{

		Input::update();

		if (Input::isPressed(0, BUTTON_B))
			exit = 1;

		cls();

		print_f(120, 100, color_rgb(255, 255, 255), "GFX DEMO OK");
		print_f(100, 130, color_rgb(255, 255, 0), "PRESS B TO RETURN");

		Render();
		Fps_sincronizar(10);
	}
}

#endif
