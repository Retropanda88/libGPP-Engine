/*----------------------------------------------+
| gfx							Florian Hufsky	|
|												|
| start:		14.12.2003						|
| last changes:	22.12.2003						|
+----------------------------------------------*/
/*--------------------------------------------------------------+
| gfx															|
|																|
| a stripped down version of tfree/free  for a pixelate article	|
|																|
| gfx is a mini graphics library containing						|
|  + initialisation of SDL										|
|  + a class for simple Sprites									|
|  + a class for fonts											|
|																|
| have a lot of fun!											|
|		  � 2003-2008 Florian Hufsky <florian.hufsky@gmail.com>	|
+--------------------------------------------------------------*/

#ifndef __GFX_H__
#define __GFX_H__

#include <SDL/SDL_image.h>
#include <SDL/SDL.h>
#include <engine/SFont.h>
#include <string>
#include <engine/types.h>

#define MMX_FONT            0x00
#define SMALL_FONT         0x01
#define LARGE_FONT         0x02
#define ROCK_FONT           0x03 //solo mayusculas

class gfxFont
{
	public:
		gfxFont();
		~gfxFont();
		
        bool init();
		bool init(const char *filename);
		bool init( u8 *buffer, int len);
		
		void add_font_char(const char *font_chars);
		
		void draw(SDL_Surface *screen, int type , int x, int y,const char *s);
		
		void drawf(SDL_Surface *screen, int type,  int x, int y, const char *s, ...);

		void drawCentered(SDL_Surface *screen, int type,  int x, int y, const char *text);
		
		
		void drawChopCentered(SDL_Surface *screen, int type, int x, int y, int width, const char *text);
		
		
		void drawRightJustified(SDL_Surface *screen, int type, int x, int y, const char *s, ...);
		
		
		void drawChopRight(SDL_Surface *screen, int type,  int x, int y, int width, const char *s);
		
		
		void drawChopLeft(SDL_Surface *screen, int type,  int x, int y, int width, const char *s);

		void setalpha(int type, Uint8 alpha);

		int getHeight(){return SFont_TextHeight(m_font);};
		int getWidth(const char *text){return SFont_TextWidth(m_font, text);};

	private:
	   //anexamos mas fuentes
		SFont_Font *m_font, *font_mmx, *font_small, *font_large;
		int ik;
};

#endif //__GFX_H__

