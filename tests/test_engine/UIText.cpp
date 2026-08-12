#include "UIText.h"
#include <string.h>

UIText::UIText(int16_t x, int16_t y, const char* text, GPP_FontType fontType, UIElement* parent) 
    : UIElement(x, y, parent), font(fontType) {
    
    if (text != NULL) {
        strncpy(this->textContent, text, sizeof(this->textContent) - 1);
        this->textContent[sizeof(this->textContent) - 1] = '\0';
    } else {
        this->textContent[0] = '\0';
    }

    updateTextSurface();
}

UIText::~UIText() {
    // Limpieza si es requerida
}

void UIText::setText(const char* text) {
    if (text != NULL) {
        strncpy(this->textContent, text, sizeof(this->textContent) - 1);
        this->textContent[sizeof(this->textContent) - 1] = '\0';
        updateTextSurface();
    }
}

const char* UIText::getText() const {
    return this->textContent;
}

GPP_Font& UIText::getFont() {
    return this->font;
}

void UIText::setFontType(GPP_FontType type) {
    this->font.setFontType(type);
    updateTextSurface();
}

void UIText::setFontSize(int size) {
    this->font.setSize(size);
    updateTextSurface();
}

void UIText::setFontColor(Uint32 color) {
    this->font.setColor(color);
    updateTextSurface();
}

void UIText::updateTextSurface() {
    int width = 300;
    int height = 40;

    //validamos a logic
    if (logic == NULL || logic->format == NULL) return;

    //generamos una surface 
    SDL_Surface* textSurf = create_surface(width, height, SDL_SWSURFACE);
    if (textSurf == NULL) return;

    // llenamos la supeficie con color magenta 
    SDL_FillRect(textSurf, NULL, SDL_MapRGB(textSurf->format, 255, 0, 255));
    // Dibujamos el texto
    this->font.drawText(textSurf, 0, 0, this->textContent);
    //aplicamos trasparencia al texto
    apply_transparency(textSurf,255,0,255);
    // Pasamos la superficie al UIElement
    this->setImageFromSurface(textSurf);
}