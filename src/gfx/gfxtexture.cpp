#include <gfx/gfxtexture.h>
#include <SDL/SDL_image.h>
#include <gfx/SDL_rotozoom.h>
#include <gfx/SDL_gfxPrimitives.h>
#include <cstdio>

#define nullptr NULL

// / ======================
// / Métodos privados
// / ======================

void GfxTexture::free_surface(SDL_Surface * &surf)
{
	if (surf)
	{
		SDL_FreeSurface(surf);
		surf = nullptr;
	}
}

void GfxTexture::update_pixels()
{
	pixels = work_surface ? reinterpret_cast < u32 * >(work_surface->pixels) : nullptr;
}

// / ======================
// / Constructor / Destructor
// / ======================

GfxTexture::GfxTexture():surface(nullptr), work_surface(nullptr), pixels(nullptr),
x(0), y(0), alpha(255), rotation(0.0f), scale(1.0f)
{
}

GfxTexture::~GfxTexture()
{
	free_surface(surface);
	free_surface(work_surface);
}

// / ======================
// / Inicialización / Carga
// / ======================

bool GfxTexture::init(int w, int h)
{
	free_surface(work_surface);

	work_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, 0, 0, 0, 0);
	if (!work_surface)
	{
		printf("SDL_CreateRGBSurface error: %s\n", SDL_GetError());
		return false;
	}

	update_pixels();
	return true;
}

bool GfxTexture::load_image(const char *filename)
{
	free_surface(work_surface);

	SDL_Surface *temp = IMG_Load(filename);
	if (!temp)
	{
		printf("IMG_Load error: %s\n", IMG_GetError());
		return false;
	}

	work_surface = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	update_pixels();
	return true;
}

bool GfxTexture::load_frommem(u8 * buffer, int len)
{
	free_surface(work_surface);

	SDL_RWops *mem_rwops = SDL_RWFromMem(buffer, sizeof(u8) * len);
	if (!mem_rwops)
	{
		printf("SDL_RWFromMem error: %s\n", SDL_GetError());
		return false;
	}

	SDL_Surface *temp = IMG_Load_RW(mem_rwops, 1);
	if (!temp)
	{
		printf("IMG_Load_RW error: %s\n", SDL_GetError());
		return false;
	}

	work_surface = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	update_pixels();
	return true;
}

bool GfxTexture::load_from_surface(SDL_Surface * surf)
{
	if (!surf)
		return false;

	free_surface(work_surface);

	// Copia la superficie para no modificar la original
	work_surface = SDL_ConvertSurface(surf, surf->format, surf->flags);
	if (!work_surface)
	{
		printf("SDL_ConvertSurface error: %s\n", SDL_GetError());
		return false;
	}

	update_pixels();
	return true;
}


// / ======================
// / Rellenos
// / ======================

void GfxTexture::fill(u8 r, u8 g, u8 b)
{
	if (!work_surface)
		return;

	if (SDL_MUSTLOCK(work_surface))
		SDL_LockSurface(work_surface);

	u32 color = SDL_MapRGB(work_surface->format, r, g, b);
	int total = work_surface->w * work_surface->h;
	for (int i = 0; i < total; i++)
		pixels[i] = color;

	if (SDL_MUSTLOCK(work_surface))
		SDL_UnlockSurface(work_surface);
}

void GfxTexture::fill_checkerboard(u8 r1, u8 g1, u8 b1, u8 r2, u8 g2, u8 b2, int block_size)
{
	if (!work_surface || work_surface->format->BytesPerPixel != 4)
		return;

	if (SDL_MUSTLOCK(work_surface))
		SDL_LockSurface(work_surface);

	int pitch = work_surface->pitch / 4;
	Uint32 c1 = SDL_MapRGB(work_surface->format, r1, g1, b1);
	Uint32 c2 = SDL_MapRGB(work_surface->format, r2, g2, b2);

	for (int y = 0; y < work_surface->h; y++)
	{
		for (int x = 0; x < work_surface->w; x++)
		{
			int bx = x / block_size;
			int by = y / block_size;
			pixels[y * pitch + x] = ((bx + by) % 2 == 0) ? c1 : c2;
		}
	}

	if (SDL_MUSTLOCK(work_surface))
		SDL_UnlockSurface(work_surface);
}

void GfxTexture::fill_checkerboard_colors(u32 c1, u32 c2, int block_size)
{
	if (!work_surface || work_surface->format->BytesPerPixel != 4)
		return;

	if (SDL_MUSTLOCK(work_surface))
		SDL_LockSurface(work_surface);

	int pitch = work_surface->pitch / 4;
	for (int y = 0; y < work_surface->h; y++)
	{
		for (int x = 0; x < work_surface->w; x++)
		{
			int bx = x / block_size;
			int by = y / block_size;
			pixels[y * pitch + x] = ((bx + by) % 2 == 0) ? c1 : c2;
		}
	}

	if (SDL_MUSTLOCK(work_surface))
		SDL_UnlockSurface(work_surface);
}

