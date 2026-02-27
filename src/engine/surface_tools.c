	/* 
	 * libGPP-Engine - A lightweight static game engine for retro consoles.
	 * FIXES APLICADOS (SIN CAMBIAR NOMBRES NI API):
	 *  - Se eliminan fugas por SDL_DisplayFormat
	 *  - Se corrigen returns inválidos
	 *  - Se asegura liberación de superficies temporales
	 *  - NO se cambió ningún nombre de función
	 */
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <gfx/SDL_rotozoom.h>
#include <gfx/SDL_gfxPrimitives.h>
#include <video/video.h>
#include <engine/types.h>
#include <math.h>
#include <video/pixel.h>


#define FP 16
#define F1 (1 << FP)

extern SDL_Surface *vram;

/* --------------------------------------------------------- */
/* load_img FIX */
/* --------------------------------------------------------- */
SDL_Surface *load_img(const char *file)
{
	SDL_Surface *raw = IMG_Load(file);
	if (!raw)
	{
		printf("IMG_Load error: %s\n", SDL_GetError());
		return NULL;
	}

	SDL_Surface *fmt = SDL_DisplayFormat(raw);
	SDL_FreeSurface(raw);		/* FIX fuga */
	return fmt;
}

/* --------------------------------------------------------- */
int AplyTransparency(SDL_Surface * src, u8 r, u8 g, u8 b)
{
	return SDL_SetColorKey(src, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(src->format, r, g, b));
}

/* --------------------------------------------------------- */
void draw_surface(SDL_Surface * src, int x, int y)
{
	SDL_Rect rect = { x, y, 0, 0 };
	SDL_BlitSurface(src, NULL, logic, &rect);
}

/* --------------------------------------------------------- */
/* create_surface FIX */
/* --------------------------------------------------------- */
SDL_Surface *create_surface(int w, int h, Uint32 flag)
{
	SDL_Surface *s = NULL;

#if defined(PSP_BUILD)

	/* PSP: usar EXACTAMENTE el mismo formato que el framebuffer */
	s = SDL_CreateRGBSurface(SDL_SWSURFACE,
							 w,
							 h,
							 vram->format->BitsPerPixel,
							 vram->format->Rmask,
							 vram->format->Gmask, vram->format->Bmask, vram->format->Amask);

	if (!s)
	{
		printf("PSP CreateRGBSurface error: %s\n", SDL_GetError());
		return NULL;
	}

	return s;

#else

	/* PC / PS2 / GameCube */
	SDL_Surface *raw = SDL_CreateRGBSurface(flag,
											w,
											h,
											vram->format->BitsPerPixel,
											vram->format->Rmask,
											vram->format->Gmask,
											vram->format->Bmask,
											vram->format->Amask);

	if (!raw)
	{
		printf("CreateRGBSurface error: %s\n", SDL_GetError());
		return NULL;
	}

	/* Acelera blits en plataformas no PSP */
	s = SDL_DisplayFormat(raw);
	SDL_FreeSurface(raw);

	if (!s)
	{
		printf("DisplayFormat error: %s\n", SDL_GetError());
		return NULL;
	}

	return s;

#endif
}

/* --------------------------------------------------------- */
// fill rect full speed 

void fill_rect(SDL_Surface * dst, int x, int y, int w, int h, u32 c)
{
	if (!dst || w <= 0 || h <= 0)
		return;

	/* ---------- CLIPPING ---------- */

	if (x < 0)
	{
		w += x;
		x = 0;
	}
	if (y < 0)
	{
		h += y;
		y = 0;
	}

	if (x + w > dst->w)
		w = dst->w - x;
	if (y + h > dst->h)
		h = dst->h - y;

	if (w <= 0 || h <= 0)
		return;

	if (SDL_MUSTLOCK(dst))
		SDL_LockSurface(dst);

#if defined(PSP_BUILD)

	// fb ya esta declarado
	u16 color = (u16) c;
	int pitch = dst->pitch >> 1;
	int dy;
	for (dy = 0; dy < h; dy++)
	{
		u16 *row = fb + (y + dy) * pitch + x;
		int dx;
		for (dx = 0; dx < w; dx++)
			row[dx] = color;
	}

#else
	// fb ya esta declarado
	u32 color = c;
	int pitch = dst->pitch >> 2;
  int dy;
	for (dy = 0; dy < h; dy++)
	{
		u32 *row = fb + (y + dy) * pitch + x;
		int dx;
		for (dx = 0; dx < w; dx++)
			row[dx] = color;
	}

#endif

	if (SDL_MUSTLOCK(dst))
		SDL_UnlockSurface(dst);
}



