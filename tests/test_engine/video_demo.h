#ifndef VIDEO_DEMO_H
#define VIDEO_DEMO_H

#include "video/test0.h"
#include "video/test1.h"
#include "video/test2.h"
#include "video/test3.h"
#include "video/test4.h"
#include "video/test5.h"
#include "video/test6.h"
#include "video/test7.h"

void video_demo()
{
	int exit = 0;
	
	TEST0();
	TEST1();
	TEST2();
	TEST3();
	TEST4();
	TEST5();
	TEST6();
	TEST7();
	
	while (!exit)
	{

		Input::update();

		if (Input::isPressed(0, BUTTON_B))
			exit = 1;

		cls();

		print_f(120, 100, color_rgb(255, 255, 255), "VIDEO DEMO OK");
		print_f(100, 130, color_rgb(255, 255, 0), "PRESS B TO RETURN");

		Render();
		Fps_sincronizar(10);
	}
}

#endif
