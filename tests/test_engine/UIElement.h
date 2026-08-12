#ifndef UIELEMENT_H
#define UIELEMENT_H

#include <stdint.h>
#include <engine/engine.h>

#ifndef NULL
#define NULL 0
#endif

#define MAX_CHILDREN 10

class UIElement {
protected:
    int16_t x, y;
    float scale;
    float rotation;
    uint8_t alpha;
    
    // Variables de Fundido (Fade)
    uint8_t targetAlpha;
    bool hasAlphaTarget;
    float alphaSpeed;

    // Variables de Rotación
    float targetRotation;
    bool hasRotationTarget;
    float rotationSpeed;

    //funciones de escalado
    float targetScale;
    bool hasScaleTarget;
    float scaleSpeed;

    SDL_Surface* surface;
    
    UIElement* parent;
    UIElement* children[MAX_CHILDREN];
    int childCount;

public:
    UIElement(int16_t x, int16_t y, UIElement* parent = NULL);
    virtual ~UIElement();

    bool addChild(UIElement* child);
    bool setImage(const char* filepath);
    bool setImageFromSurface(SDL_Surface* surf);

    // Setters básicos y Consultas
    void setAlpha(uint8_t alpha);
    uint8_t getAlpha() const { return this->alpha; }
    bool isAnimating() const { return this->hasAlphaTarget || this->hasRotationTarget; }

    void setScale(float scale);
    void setRotation(float rotation);

    // Funciones para animar
    void fadeTo(uint8_t targetAlpha, float speed);
    void rotateTo(float targetRot, float speed);
    void zoomTo(float targetScale, float speed);

    int16_t getX() const { return this->x; }
    int16_t getY() const { return this->y; }

    float getScale() const { return this->scale; }
    float getRotation() const { return this->rotation; }

    
    virtual void update();
    virtual void draw();
};

#endif