/* --------------------------------------------------------- */
void fill_texture(SDL_Surface * src, int w, int h, Uint32 c1, Uint32 c2)
{
	if (!src)
		return;

	if (SDL_MUSTLOCK(src))
		SDL_LockSurface(src);

	int x, y;

#if defined(PSP_BUILD)

	/* ================= PSP (RGB565) ================= */

	Uint16 *pixels = (Uint16 *) src->pixels;
	int pitch = src->pitch >> 1;	// bytes → Uint16

	/* c1 y c2 YA están en RGB565 */
	Uint16 c1_16 = (Uint16) c1;
	Uint16 c2_16 = (Uint16) c2;

	for (y = 0; y < h; y++)
		for (x = 0; x < w; x++)
			pixels[y * pitch + x] = ((x + y) & 1) ? c2_16 : c1_16;

#else

	/* ============== PC / PS2 / GC (32 bpp) ============== */

	Uint32 *pixels = (Uint32 *) src->pixels;
	int pitch = src->pitch >> 2;

	for (y = 0; y < h; y++)
		for (x = 0; x < w; x++)
			pixels[y * pitch + x] = ((x + y) & 1) ? c2 : c1;

#endif

	if (SDL_MUSTLOCK(src))
		SDL_UnlockSurface(src);
}



/* --------------------------------------------------------- */
void fill_textureRect(SDL_Surface * src, int w, int h, Uint32 c1, Uint32 c2, int block_size)
{
	if (!src || block_size <= 0)
		return;

	if (SDL_MUSTLOCK(src))
		SDL_LockSurface(src);

#if defined(PSP_BUILD)

	/* PSP: RGB565 (16 bits) */
	Uint16 *pixels = (Uint16 *) src->pixels;
	int pitch = src->pitch >> 1;	// /2

	Uint16 c1_16 = (Uint16) c1;
	Uint16 c2_16 = (Uint16) c2;
	int y;
	for (y = 0; y < h; y++)
	{
		Uint16 *row = pixels + y * pitch;
		int by = (y / block_size) & 1;

		int x = 0;
		while (x < w)
		{
			int bx = (x / block_size) & 1;
			Uint16 color = (bx ^ by) ? c2_16 : c1_16;

			int limit = x + block_size;
			if (limit > w)
				limit = w;

			while (x < limit)
				row[x++] = color;
		}
	}

#else

	/* PC / PS2 / GameCube: 32 bits */
	if (src->format->BytesPerPixel != 4)
		goto end;

	Uint32 *pixels = (Uint32 *) src->pixels;
	int pitch = src->pitch >> 2;	// /4
	int y;
	for (y = 0; y < h; y++)
	{
		Uint32 *row = pixels + y * pitch;
		int by = (y / block_size) & 1;

		int x = 0;
		while (x < w)
		{
			int bx = (x / block_size) & 1;
			Uint32 color = (bx ^ by) ? c2 : c1;

			int limit = x + block_size;
			if (limit > w)
				limit = w;

			while (x < limit)
				row[x++] = color;
		}
	}

#endif

  end:
	if (SDL_MUSTLOCK(src))
		SDL_UnlockSurface(src);
}

/*-------------------------------------------------------------*/

void draw_line_fast(SDL_Surface * dst, int x0, int y0, int x1, int y1, u32 c)
{
	if (!dst)
		return;

	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);

	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;

	int err = dx - dy;

	if (SDL_MUSTLOCK(dst))
		SDL_LockSurface(dst);

#if defined(PSP_BUILD)
	u16 *fb = (u16 *) dst->pixels;
	u16 color = (u16) c;
	int pitch = dst->pitch >> 1;
#else
	u32 *fb = (u32 *) dst->pixels;
	u32 color = c;
	int pitch = dst->pitch >> 2;
