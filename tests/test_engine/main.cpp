#include <engine/engine.h>
#include <input/Input.h>
#include <audio/mixer.h>
#include <audio/sample.h>
#include <font/gfxFont.h>
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- INCLUSIÓN DE TUS HEADERS DE TEST ---
#include "test_graficos.h"
// #include "tests/audio_test.h"

#define MAX_TESTS 8
#define VISIBLE_ITEMS 4

// logic es la superficie precalculada del motor
extern SDL_Surface *logic;

// Variables para el Marquee de Retropanda88
float marqueeX = 320.0f;
const char *infoText =
	"DESARROLLADO POR RETROPANDA88 - USE ARRIBA/ABAJO PARA NAVEGAR - BOTON A PARA SELECCIONAR TEST";

// ==========================================================
// ESTRUCTURAS Y TIPOS
// ==========================================================
typedef struct icon
{
	SDL_Surface *icon;
} icon;

typedef void (*TestAction) (void);

struct TestItem
{
	const char *title;
	SDL_Surface *ico;
	TestAction action;			// Puntero a la función del test
	float animOffset;
};

// ==========================================================
// FUNCIONES DE APOYO
// ==========================================================

void render_background_gradient(SDL_Surface * s, u32 colorTop, u32 colorBottom)
{
	/*int r1 = (colorTop >> 16) & 0xFF, g1 = (colorTop >> 8) & 0xFF, b1 = colorTop & 0xFF;
	int r2 = (colorBottom >> 16) & 0xFF, g2 = (colorBottom >> 8) & 0xFF, b2 = colorBottom & 0xFF;
	for (int i = 0; i < s->h; i++)
	{
		float f = (float)i / (float)s->h;
		u8 r = (u8) (r1 + (r2 - r1) * f);
		u8 g = (u8) (g1 + (g2 - g1) * f);
		u8 b = (u8) (b1 + (b2 - b1) * f);
		fill_rect(s, 0, i, s->w, 1, color_rgb(r, g, b));
	}*/
	fill_vertical_gradient(s, colorTop, colorBottom);
}

icon *quickLoad(SDL_Surface * sheet, int x, int y, int w, int h, float zoom)
{
	icon *temp = (icon *) malloc(sizeof(icon));
	if (!temp)
		return NULL;
	temp->icon = cut_surface(sheet, x, y, w, h);
	SDL_Surface *scaled = rotozoom_create(temp->icon, 0.0f, zoom);
	SDL_FreeSurface(temp->icon);
	temp->icon = scaled;
	return temp;
}

void renderItem(SDL_Surface * s, TestItem * it, int x, int y, bool sel, bool prs)
{
	int w = 210, h = 36;
	float target = sel ? 15.0f : 0.0f;
	it->animOffset += (target - it->animOffset) * 0.2f;
	int cx = x + (int)it->animOffset;

	u32 bg =
		prs ? color_rgb(255, 255, 255) : (sel ? color_rgb(40, 60, 120) : color_rgb(20, 20, 35));
	u32 acc = sel ? color_rgb(0, 255, 255) : color_rgb(60, 60, 80);

	fill_rect(s, cx + 12, y, w - 12, h, bg);
	fill_triangle_fast(s, cx + 12, y, cx, y + h, cx + 12, y + h, bg);
	if (it->ico)
		draw_surface(it->ico, cx - 2, y + (h / 2) - (it->ico->h / 2));

	print(cx + 42, y + 13, it->title, sel ? color_rgb(255, 255, 255) : color_rgb(160, 160, 170));
	fill_rect(s, cx + w - 3, y + 4, 2, h - 8, acc);
}

