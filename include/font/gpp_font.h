#ifndef GPP_FONT_HPP
#define GPP_FONT_HPP

#include <SDL/SDL.h>

extern SDL_Surface *logic;

// Enum con todas las fuentes disponibles en font/type/
enum GPP_FontType {
    FONT_5 = 0,
    FONT_12,
    FONT_15,
    FONT_19,
    FONT_29,
    FONT_31,
    FONT_32,
    FONT_33,
    FONT_34,
    FONT_45,
    FONT_54,
    FONT_55,
    FONT_57,
    FONT_61,
    FONT_62,
    FONT_90,
    FONT_98,
    FONT_104,
    FONT_106,
    FONT_107,
    FONT_110,
    // Alias de compatibilidad
    FONT_NORMAL_24 = FONT_32
};

class GPP_Font {
private:
    int size;
    Uint32 color;
    int tracking;
    
    const unsigned int *current_font_data;
    int matrix_size;
    GPP_FontType current_type;

    void draw_pixel(SDL_Surface *screen, int x, int y, Uint32 col);

public:
    GPP_Font(GPP_FontType type = FONT_32, int default_size = 16, Uint32 default_color = 0xFFFFFFFF);

    void setFontType(GPP_FontType type);
    void setSize(int new_size);
    void setColor(Uint32 new_color);
    void setTracking(int new_tracking);

    // Métodos de dibujado
    void drawChar(SDL_Surface *screen, int x, int y, char ascii);
    void drawText(SDL_Surface *screen, int x, int y, const char *texto);
    void drawChar(int x, int y, char ascii);
    void drawText(int x, int y, const char *texto);

    // Métodos formato printf
    void printf(SDL_Surface *screen, int x, int y, const char *format, ...);
    void printf(int x, int y, const char *format, ...);
};

#endif