#endif

	while (1)
	{
		// Clipping simple
		if ((unsigned)x0 < (unsigned)dst->w && (unsigned)y0 < (unsigned)dst->h)
		{
#if defined(PSP_BUILD)
			fb[y0 * pitch + x0] = color;
#else
			fb[y0 * pitch + x0] = color;
#endif
		}

		if (x0 == x1 && y0 == y1)
			break;

		int e2 = err << 1;

		if (e2 > -dy)
		{
			err -= dy;
			x0 += sx;
		}

		if (e2 < dx)
		{
			err += dx;
			y0 += sy;
		}
	}

	if (SDL_MUSTLOCK(dst))
		SDL_UnlockSurface(dst);
}


/*-------------------------------------------------*/
void draw_line_gradient(SDL_Surface * dst, int x0, int y0, int x1, int y1, u32 c0, u32 c1)
{
	if (!dst)
		return;

	if (SDL_MUSTLOCK(dst))
		SDL_LockSurface(dst);

	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);

	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;

	int err = dx - dy;

	/* Extraemos colores */
	u8 r0, g0, b0;
	u8 r1, g1, b1;

	SDL_GetRGB(c0, dst->format, &r0, &g0, &b0);
	SDL_GetRGB(c1, dst->format, &r1, &g1, &b1);

	int steps = (dx > dy) ? dx : dy;
	if (steps == 0)
		steps = 1;

	int r = r0 << 8;
	int g = g0 << 8;
	int b = b0 << 8;

	int dr = ((int)(r1 - r0) << 8) / steps;
	int dg = ((int)(g1 - g0) << 8) / steps;
	int db = ((int)(b1 - b0) << 8) / steps;

#if defined(PSP_BUILD)
	u16 *pixels = (u16 *) dst->pixels;
	int pitch = dst->pitch >> 1;
#else
	u32 *pixels = (u32 *) dst->pixels;
	int pitch = dst->pitch >> 2;
#endif

	while (1)
	{
		if (x0 >= 0 && x0 < dst->w && y0 >= 0 && y0 < dst->h)
		{
#if defined(PSP_BUILD)
			u16 color = ((r >> 11) & 0x1F) << 11 | ((g >> 10) & 0x3F) << 5 | ((b >> 11) & 0x1F);
			pixels[y0 * pitch + x0] = color;
#else
			pixels[y0 * pitch + x0] =
				(r >> 8) << dst->format->Rshift |
				(g >> 8) << dst->format->Gshift | (b >> 8) << dst->format->Bshift;
#endif
		}

		if (x0 == x1 && y0 == y1)
			break;

		int e2 = err << 1;

		if (e2 > -dy)
		{
			err -= dy;
			x0 += sx;
		}
		if (e2 < dx)
		{
			err += dx;
			y0 += sy;
		}

		r += dr;
		g += dg;
		b += db;
	}

	if (SDL_MUSTLOCK(dst))
		SDL_UnlockSurface(dst);
}


/*-------------------------------------------------*/
void draw_triangle(SDL_Surface * dst, int x0, int y0, int x1, int y1, int x2, int y2, u32 color)
{
	if (!dst)
		return;

	// AB
	draw_line_fast(dst, x0, y0, x1, y1, color);

	// BC
	draw_line_fast(dst, x1, y1, x2, y2, color);

	// CA
	draw_line_fast(dst, x2, y2, x0, y0, color);
}


/*---------------------------------------------*/


static inline void hline_fast(SDL_Surface * dst, int x0, int x1, int y, u32 color)
{
	if ((unsigned)y >= (unsigned)dst->h)
		return;

	if (x0 > x1)
	{
		int t = x0;
		x0 = x1;
		x1 = t;
	}

	if (x0 < 0)
		x0 = 0;
	if (x1 >= dst->w)
		x1 = dst->w - 1;

#if defined(PSP_BUILD)
	u16 *row = (u16 *) dst->pixels + y * (dst->pitch >> 1) + x0;
	u16 c = (u16) color;
	int x;
	for (x = x0; x <= x1; x++)
		*row++ = c;
#else
	u32 *row = (u32 *) dst->pixels + y * (dst->pitch >> 2) + x0;
	int x;
	for (x = x0; x <= x1; x++)
		*row++ = color;
#endif
}