// ==========================================================
// MAIN
// ==========================================================
int main(int argc, char **argv)
{
	if (Init_Sistem("GPP Pro Suite") < 0)
		return 1;
	Set_Video();
	Input::init();

	gfxFont font;
	font.init();

	Cmixer mixer;
	mixer.init(44100, 2, 2048);

	CSample sfxMove, sfxPush;
	sfxMove.Load("sfx/button.wav");
	sfxPush.Load("sfx/push.wav");

	SDL_Surface *sheet = load_img("gfx/icon.png");
	startup();
	mixer.playMusic("music/music.wav", true);
	

	icon *icons[MAX_TESTS];

	icons[0] = quickLoad(sheet, 35, 5, 190, 200, 0.2);
	icons[1] = quickLoad(sheet, 250, 5, 190, 200, 0.2);
	icons[2] = quickLoad(sheet, 460, 5, 190, 200, 0.2);
	icons[3] = quickLoad(sheet, 680, 5, 190, 200, 0.2);

	icons[4] = quickLoad(sheet, 250, 230, 190, 210, 0.2);
	icons[5] = quickLoad(sheet, 35, 230, 190, 210, 0.2);
	icons[6] = quickLoad(sheet, 460, 240, 190, 200, 0.2);
	icons[7] = quickLoad(sheet, 680, 240, 190, 200, 0.2);

	// --- CONEXIÓN DE LOS TESTS ---
	TestItem tests[MAX_TESTS] = {
		{"Graphics", icons[0]->icon, run_graficos_test, 0.0f}
		,
		{"Audio", icons[1]->icon, NULL, 0.0f}
		,
		{"Input Pad", icons[2]->icon, NULL, 0.0f}
		,
		{"Fonts", icons[3]->icon, NULL, 0.0f}
		,
		{"Sprites", icons[4]->icon, NULL, 0.0f}
		,
		{"Gfx Engine", icons[5]->icon, NULL, 0.0f}
		,
		{"CPU Stress", icons[6]->icon, NULL, 0.0f}
		,
		{"Credits", icons[7]->icon, NULL, 0.0f}
	};

	int sel = 0, scroll = 0, d_l = 0, u_l = 0, a_l = 0;



	while (1)
	{
		Input::update();
		bool d = Input::isPressed(0, BUTTON_DOWN);
		bool u = Input::isPressed(0, BUTTON_UP);
		bool a = Input::isPressed(0, BUTTON_A);

		// Navegación
		if (d && !d_l)
		{
			sel = (sel + 1) % MAX_TESTS;
			mixer.playChannel(&sfxMove, 0, 128);
		}
		if (u && !u_l)
		{
			sel = (sel - 1 + MAX_TESTS) % MAX_TESTS;
			mixer.playChannel(&sfxMove, 0, 128);
		}

		// Lanzar Test
		if (a && !a_l)
		{
			mixer.playChannel(&sfxPush, 0, 128);
			if (tests[sel].action != NULL)
			{
				// El programa entra aquí y no sale hasta que el test termine
				tests[sel].action();
			}
		}
		d_l = d;
		u_l = u;
		a_l = a;

		if (sel >= scroll + VISIBLE_ITEMS)
			scroll = sel - VISIBLE_ITEMS + 1;
		if (sel < scroll)
			scroll = sel;

		// Renderizado sobre logic
		render_background_gradient(logic, color_rgb(10, 15, 30), color_rgb(35, 55, 95));

		fontsize(16, 16);
		print(20, 15, "TEST SUITE", color_rgb(0, 255, 255));
		fill_rect(logic, 20, 36, 210, 2, color_rgb(0, 255, 255));
		fill_rect(logic, 20, 38, 210, 2, color_rgb(120, 66, 255));
		fill_rect(logic, 20, 40, 210, 2, color_rgb(255, 0, 0));

		for (int i = 0; i < VISIBLE_ITEMS; i++)
		{
			int idx = scroll + i;
			if (idx < MAX_TESTS)
			{
				renderItem(logic, &tests[idx], 40, 55 + (i * 42), (idx == sel), (idx == sel && a));
			}
		}

		// Scrollbar
		fill_rect(logic, 305, 55, 3, 160, color_rgb(30, 35, 60));
		int sY = 55 + (scroll * (160 - 30) / (MAX_TESTS - VISIBLE_ITEMS));
		fill_rect(logic, 305, sY, 3, 30, color_rgb(0, 255, 255));

		// Marquee Inferior
		fill_rect(logic, 0, 222, 320, 18, color_rgb(0, 0, 0));
		font.draw(logic, MMX_FONT, (int)marqueeX, 225, infoText);

		marqueeX -= 1.3f;
		if (marqueeX < -1000.0f)
			marqueeX = 320.0f;

		Render();
		Fps_sincronizar(60);
	}

	return 0;
}
