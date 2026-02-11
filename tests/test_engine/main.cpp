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


    // Initialize engine core
    if (Init_Sistem("libGPP-Engine Test Suite v01") != 0)
        return 1;

    // Initialize video mode
    if (Set_Video() != 0)
        return 1;

    if(audio_init()<0)
        return 1;


    // Execute all available tests once
    /* TEST0(); TEST1(); TEST2(); TEST3(); TEST4(); TEST5(); TEST6(); TEST7();
       TEST8(); TEST9(); TEST10(); */

    drawMenu();
    off_video();
    shoutdown_sistem();
    audio_shutdown();

    // End program gracefully 
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
    draw_line(2, 2 ,2, 236, white);
    draw_line(2, 236,316, 236, white);
    draw_line(316, 2,316, 236, white);
    draw_line(2, 30,316, 30, white);

    const int x = 80;
    const int y = 80;
    int line_height = 12;

    
    int count = sizeof(list) / sizeof(list[0]);

    for (int i = 0; i < count; i++)
        print(x, y + i * line_height, list[i], white);
        
        
    print(80, 15, "Demo libGPP-Engine", white);

    Render();
    while (1)
    {
    };

}
