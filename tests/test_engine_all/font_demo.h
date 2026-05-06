#ifndef FONT_DEMO_H
#define FONT_DEMO_H

#include "font/test10.h"


void font_demo()
{
	int exit = 0;
	
	TEST10();

	
	while (!exit)
	{

		Input::update();

		if (Input::isPressed(0, BUTTON_B))
			exit = 1;

		cls();

		print_f(120, 100, color_rgb(255, 255, 255), "FONT DEMO OK");
		print_f(100, 130, color_rgb(255, 255, 0), "PRESS B TO RETURN");

		Render();
		Fps_sincronizar(10);
	}
}

#endif
