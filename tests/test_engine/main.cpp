/* 
   ============================================================================
   libGPP-Engine Test Suite - Main Entry Point Author: Andrés Ruiz Pérez
   Description: Executes all rendering stress tests for libGPP-Engine. Designed
   for embedded platforms with 320x240 resolution.

   Tests included: - Test 1: Random texture allocation + gradients - Test 2:
   Random rectangle gradient textures - Test 3: Alpha blending / fade test -
   Test 4: Horizontal gradient surfaces - Test 5: Vertical gradient surfaces -
   Test 6: Radial gradient surfaces

   Supported Platforms: - Android, PSP, PS2, GameCube
   ============================================================================ */

#include <engine/Engine.h>


#include "test0.h"
#include "test1.h"
#include "test2.h"
#include "test3.h"
#include "test4.h"
#include "test5.h"
#include "test6.h"
#include "test7.h"
#include "test8.h"
#include "test9.h"
#include "test10.h"


#define MAX_LINES 6

const char list[][50] = {
	"Solid Rectangle",
	"Horizontal Gradient",
	"Vertical Gradient",
	"Radial Gradient",
	"Sprite Drawing",
	"Font Writing"
};

static Cmixer gMixer;



int audio_init(void)
{
	/* inicializar mixer */
	gMixer.init(22050, 2, 512, 128);

	/* cargar música */
	if (!gMixer.loadMusic("music.wav", true))
	{
		printf("Audio: failed to load music\n");
		return -1;
	}

	gMixer.playMusic();
	return 0;
}

void audio_shutdown(void)
{
	// gMixer.stopMusic();
}


void drawMenu();


int main(int argc, char **argv)
{
	if (Init_Sistem("libGPP-Engine Test Suite v01") != 0)
		return 1;

	if (Set_Video() != 0)
		return 1;

	if (audio_init() < 0)
		return 1;


	drawMenu();


	audio_shutdown();
	off_video();
	shoutdown_sistem();

	return 0;
}

void drawMenu()
{
	/* Superficie persistente reutilizable */
	SDL_Surface *s = create_surface(320, 240, SDL_SWSURFACE);
	if (!s)
	{
		print(70, 100, "ERROR: Failed to create surface", color_rgb(255, 0, 0));
		Render();
		SDL_Delay(2000);
	}

	fill_radial_gradient(logic, color_rgb(25, 77, 255), color_rgb(22, 25, 160));
	u32 white = color_rgb(255, 255, 255);



	draw_line(2, 2, 316, 2, white);
	draw_line(2, 2, 2, 236, white);
	draw_line(2, 236, 316, 236, white);
	draw_line(316, 2, 316, 236, white);
	draw_line(2, 30, 316, 30, white);

	/* const int x = 80; const int y = 80; int line_height = 12;


	   int count = sizeof(list) / sizeof(list[0]);

	   for (int i = 0; i < count; i++) print(x, y + i * line_height, list[i],
	   white);


	   print(80, 15, "Demo libGPP-Engine", white); */


	/* --- TEST FS --- */

	if (!fs_exists("roms"))
	{
		if (fs_mkdir("roms") == 0)
			print(80, 40, "roms creado", white);
		else
			print(80, 40, "error creando roms", white);
	}
	else
	{
		print(80, 40, "roms ya existe", white);
	}

	if (fs_exists("roms"))
		print(80, 60, "roms confirmado", white);
	else
		print(80, 60, "roms no existe", white);

	if (fs_isdir("roms"))
		print(80, 80, "roms es directorio", white);
	else
		print(80, 80, "roms NO es directorio", white);

	if (fs_rmdir("roms") == 0)
		print(80, 120, "roms eliminado", white);
	else
		print(80, 120, "error eliminando", white);

	FS_DIR dir;

	if (fs_opendir(&dir, ".") == 0)
		print(80, 140, "opendir OK", white);
	else
		print(80, 140, "opendir FAIL", white);

	// FS_DIR dir;
	FS_DIRENT ent;

	int y = 160;

	if (fs_opendir(&dir, ".") == 0)
	{
		while (fs_readdir(&dir, &ent) == 0)
		{
			print(20, y, ent.name, white);
			y += 15;
		}

		fs_closedir(&dir);
	}
	else
	{
		print(20, y, "opendir FAIL", white);
	}

	FS_FILE *f = fs_open("test.txt", "wb");
	if (f)
	{
		fs_close(f);
		print(100, 200, "archivo creado", white);
	}

	if (fs_remove("test.txt") == 0)
		print(100, 220, "archivo eliminado", white);
	else
		print(100, 220, "error eliminando", white); 


	Render();
	while (1)
	{
	};

}