static inline void hline_gradient(SDL_Surface * dst,
								  int x0, int x1, int y,
								  int r0, int g0, int b0, int r1, int g1, int b1)
{
	if ((unsigned)y >= (unsigned)dst->h)
		return;

	if (x0 > x1)
	{
		int t;
		t = x0;
		x0 = x1;
		x1 = t;
		t = r0;
		r0 = r1;
		r1 = t;
		t = g0;
		g0 = g1;
		g1 = t;
		t = b0;
		b0 = b1;
		b1 = t;
	}

	if (x0 < 0)
		x0 = 0;
	if (x1 >= dst->w)
		x1 = dst->w - 1;

	int len = x1 - x0;
	if (len <= 0)
		return;

	int dr = ((r1 - r0) << FP) / len;
	int dg = ((g1 - g0) << FP) / len;
	int db = ((b1 - b0) << FP) / len;

	int r = r0 << FP;
	int g = g0 << FP;
	int b = b0 << FP;

#if defined(PSP_BUILD)
	u16 *p = (u16 *) dst->pixels + y * (dst->pitch >> 1) + x0;
	int x;
	for (x = x0; x <= x1; x++)
	{
		*p++ = ((r >> FP) << 11) | ((g >> FP) << 5) | (b >> FP);
		r += dr;
		g += dg;
		b += db;
	}
#else
	u32 *p = (u32 *) dst->pixels + y * (dst->pitch >> 2) + x0;
	int x;
	for (x = x0; x <= x1; x++)
	{
		*p++ = SDL_MapRGB(dst->format, r >> FP, g >> FP, b >> FP);
		r += dr;
		g += dg;
		b += db;
	}
#endif
}


void fill_triangle_fast(SDL_Surface * dst,
						int x0, int y0, int x1, int y1, int x2, int y2, u32 color)
{
	if (!dst)
		return;

	// Ordenar por Y
	if (y1 < y0)
	{
		int t = x0;
		x0 = x1;
		x1 = t;
		t = y0;
		y0 = y1;
		y1 = t;
	}
	if (y2 < y0)
	{
		int t = x0;
		x0 = x2;
		x2 = t;
		t = y0;
		y0 = y2;
		y2 = t;
	}
	if (y2 < y1)
	{
		int t = x1;
		x1 = x2;
		x2 = t;
		t = y1;
		y1 = y2;
		y2 = t;
	}

	if (y0 == y2)
		return;

	// Pendientes X en fixed
	int dx02 = ((x2 - x0) << FP) / (y2 - y0);
	int dx01 = 0;
	int dx12 = 0;

	if (y1 != y0)
		dx01 = ((x1 - x0) << FP) / (y1 - y0);
	if (y2 != y1)
		dx12 = ((x2 - x1) << FP) / (y2 - y1);

	int xl = x0 << FP;
	int xr = xl;

	// Parte superior
	int y;
	for (y = y0; y < y1; y++)
	{
		hline_fast(dst, xl >> FP, xr >> FP, y, color);
		xl += dx02;
		xr += dx01;
	}

	xr = x1 << FP;

	// Parte inferior
	
	for (y = y1; y <= y2; y++)
	{
		hline_fast(dst, xl >> FP, xr >> FP, y, color);
		xl += dx02;
		xr += dx12;
	}
}



