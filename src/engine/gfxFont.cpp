#include <engine/gfxFont.h>

#include <stdarg.h>				// atexit?
#include <stdlib.h>
#include <stdio.h>				// atexit?
#include <string>

#include <engine/surface_tools.h>
#include <engine/SFont.h>
#include <engine/log.h>

// fonts 
#include <engine/font_large.h>
#include <engine/font_mmx.h>
#include <engine/font_small.h>
#include <engine/font_rock.h>


// chars caraters order
const char *font_chars_mmx =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"abcdefghijklmnopqrstuvwxyz" "0123456789" "!\"#$%&'()*+,-./" ":;<=>?" "[\\]^_`";

const char *font_chars_rock = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "1234567890%:!?+-=*/.(),'\"$#\\";



int short x_shake;
int short y_shake;

// gfxFont
gfxFont::gfxFont()
{
	m_font = NULL;
	font_mmx = NULL;
	font_small = NULL;
	font_large = NULL;
	ik = -1;
}

gfxFont::~gfxFont()
{
	if (m_font)
		SFont_FreeFont(m_font);
};

bool gfxFont::init(const char *filename)
{
	if (m_font)
		SFont_FreeFont(m_font);

	printf("\nloading font....\n");

	SDL_Surface *fontsurf = IMG_Load(filename);
	if (fontsurf == NULL)
	{
		printf(" ERROR: Couldn't load file %s\n ", SDL_GetError());

		return false;
	}

	m_font = SFont_InitFont(fontsurf);
	if (!m_font)
	{
		printf(" ERROR: an error occurre while loading the font.");
		return false;
	}
	m_font->font_chars = NULL;
	m_font->is_char_font = -1;	// recuerda que si no inicias esto en 0 no
	// funciona la fuente de forma normal...
	printf("done");
	return true;
}



void gfxFont::add_font_char(const char *font_chars)
{
	add_font_chars(font_chars, m_font);
}



// esta funcion permite cargar bitmap desde la memoria

bool gfxFont::init(u8 * buffer, int len)
{
	if (m_font)
		SFont_FreeFont(m_font);

	printf("\nloading font....\n");

	SDL_Surface *fontsurf = load_texture_from_mem(buffer, len);
	if (fontsurf == NULL)
	{
		printf(" ERROR: Couldn't load file %s\n ", SDL_GetError());

		return false;
	}

	m_font = SFont_InitFont(fontsurf);
	if (!m_font)
	{
		printf(" ERROR: an error occurre while loading the font.");
		return false;
	}

	printf("done");
	return true;
}

// esto se mandara llamar una sola vez 
// se leen todasnlas fuentes posibles
bool gfxFont::init()
{

	if (font_mmx)
		SFont_FreeFont(m_font);

	printf("\nloading font....\n");

	// font mmx
	font_mmx = SFont_InitFont(load_texture_from_mem((u8 *) font_mmx_data, font_mmx_size));

	// font large
	font_large = SFont_InitFont(load_texture_from_mem((u8 *) font_large_data, font_large_size));

	// font rock
	m_font = SFont_InitFont(load_texture_from_mem((u8 *) rock_font_data, rock_font_size));

	// font small
	font_small = SFont_InitFont(load_texture_from_mem((u8 *) font_small_data, font_small_size));

	if (!font_mmx && !font_small && !font_large && !m_font)
	{
		Write_Log("error: unload bitmat font %s", SDL_GetError());
		return false;
	}

	add_font_chars(font_chars_mmx, font_mmx);
	add_font_chars(font_chars_rock, m_font);

	Write_Log("font load success");

	return true;
}

void gfxFont::draw(SDL_Surface * screen, int type, int x, int y, const char *s)
{
	// if(y + getHeight() < 0)
	// return;
	ik = type;
	switch (ik)
	{
	case MMX_FONT:
		SFont_Write(screen, font_mmx, x, y, s);
		break;
	case SMALL_FONT:
		SFont_Write(screen, font_small, x, y, s);
		break;
	case LARGE_FONT:
		SFont_Write(screen, font_large, x, y, s);
		break;
	case ROCK_FONT:
		SFont_Write(screen, m_font, x, y, s);
		break;
	}
}

void gfxFont::drawChopRight(SDL_Surface * screen, int type, int x, int y, int width, const char *s)
{
	// if(y + getHeight() < 0)
	// return;

	ik = type;
	switch (ik)
	{
	case MMX_FONT:
		SFont_WriteChopRight(screen, font_mmx, x, y, width, s);
		break;
	case SMALL_FONT:
		SFont_WriteChopRight(screen, font_small, x, y, width, s);
		break;
	case LARGE_FONT:
		SFont_WriteChopRight(screen, font_large, x, y, width, s);
		break;
	case ROCK_FONT:
		SFont_WriteChopRight(screen, m_font, x, y, width, s);
		break;
	}

}

