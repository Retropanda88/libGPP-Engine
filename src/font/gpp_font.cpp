#include <font/gpp_font.h>
#include <cstdio>
#include <cstdarg>

// Inclusión de las 21 fuentes bitmap desde include/font/type/
#include <font/type/font_5.h>
#include <font/type/font_12.h>
#include <font/type/font_15.h>
#include <font/type/font_19.h>
#include <font/type/font_29.h>
#include <font/type/font_31.h>
#include <font/type/font_32.h>
#include <font/type/font_33.h>
#include <font/type/font_34.h>
#include <font/type/font_45.h>
#include <font/type/font_54.h>
#include <font/type/font_55.h>
#include <font/type/font_57.h>
#include <font/type/font_61.h>
#include <font/type/font_62.h>
#include <font/type/font_90.h>
#include <font/type/font_98.h>
#include <font/type/font_104.h>
#include <font/type/font_106.h>
#include <font/type/font_107.h>
#include <font/type/font_110.h>

GPP_Font::GPP_Font(GPP_FontType type, int default_size, Uint32 default_color) {
    size = (default_size < 8) ? 8 : default_size;
    setColor(default_color);
    tracking = 2;
    setFontType(type);
}

void GPP_Font::setFontType(GPP_FontType type) {
    current_type = type;
    switch (type) {
        case FONT_5:   current_font_data = font_data_5;   matrix_size = 24; break;
        case FONT_12:  current_font_data = font_data_12;  matrix_size = 24; break;
        case FONT_15:  current_font_data = font_data_15;  matrix_size = 24; break;
        case FONT_19:  current_font_data = font_data_19;  matrix_size = 24; break;
        case FONT_29:  current_font_data = font_data_29;  matrix_size = 24; break;
        case FONT_31:  current_font_data = font_data_31;  matrix_size = 24; break;
        case FONT_32:  current_font_data = font_data_32;  matrix_size = 24; break;
        case FONT_33:  current_font_data = font_data_33;  matrix_size = 24; break;
        case FONT_34:  current_font_data = font_data_34;  matrix_size = 24; break;
        case FONT_45:  current_font_data = font_data_45;  matrix_size = 24; break;
        case FONT_54:  current_font_data = font_data_54;  matrix_size = 24; break;
        case FONT_55:  current_font_data = font_data_55;  matrix_size = 24; break;
        case FONT_57:  current_font_data = font_data_57;  matrix_size = 24; break;
        case FONT_61:  current_font_data = font_data_61;  matrix_size = 24; break;
        case FONT_62:  current_font_data = font_data_62;  matrix_size = 24; break;
        case FONT_90:  current_font_data = font_data_90;  matrix_size = 24; break;
        case FONT_98:  current_font_data = font_data_98;  matrix_size = 24; break;
        case FONT_104: current_font_data = font_data_104; matrix_size = 24; break;
        case FONT_106: current_font_data = font_data_106; matrix_size = 24; break;
        case FONT_107: current_font_data = font_data_107; matrix_size = 24; break;
        case FONT_110: current_font_data = font_data_110; matrix_size = 24; break;
        default:       current_font_data = font_data_32;  matrix_size = 24; break;
    }
}

void GPP_Font::draw_pixel(SDL_Surface *screen, int x, int y, Uint32 col) {
    if (!screen) return;
    if (x < 0 || x >= screen->w || y < 0 || y >= screen->h) return;
    
    Uint16 *bufp = (Uint16 *)screen->pixels + y * (screen->pitch / 2) + x;
    *bufp = (Uint16)col; 
}

void GPP_Font::setSize(int new_size) { size = (new_size < 8) ? 8 : new_size; }

void GPP_Font::setColor(Uint32 new_color) { 
    // Si recibe un color en 32 bits (ARGB8888), lo convierte automáticamente a RGB565 (16 bpp)
    if (new_color > 0xFFFF) {
        Uint8 r = (new_color >> 16) & 0xFF;
        Uint8 g = (new_color >> 8) & 0xFF;
        Uint8 b = new_color & 0xFF;
        color = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    } else {
        color = new_color;
    }
}

void GPP_Font::setTracking(int new_tracking) { tracking = new_tracking; }

void GPP_Font::drawChar(SDL_Surface *screen, int x, int y, char ascii) {
    if (!current_font_data || !screen) return;
    unsigned char uc = (unsigned char)ascii;

    Uint16 *pixels = (Uint16 *)screen->pixels;
    int pitch_pixels = screen->pitch / 2;
    int screen_w = screen->w;
    int screen_h = screen->h;

    const unsigned int *char_ptr = &current_font_data[uc * matrix_size];

    for (int Y = 0; Y < size; ++Y) {
        int screen_y = y + Y;
        if (screen_y < 0 || screen_y >= screen_h) continue;

        int H = (matrix_size * Y) / size;
        if (H >= matrix_size) continue;

        unsigned int fila_bits = char_ptr[H];
        Uint16 *row_dest = pixels + screen_y * pitch_pixels;

        for (int X = 0; X < size; ++X) {
            int screen_x = x + X;
            if (screen_x < 0 || screen_x >= screen_w) continue;

            int W = (matrix_size * X) / size;
            if (W < matrix_size && (fila_bits & (1U << (31 - W)))) {
                row_dest[screen_x] = (Uint16)color;
            }
        }
    }
}

void GPP_Font::drawText(SDL_Surface *screen, int x, int y, const char *texto) {
    int cursor_x = x;
    while (*texto) {
        if (*texto == ' ') {
            cursor_x += size / 2;
        } else {
            drawChar(screen, cursor_x, y, *texto);
            int avance = size - (size / 4);
            if (avance < 4) avance = size - 1;
            cursor_x += avance;
        }
        texto++;
    }
}

void GPP_Font::drawChar(int x, int y, char ascii) {
    drawChar(logic, x, y, ascii);
}

void GPP_Font::drawText(int x, int y, const char *texto) {
    drawText(logic, x, y, texto);
}

void GPP_Font::printf(SDL_Surface *screen, int x, int y, const char *format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    drawText(screen, x, y, buffer);
}

void GPP_Font::printf(int x, int y, const char *format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    drawText(logic, x, y, buffer);
}