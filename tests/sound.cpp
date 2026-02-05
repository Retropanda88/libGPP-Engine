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

#include <engine/engine.h>
#include "sfx.h"


int main(int argc, char **argv)
{
	// Initialize engine core
	if (Init_Sistem("libGPP-Engine Test Suite v01") != 0)
		return 1;

	// Initialize video mode
	if (Set_Video() != 0)
		return 1;

	CSample sample;
	Cmixer mixer;

	if(mixer.init(48000,2,512,128)==false){
		print(100,100,"sample test fail",color_rgb(0,255,0));
   	Render();
   	SDL_Delay(3000);
		return 1;
	}


	if (!sample.LoadFromMemory((u8*)button_data, button_size))
{
    print(10,110,"Error cargando WAV desde memoria",color_rgb(0,255,0));
}

   mixer.playChannel(&sample,-1,1,128);

   print(100,100,"sample test",color_rgb(0,255,0));
   Render();
   SDL_Delay(3000);
   while(1);
	off_video();
	shoutdown_sistem();

	// End program gracefully
	return 0;
}
