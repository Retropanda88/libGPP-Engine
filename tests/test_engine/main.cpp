#include <engine/engine.h>

#include "video_demo.h"
#include "font_demo.h"
#include "input_demo.h"
#include "sprite_demo.h"
#include "gfx_demo.h"
#include "fs_demo.h"

#define NUM_TESTS 9
#define NUM_TESTS_VIDEO 4

// ==============================
// Variables Globales
// ==============================
int done = 0;
SDL_Surface *temp = NULL;
Cmixer mixer;

const char *tests[NUM_TESTS] = {
	"Video", "Sound", "Font", "Input", "File System", "Sprites", "Gfx", "Creditos", "exit"
};

const char *credits[] = {
	"LIBGPP-ENGINE",
	"",
	"Created by",
	"Andres Ruiz Perez",
	"",
	"Programming",
	"Andres Ruiz Perez",
	"",
	"Engine",
	"libGPP-Engine",
	"",
	"Libraries",
	"SDL 1.2",
	"",
	"Platforms",
	"Android",
	"PC",
	"PlayStation 2",
	"GameCube",
	"PSP",
	"",
	"Special Thanks",
	"Homebrew Community",
	"DevKitPro",
	"PS2SDK Developers"
};

#define NUM_CREDITS 24


int selected = 0;

// ==============================
// Texto desplazable
// ==============================
float scroll_x = 320;
float credits_y = 260;

const char *scroll_text =
	"LIBGPP-ENGINE DEMO - USE DPAD UP/DOWN TO SELECT A TEST - PRESS A TO RUN THE TEST - PRESS SELECT TO EXIT";


void print_center(int y, u32 color, const char *text)
{
	int x = (320 / 2) - (strlen(text) * 4);
	print_f(x, y, color, "%s", text);
}


void credits_demo()
{
	int exit = 0;

	while (!exit)
	{
		Input::update();

		if (Input::isPressed(0, BUTTON_B))
			exit = 1;

		cls();

		int y = credits_y;

		for (int i = 0; i < NUM_CREDITS; i++)
		{
			int x = (320 / 2) - (strlen(credits[i]) * 4);

			print_f(x, y, color_rgb(255, 255, 255), "%s", credits[i]);

			y += 20;
		}

		credits_y -= 0.5;

		if (credits_y < -500)
			exit = 1;


		Render();
		Fps_sincronizar(10);
	}
	credits_y = 260;
}


// ==============================
// Dibujar marco de la pantalla
// ==============================
void drawTable()
{
	u32 red = color_rgb(0, 255, 255);

	draw_line_fast(logic, 0, 2, 318, 2, red);
	draw_line_fast(logic, 0, 238, 318, 238, red);
	draw_line_fast(logic, 0, 2, 0, 238, red);
	draw_line_fast(logic, 318, 2, 318, 238, red);

	draw_line_fast(logic, 2, 25, 318, 25, red);
}


// ==============================
// Inicialización
// ==============================
void init()
{
	u32 color1 = color_rgb(180, 0, 0);
	u32 color2 = color_rgb(0, 0, 0);

	temp = create_surface(320, 240, SDL_SWSURFACE);

	fill_radial_gradient(temp, color2, color1);

	Input::init();
}


// ==============================
// Dibujar menú
// ==============================
void drawMenu()
{
	int y = 40;
	int x = 80;
	// int x_r = x + 20;
	int x_r = x + 40;

	for (int i = 0; i < NUM_TESTS; i++)
	{
		u32 color = color_rgb(255, 255, 255);

		if (i == selected)
		{
			color = color_rgb(255, 255, 0);
			print_f(x, y, color, "> %s", tests[i]);
		}
		else
		{
			print_f(x_r, y, color, "%s", tests[i]);
		}

		y += 20;
	}
}

void runTest()
{

	switch (selected)
	{
	case 0:
		video_demo();
		break;

	case 1:
		cls();
		Render();
		mixer.stopMusic();
		SDL_Delay(1000);
		mixer.playMusic("music/music_22050.wav", true);

		Input::update();

		if (Input::isPressed(0, BUTTON_B))

			cls();

		print_f(120, 100, color_rgb(255, 255, 255), "SOUND DEMO OK");
		// print_f(100, 130, color_rgb(255, 255, 0), "PRESS B TO RETURN");

		Render();
		SDL_Delay(4000);

		break;

	case 2:
		font_demo();
		break;

	case 3:
		input_demo();
		break;

	case 4:
		fs_demo();
		break;

	case 5:
		sprite_demo();
		break;

	case 6:
		gfx_demo();
		break;

	case 7:
		credits_demo();
		break;

	case 8:
		done = 1;
		break;
	}
}


// ==============================
// Update
// ==============================
void update()
{
	Input::update();

	if (Input::isPressed(0, BUTTON_UP))
	{
		selected--;

		if (selected < 0)
			selected = NUM_TESTS - 1;
	}

	if (Input::isPressed(0, BUTTON_DOWN))
	{
		selected++;

		if (selected >= NUM_TESTS)
			selected = 0;
	}

	// mover texto
	scroll_x -= 0.15;

	if (scroll_x < -900)
		scroll_x = 320;

	// salir del programa
	if (Input::isPressed(0, BUTTON_SELECT))
		done = 1;

	if (Input::isPressed(0, BUTTON_A))
	{
		runTest();
	}
}


// ==============================
// Dibujar frame
// ==============================
void drawFramewire()
{
	draw_surface(temp, 0, 0);

	drawTable();

	print_f(70, 10, color_rgb(255, 255, 255), "libGGP-Engine DEMO");

	drawMenu();

	// texto desplazable
	print_f(scroll_x, 220, color_rgb(255, 255, 0), scroll_text);
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


	if (!mixer.init(22050, 2, 512))
		return 1;

	init();
	startup();



	mixer.playMusic("music/music_22050.wav", true);



	while (!done)
	{
		update();
		drawFramewire();
		Render();
		Fps_sincronizar(10);
		update_log_scroll();
	}

	off_video();
	shoutdown_sistem();

	return 0;
}
