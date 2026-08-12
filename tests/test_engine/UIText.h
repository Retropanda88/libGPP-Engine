#ifndef UITEXT_H
#define UITEXT_H

#include "UIElement.h"
#include <font/gpp_font.h> // Ajusta el include según la ruta exacta de tu archivo de fuentes

class UIText : public UIElement {
private:
    char textContent[128];
    GPP_Font font;

    // Método interno para convertir el texto actual en una SDL_Surface
    void updateTextSurface();

public:
    UIText(int16_t x, int16_t y, const char* text, GPP_FontType fontType = FONT_32, UIElement* parent = NULL);
    virtual ~UIText();

    // Cambiar el texto dinámicamente (estilo RPG / máquina de escribir)
    void setText(const char* text);
    const char* getText() const;

    GPP_Font& getFont();
    void setFontType(GPP_FontType type);
    void setFontSize(int size);
    void setFontColor(Uint32 color);
};
#endif // UITEXT_H