void fill_triangle_gradient(SDL_Surface * dst,
							int x0, int y0, int r0, int g0, int b0,
							int x1, int y1, int r1, int g1, int b1,
							int x2, int y2, int r2, int g2, int b2)
{
	if (!dst)
		return;

	// Ordenar por Y
#define SWAP(a,b) { int t=a; a=b; b=t; }
	if (y1 < y0)
	{
		SWAP(x0, x1);
		SWAP(y0, y1);
		SWAP(r0, r1);
		SWAP(g0, g1);
		SWAP(b0, b1);
	}
	if (y2 < y0)
	{
		SWAP(x0, x2);
		SWAP(y0, y2);
		SWAP(r0, r2);
		SWAP(g0, g2);
		SWAP(b0, b2);
	}
	if (y2 < y1)
	{
		SWAP(x1, x2);
		SWAP(y1, y2);
		SWAP(r1, r2);
		SWAP(g1, g2);
		SWAP(b1, b2);
	}

	int dy02 = y2 - y0;
	if (dy02 == 0)
		return;

	int dx02 = ((x2 - x0) << FP) / dy02;
	int dr02 = ((r2 - r0) << FP) / dy02;
	int dg02 = ((g2 - g0) << FP) / dy02;
	int db02 = ((b2 - b0) << FP) / dy02;

	int dx01 = 0, dr01 = 0, dg01 = 0, db01 = 0;
	if (y1 != y0)
	{
		int dy01 = y1 - y0;
		dx01 = ((x1 - x0) << FP) / dy01;
		dr01 = ((r1 - r0) << FP) / dy01;
		dg01 = ((g1 - g0) << FP) / dy01;
		db01 = ((b1 - b0) << FP) / dy01;
	}

	int xl = x0 << FP, xr = xl;
	int rl = r0 << FP, rr = rl;
	int gl = g0 << FP, gr = gl;
	int bl = b0 << FP, br = bl;

	// Parte superior
	int y;
	for (y = y0; y < y1; y++)
	{
		hline_gradient(dst, xl >> FP, xr >> FP, y,
					   rl >> FP, gl >> FP, bl >> FP, rr >> FP, gr >> FP, br >> FP);
		xl += dx02;
		rl += dr02;
		gl += dg02;
		bl += db02;
		xr += dx01;
		rr += dr01;
		gr += dg01;
		br += db01;
	}

	// Recalcular borde corto
	if (y2 != y1)
	{
		int dy12 = y2 - y1;
		dx01 = ((x2 - x1) << FP) / dy12;
		dr01 = ((r2 - r1) << FP) / dy12;
		dg01 = ((g2 - g1) << FP) / dy12;
		db01 = ((b2 - b1) << FP) / dy12;

		xr = x1 << FP;
		rr = r1 << FP;
		gr = g1 << FP;
		br = b1;
		int y;
		for (y = y1; y <= y2; y++)
		{
			hline_gradient(dst, xl >> FP, xr >> FP, y,
						   rl >> FP, gl >> FP, bl >> FP, rr >> FP, gr >> FP, br >> FP);
			xl += dx02;
			rl += dr02;
			gl += dg02;
			bl += db02;
			xr += dx01;
			rr += dr01;
			gr += dg01;
			br += db01;
		}
	}
}

/* --------------------------------------------------------- */
void apply_alpha(SDL_Surface * src, u8 alpha)
{
	SDL_SetAlpha(src, SDL_SRCALPHA, alpha);
}

/* --------------------------------------------------------- */
void fill_horizontal_gradient(SDL_Surface * surface, Uint32 color1, Uint32 color2)
{
	if (!surface)
		return;

	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);

	int w = surface->w;
	int h = surface->h;

	Uint8 r1, g1, b1;
	Uint8 r2, g2, b2;

	SDL_GetRGB(color1, surface->format, &r1, &g1, &b1);
	SDL_GetRGB(color2, surface->format, &r2, &g2, &b2);

#if defined(PSP_BUILD)
	/* ================= PSP (BGR565) ================= */

	Uint16 *pixels = (Uint16 *) surface->pixels;
	int pitch = surface->pitch >> 1;	// /2
	int x;
	for (x = 0; x < w; x++)
	{
		/* Interpolación ENTERA (sin float) */
		int r = r1 + ((r2 - r1) * x) / (w - 1);
		int g = g1 + ((g2 - g1) * x) / (w - 1);
		int b = b1 + ((b2 - b1) * x) / (w - 1);

		//Uint16 c = ((r >> 3) << 11) | ((g >> 2) << 5) | ((b >> 3));
		Uint32 c = SDL_MapRGB(surface->format, r, g, b);
		int y;
		for (y = 0; y < h; y++)
			pixels[y * pitch + x] = (Uint16)c;
	}

#else
	/* ================= 32 BPP (PC / PS2 / GC) ================= */

	Uint32 *pixels = (Uint32 *) surface->pixels;
	int pitch = surface->pitch >> 2;	// /4
	int x;
	for (x = 0; x < w; x++)
	{
		int r = r1 + ((r2 - r1) * x) / (w - 1);
		int g = g1 + ((g2 - g1) * x) / (w - 1);
		int b = b1 + ((b2 - b1) * x) / (w - 1);

		Uint32 c = SDL_MapRGB(surface->format, r, g, b);
		int y;
		for (y = 0; y < h; y++)
			pixels[y * pitch + x] = c;
	}