void GfxTexture::fill_horizontal_gradient(Uint32 color1, Uint32 color2)
{
	if (!work_surface)
		return;

	if (SDL_MUSTLOCK(work_surface))
		SDL_LockSurface(work_surface);

	Uint8 r1, g1, b1, r2, g2, b2;
	SDL_GetRGB(color1, work_surface->format, &r1, &g1, &b1);
	SDL_GetRGB(color2, work_surface->format, &r2, &g2, &b2);

	for (int x = 0; x < work_surface->w; x++)
	{
		float t = static_cast < float >(x) / (work_surface->w - 1);
		Uint8 r = static_cast < Uint8 > ((1 - t) * r1 + t * r2);
		Uint8 g = static_cast < Uint8 > ((1 - t) * g1 + t * g2);
		Uint8 b = static_cast < Uint8 > ((1 - t) * b1 + t * b2);

		Uint32 blended_color = SDL_MapRGB(work_surface->format, r, g, b);
		for (int y = 0; y < work_surface->h; y++)
			pixels[y * work_surface->w + x] = blended_color;
	}

	if (SDL_MUSTLOCK(work_surface))
		SDL_UnlockSurface(work_surface);
}

void GfxTexture::fill_vertical_gradient(Uint32 color1, Uint32 color2)
{
	if (!work_surface)
		return;

	if (SDL_MUSTLOCK(work_surface))
		SDL_LockSurface(work_surface);

	Uint8 r1, g1, b1, r2, g2, b2;
	SDL_GetRGB(color1, work_surface->format, &r1, &g1, &b1);
	SDL_GetRGB(color2, work_surface->format, &r2, &g2, &b2);

	int pitch = work_surface->pitch / 4;
	for (int y = 0; y < work_surface->h; y++)
	{
		float t = static_cast < float >(y) / (work_surface->h - 1);
		Uint8 r = static_cast < Uint8 > ((1 - t) * r1 + t * r2);
		Uint8 g = static_cast < Uint8 > ((1 - t) * g1 + t * g2);
		Uint8 b = static_cast < Uint8 > ((1 - t) * b1 + t * b2);

		Uint32 color = SDL_MapRGB(work_surface->format, r, g, b);
		for (int x = 0; x < work_surface->w; x++)
			pixels[y * pitch + x] = color;
	}

	if (SDL_MUSTLOCK(work_surface))
		SDL_UnlockSurface(work_surface);
}

// / ======================
// / Transformaciones
// / ======================

void GfxTexture::set_scale(float percent)
{
	scale = percent;
}

void GfxTexture::set_rotation(float angleDegrees)
{
	rotation = angleDegrees;
}

void GfxTexture::applyTransparency(Uint8 r, Uint8 g, Uint8 b)
{
	if (surface)
		SDL_SetColorKey(surface, SDL_SRCCOLORKEY | SDL_RLEACCEL,
						SDL_MapRGB(surface->format, r, g, b));
}

void GfxTexture::render(SDL_Surface * dst)
{
	if (!dst || !surface)
		return;

	int rotatedWidth = surface->w;
	int rotatedHeight = surface->h;
	SDL_Rect dstRect = { static_cast < Sint16 > (x - rotatedWidth / 2),
		static_cast < Sint16 > (y - rotatedHeight / 2),
		rotatedWidth, rotatedHeight
	};

	SDL_SetAlpha(surface, SDL_SRCALPHA, alpha);
	SDL_BlitSurface(surface, nullptr, dst, &dstRect);
}

void GfxTexture::set_position(int px, int py)
{
	x = px;
	y = py;
}

void GfxTexture::set_alpha(u8 a)
{
	alpha = a;
}

void GfxTexture::rotozoom()
{
	free_surface(surface);

	//surface = rotozoomSurface(work_surface, rotation, scale, SMOOTHING_ON);
	surface = rotozoomSurface(work_surface, rotation, scale,0);
	if (!surface)
	{
		printf("Error en rotozoomSurface: %s\n", SDL_GetError());
		return;
	}

	applyTransparency(0, 0, 0);
}


void GfxTexture::set_surface(SDL_Surface * src, int x, int y)
{
	if (!src || !work_surface)
		return;
	SDL_Rect pos = { (Sint16) x, (Sint16) y, 0, 0 };
	SDL_BlitSurface(src, NULL, work_surface, &pos);

}


