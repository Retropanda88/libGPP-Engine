#ifndef GRAFICOS_TEST_H
#define GRAFICOS_TEST_H

#include <engine/engine.h>
#include <input/Input.h>
#include <sprite/Sprite.h>
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>

// Superficie precalculada
extern SDL_Surface *logic;

struct Entity
{
	float x, y, speed;
};

inline void run_graficos_test()
{
	printf(">> TEST_GRAFICOS: INICIADO\n");

	// 🔴 Validación importante
	if (!logic)
	{
		printf("ERROR: logic es NULL\n");
		return;
	}

	bool testing = true;
	int entityCount = 40;
	const int MAX_ENTITIES = 300;

	// -------- SPRITE --------
	Sprite *player = new Sprite("gfx/sheet.bmp", 6, 120);

	if (!player)
	{
		printf("ERROR: no se pudo crear sprite\n");
		return;
	}

	player->setWidth(32);
	player->setHeight(32);
	player->setTransparency(255, 0, 255);
	player->start();

	// -------- ENTIDADES --------
	Entity entities[MAX_ENTITIES];

	for (int i = 0; i < MAX_ENTITIES; i++)
	{
		entities[i].x = (float)(rand() % 320);
		entities[i].y = (float)(rand() % 160 + 40);
		entities[i].speed = 0.8f + (rand() % 100) / 40.0f;
	}

	// -------- LOOP --------
	while (testing)
	{
		Input::update();

		if (Input::isPressed(0, BUTTON_B))
		{
			printf(">> TEST_GRAFICOS: SALIENDO...\n");
			testing = false;
		}

		fill_vertical_gradient(logic, color_rgb(20, 20, 40), color_rgb(60, 40, 100));

		// 🔥 animar UNA sola vez por frame
		player->animate();

		for (int j = 0; j < entityCount; j++)
		{
			entities[j].x += entities[j].speed;

			if (entities[j].x > 320)
				entities[j].x = -32;

			player->draw(logic, (int)entities[j].x, (int)entities[j].y);
		}

		// UI
		fontsize(8, 8);
		print(10, 10, "GRAFICOS TEST", color_rgb(0, 255, 255));

		char buffer[64];
		sprintf(buffer, "SPRITES: %d | B: SALIR", entityCount);
		print(10, 220, buffer, color_rgb(255, 255, 0));

		// Controles
		if (Input::isPressed(0, BUTTON_UP) && entityCount < MAX_ENTITIES)
			entityCount++;

		if (Input::isPressed(0, BUTTON_DOWN) && entityCount > 1)
			entityCount--;

		Render();
		Fps_sincronizar(60);
	}

	// -------- LIMPIEZA --------

	// limpiar eventos (importante para segunda ejecución)
	SDL_Event e;
	while (SDL_PollEvent(&e));

	if (player)
	{
		delete player;
		player = NULL;
	}


	printf(">> TEST_GRAFICOS: FINALIZADO\n");
}

#endif // GRAFICOS_TEST_H