#endif

	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
}


/* --------------------------------------------------------- */
void fill_vertical_gradient(SDL_Surface * surface, Uint32 color1, Uint32 color2)
{
	if (!surface)
		return;

	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);

	int w = surface->w;
	int h = surface->h;
	if (h <= 1)
		h = 2;

	Uint8 r1, g1, b1;
	Uint8 r2, g2, b2;

	SDL_GetRGB(color1, surface->format, &r1, &g1, &b1);
	SDL_GetRGB(color2, surface->format, &r2, &g2, &b2);

#if defined(PSP_BUILD)
	/* ================= PSP (BGR565) ================= */

	Uint16 *pixels = (Uint16 *) surface->pixels;
	int pitch = surface->pitch >> 1;	// /2
	int y;
	for (y = 0; y < h; y++)
	{
		int t = (y * 255) / (h - 1);

		int r = (r1 * (255 - t) + r2 * t) >> 8;
		int g = (g1 * (255 - t) + g2 * t) >> 8;
		int b = (b1 * (255 - t) + b2 * t) >> 8;

		//Uint16 c = ((r >> 3) << 11) | ((g >> 2) << 5) | ((b >> 3));
		Uint32 c = SDL_MapRGB(surface->format, r, g, b);

		Uint16 *row = pixels + y * pitch;
		int x;
		for (x = 0; x < w; x++)
			row[x] = (Uint16)c;
	}

#else
	/* ================= 32 BPP (PC / PS2 / GC) ================= */

	Uint32 *pixels = (Uint32 *) surface->pixels;
	int pitch = surface->pitch >> 2;	// /4

	Uint32 a = surface->format->Amask;
	int y;
	for (y = 0; y < h; y++)
	{
		int t = (y * 255) / (h - 1);

		int r = (r1 * (255 - t) + r2 * t) >> 8;
		int g = (g1 * (255 - t) + g2 * t) >> 8;
		int b = (b1 * (255 - t) + b2 * t) >> 8;

		Uint32 c =
			a |
			(r << surface->format->Rshift) |
			(g << surface->format->Gshift) | (b << surface->format->Bshift);

		Uint32 *row = pixels + y * pitch;
		int x;
		for (x = 0; x < w; x++)
			row[x] = c;
	}
#endif

	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
}


/* --------------------------------------------------------- */
void fill_radial_gradient(SDL_Surface * surface, Uint32 color1, Uint32 color2)
{
	if (!surface)
		return;

	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);

	int w = surface->w;
	int h = surface->h;

	Uint8 r1, g1, b1;
	Uint8 r2, g2, b2;

	SDL_GetRGB(color1, surface->format, &r1, &g1, &b1);
	SDL_GetRGB(color2, surface->format, &r2, &g2, &b2);

	int cx = w >> 1;
	int cy = h >> 1;

	int maxd2 = cx * cx + cy * cy;
	if (maxd2 == 0)
		maxd2 = 1;

#if defined(PSP_BUILD)
	/* ================= PSP (BGR565) ================= */

	Uint16 *pixels = (Uint16 *) surface->pixels;
	int pitch = surface->pitch >> 1;	// /2
	int y;
	for (y = 0; y < h; y++)
	{
		int dy = y - cy, x;
		for (x = 0; x < w; x++)
		{
			int dx = x - cx;
			int d2 = dx * dx + dy * dy;
			if (d2 > maxd2)
				d2 = maxd2;

			int t = (d2 * 255) / maxd2;

			int r = (r1 * (255 - t) + r2 * t) >> 8;
			int g = (g1 * (255 - t) + g2 * t) >> 8;
			int b = (b1 * (255 - t) + b2 * t) >> 8;

			Uint32 mapped = SDL_MapRGB(surface->format, r, g, b);
			pixels[y * pitch + x] = (Uint16)mapped;


		}
	}