void gfxFont::drawChopLeft(SDL_Surface * screen, int type, int x, int y, int width, const char *s)
{
	// if(y + getHeight() < 0)
	// return;

	ik = type;
	switch (ik)
	{
	case MMX_FONT:
		SFont_WriteChopLeft(screen, font_mmx, x, y, width, s);
		break;
	case SMALL_FONT:
		SFont_WriteChopLeft(screen, font_small, x, y, width, s);
		break;
	case LARGE_FONT:
		SFont_WriteChopLeft(screen, font_large, x, y, width, s);
		break;
	case ROCK_FONT:
		SFont_WriteChopLeft(screen, m_font, x, y, width, s);
		break;
	}

	// SFont_WriteChopLeft(screen, m_font, x, y, width, s);
}

void gfxFont::drawCentered(SDL_Surface * screen, int type, int x, int y, const char *text)
{
	// if(y + getHeight() < 0)
	// return;

	ik = type;
	switch (ik)
	{
	case MMX_FONT:
		SFont_WriteCenter(screen, font_mmx, x, y, text);
		break;
	case SMALL_FONT:
		SFont_WriteCenter(screen, font_small, x, y, text);
		break;
	case LARGE_FONT:
		SFont_WriteCenter(screen, font_large, x, y, text);
		break;
	case ROCK_FONT:
		SFont_WriteCenter(screen, m_font, x, y, text);
		break;
	}

	// SFont_WriteCenter(screen, m_font, x, y, text);
};

void gfxFont::drawChopCentered(SDL_Surface * screen, int type, int x, int y, int width,
							   const char *text)
{
	// if(y + getHeight() < 0)
	// return;

	ik = type;
	switch (ik)
	{
	case MMX_FONT:
		SFont_WriteChopCenter(screen, font_mmx, x, y, width, text);
		break;
	case SMALL_FONT:
		SFont_WriteChopCenter(screen, font_small, x, y, width, text);
		break;
	case LARGE_FONT:
		SFont_WriteChopCenter(screen, font_large, x, y, width, text);
		break;
	case ROCK_FONT:
		SFont_WriteChopCenter(screen, m_font, x, y, width, text);
		break;
	}

	// SFont_WriteChopCenter(screen, m_font, x, y, width, text);
};

void gfxFont::drawRightJustified(SDL_Surface * screen, int type, int x, int y, const char *s, ...)
{
	char buffer[256];

	va_list zeiger;
	va_start(zeiger, s);
	vsprintf(buffer, s, zeiger);
	va_end(zeiger);

	// if(y + getHeight() < 0)
	// return;
	ik = type;
	switch (ik)
	{
	case MMX_FONT:
		SFont_WriteRight(screen, font_mmx, x, y, buffer);
		break;
	case SMALL_FONT:
		SFont_WriteRight(screen, font_small, x, y, buffer);
		break;
	case LARGE_FONT:
		SFont_WriteRight(screen, font_large, x, y, buffer);
		break;
	case ROCK_FONT:
		SFont_WriteRight(screen, m_font, x, y, buffer);
		break;
	}


	// SFont_WriteRight(screen, m_font, x, y, buffer);
};


void gfxFont::drawf(SDL_Surface * screen, int type, int x, int y, const char *s, ...)
{
	char buffer[256];

	va_list zeiger;
	va_start(zeiger, s);
	vsprintf(buffer, s, zeiger);
	va_end(zeiger);

	// draw(screen, x, y, buffer);

	ik = type;
	switch (ik)
	{
	case MMX_FONT:
		draw(screen, MMX_FONT, x, y, buffer);
		break;
	case SMALL_FONT:
		draw(screen, SMALL_FONT, x, y, buffer);
		break;
	case LARGE_FONT:
		draw(screen, LARGE_FONT, x, y, buffer);
		break;
	case ROCK_FONT:
		draw(screen, ROCK_FONT, x, y, buffer);
		break;
	}
}

void gfxFont::setalpha(int type, Uint8 alpha)
{


	ik = type;
	switch (ik)
	{
	case MMX_FONT:
		if ((SDL_SetAlpha(font_mmx->Surface, SDL_SRCALPHA | SDL_RLEACCEL, alpha)) < 0)
		{
			Write_Log("\n ERROR: couldn't set alpha on sprite: %s\n", SDL_GetError());
		}
		break;
	case SMALL_FONT:
		if ((SDL_SetAlpha(font_small->Surface, SDL_SRCALPHA | SDL_RLEACCEL, alpha)) < 0)
		{
			Write_Log("\n ERROR: couldn't set alpha on sprite: %s\n", SDL_GetError());
		}
		break;
	case LARGE_FONT:
		if ((SDL_SetAlpha(font_large->Surface, SDL_SRCALPHA | SDL_RLEACCEL, alpha)) < 0)
		{
			Write_Log("\n ERROR: couldn't set alpha on sprite: %s\n", SDL_GetError());
		}
		break;
	case ROCK_FONT:
		if ((SDL_SetAlpha(m_font->Surface, SDL_SRCALPHA | SDL_RLEACCEL, alpha)) < 0)
		{
			Write_Log("\n ERROR: couldn't set alpha on sprite: %s\n", SDL_GetError());
		}
		break;
	}

}
