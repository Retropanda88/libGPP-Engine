#include <engine/engine.h>

#define NUM_TESTS 6

// ==============================
// Variables Globales
// ==============================
int done = 0;
SDL_Surface *temp = NULL;
Cmixer mixer;

const char *tests[NUM_TESTS] = {
	"Video Test",
	"Sound Test",
	"Font Test",
	"Input Test",
	"File System Test",
	"Sprite Test"
};

int selected = 0;

// ==============================
// Texto desplazable
// ==============================
float scroll_x = 320;

const char *scroll_text =
"LIBGPP-ENGINE DEMO - USE DPAD UP/DOWN TO SELECT A TEST - PRESS A TO RUN THE TEST - PRESS SELECT TO EXIT";


// ==============================
// Dibujar marco de la pantalla
// ==============================
void drawTable()
{
	u32 red = color_rgb(0, 255, 255);

	draw_line_fast(logic, 1, 2, 318, 2, red);
	draw_line_fast(logic, 1, 238, 318, 238, red);
	draw_line_fast(logic, 1, 2, 1, 238, red);
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
	int y = 60;

	for (int i = 0; i < NUM_TESTS; i++)
	{
		u32 color = color_rgb(255, 255, 255);

		if (i == selected)
		{
			color = color_rgb(0, 100, 255);
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

	if (!mixer.init(22050, 2, 1024, 100))
		return 1;

	init();
	startup();

	mixer.loadMusic("music/music.wav", true);
	mixer.playMusic();

	while (!done)
	{
		update();

		drawFramewire();

		Render();
		Fps_sincronizar(10);
	}

	off_video();
	shoutdown_sistem();

	return 0;
}