#else
	/* ================= 32 BPP (PC / PS2 / GC) ================= */

	Uint32 *pixels = (Uint32 *) surface->pixels;
	int pitch = surface->pitch >> 2;	// /4
	int y;
	for (y = 0; y < h; y++)
	{
		int dy = y - cy;
		int x;
		for (x = 0; x < w; x++)
		{
			int dx = x - cx;
			int d2 = dx * dx + dy * dy;
			if (d2 > maxd2)
				d2 = maxd2;

			int t = (d2 * 255) / maxd2;

			int r = (r1 * (255 - t) + r2 * t) >> 8;
			int g = (g1 * (255 - t) + g2 * t) >> 8;
			int b = (b1 * (255 - t) + b2 * t) >> 8;

			pixels[y * pitch + x] =
				(r << surface->format->Rshift) |
				(g << surface->format->Gshift) | (b << surface->format->Bshift);
		}
	}
#endif

	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
}



/* --------------------------------------------------------- */
/* Load_Textura_From_Mem FIX */
/* --------------------------------------------------------- */
SDL_Surface *load_texture_from_mem(u8 * buffer, int len)
{
	SDL_RWops *rw = SDL_RWFromMem(buffer, len);
	if (!rw)
	{
		printf("RWFromMem error: %s\n", SDL_GetError());
		return NULL;
	}

	SDL_Surface *raw = IMG_Load_RW(rw, 1);
	if (!raw)
	{
		printf("IMG_Load_RW error: %s\n", SDL_GetError());
		return NULL;
	}

	SDL_Surface *fmt = SDL_DisplayFormat(raw);
	SDL_FreeSurface(raw);		/* FIX fuga */
	return fmt;
}

/* --------------------------------------------------------- */
/* rotozoom FIX */
/* --------------------------------------------------------- */
SDL_Surface *rotozoom_create(SDL_Surface * src, double angle, double zoom)
{
	SDL_Surface *temp = rotozoomSurface(src, angle, zoom, SMOOTHING_ON);
	if (!temp)
	{
		printf("rotozoomSurface error: %s\n", SDL_GetError());
		return NULL;			/* FIX */
	}

	AplyTransparency(temp, 0, 0, 0);
	return temp;
}

void rotozoom_set_position(SDL_Surface * src, int x, int y)
{
	SDL_Rect d = { x - src->w / 2, y - src->h / 2, src->w, src->h };
	SDL_BlitSurface(src, NULL, logic, &d);
}

void rotozoom_destroy(SDL_Surface * src)
{
	if (src)
		SDL_FreeSurface(src);
}

/* --------------------------------------------------------- */
void set_color(Uint32 color, SDL_PixelFormat * format, Uint8 * r, Uint8 * g, Uint8 * b)
{
	SDL_GetRGB(color, format, r, g, b);
}

SDL_Surface *cut_surface(SDL_Surface * src, int x, int y, int w, int h)
{
	if (!src)
		return NULL;

	SDL_Surface *cut = SDL_CreateRGBSurface(0, w, h,
											src->format->BitsPerPixel,
											src->format->Rmask,
											src->format->Gmask,
											src->format->Bmask,
											src->format->Amask);

	if (!cut)
		return NULL;

	SDL_Rect r = { x, y, w, h };
	SDL_BlitSurface(src, &r, cut, NULL);
	return cut;
}

void set_surface(SDL_Surface * s, int x, int y)
{
	SDL_Rect dst = { x, y, 0, 0 };
	SDL_BlitSurface(s, NULL, vram, &dst);
}

void draw_surface_region(SDL_Surface * src, int sx, int sy, int sw, int sh, int dx, int dy)
{
	SDL_Rect sr = { sx, sy, sw, sh };
	SDL_Rect dr = { dx, dy, sw, sh };
	SDL_BlitSurface(src, &sr, logic, &dr);
}

void draw_line(int x0, int y0, int x1, int y1, u32 color)
{
    int dx = x1 - x0;
    int dy = y1 - y0;

    int sx = (dx >= 0) ? 1 : -1;
    int sy = (dy >= 0) ? 1 : -1;

    dx = (dx >= 0) ? dx : -dx;
    dy = (dy >= 0) ? dy : -dy;

    int err = dx - dy;

    for (;;)
    {
        pixel(logic,x0, y0, color);

        if (x0 == x1 && y0 == y1)
            break;

        int e2 = err << 1;

        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}
