#include "UIElement.h"

UIElement::UIElement(int16_t x, int16_t y, UIElement* parent) {
    this->x = x;
    this->y = y;
    this->scale = 1.0f;
    this->rotation = 0.0f;
    this->alpha = 255;
    
    // Inicializar fundido
    this->targetAlpha = 255;
    this->hasAlphaTarget = false;
    this->alphaSpeed = 3.0f;

    // Inicializar rotación
    this->targetRotation = 0.0f;
    this->hasRotationTarget = false;
    this->rotationSpeed = 1.0f;

    this->surface = NULL;
    this->parent = parent;
    this->childCount = 0;
    
    for (int i = 0; i < MAX_CHILDREN; i++) {
        this->children[i] = NULL;
    }

    if (this->parent != NULL) {
        this->parent->addChild(this);
    }
}

UIElement::~UIElement() {
    if (this->surface != NULL) {
        SDL_FreeSurface(this->surface);
        this->surface = NULL;
    }
}

bool UIElement::addChild(UIElement* child) {
    if (this->childCount < MAX_CHILDREN && child != NULL) {
        this->children[this->childCount] = child;
        child->parent = this;
        this->childCount++;
        return true;
    }
    return false;
}

void UIElement::setAlpha(uint8_t alpha) { 
    this->alpha = alpha; 
}

void UIElement::setScale(float scale) { 
    this->scale = scale; 
}

void UIElement::setRotation(float rotation) { 
    this->rotation = rotation; 
}

void UIElement::fadeTo(uint8_t targetAlpha, float speed) {
    this->targetAlpha = targetAlpha;
    this->hasAlphaTarget = true;
    this->alphaSpeed = speed;
}

void UIElement::rotateTo(float targetRot, float speed) {
    this->targetRotation = targetRot;
    this->hasRotationTarget = true;
    this->rotationSpeed = speed;
}

bool UIElement::setImage(const char* filepath) {
    if (this->surface != NULL) {
        SDL_FreeSurface(this->surface);
        this->surface = NULL;
    }

    this->surface = load_img(filepath);
    if (this->surface == NULL) return false;

    apply_transparency(this->surface, 255, 0, 255);
    return true;
}

// ⚠️ Tu update con el alpha intacto + la nueva lógica de rotación agregada abajo
void UIElement::update() {
    if (this->hasAlphaTarget) {
        if (this->alpha < this->targetAlpha) {
            float nextAlpha = (float)this->alpha + this->alphaSpeed;
            if (nextAlpha >= (float)this->targetAlpha) {
                this->alpha = this->targetAlpha;
                this->hasAlphaTarget = false;
            } else {
                this->alpha = (uint8_t)nextAlpha;
            }
        } else if (this->alpha > this->targetAlpha) {
            float nextAlpha = (float)this->alpha - this->alphaSpeed;
            if (nextAlpha <= (float)this->targetAlpha) {
                this->alpha = this->targetAlpha;
                this->hasAlphaTarget = false;
            } else {
                this->alpha = (uint8_t)nextAlpha;
            }
        }
    }

    // --- NUEVA LÓGICA DE ROTACIÓN ---
    if (this->hasRotationTarget) {
        if (this->rotation < this->targetRotation) {
            float nextRot = this->rotation + this->rotationSpeed;
            if (nextRot >= this->targetRotation) {
                this->rotation = this->targetRotation;
                this->hasRotationTarget = false;
            } else {
                this->rotation = nextRot;
            }
        } else if (this->rotation > this->targetRotation) {
            float nextRot = this->rotation - this->rotationSpeed;
            if (nextRot <= this->targetRotation) {
                this->rotation = this->targetRotation;
                this->hasRotationTarget = false;
            } else {
                this->rotation = nextRot;
            }
        }
    }
    // ---------------------------------

    for (int i = 0; i < this->childCount; i++) {
        if (this->children[i] != NULL) {
            this->children[i]->update();
        }
    }
}

void UIElement::draw() {
    if (this->surface == NULL || this->alpha == 0) return;

    SDL_Surface* surfaceToDraw = this->surface;
    bool isRotozoomed = false;

    if (this->scale != 1.0f || this->rotation != 0.0f) {
        surfaceToDraw = rotozoom_create(this->surface, this->rotation, this->scale);
        if (surfaceToDraw != NULL) {
            isRotozoomed = true;
            rotozoom_set_position(surfaceToDraw, this->x, this->y);
        } else {
            surfaceToDraw = this->surface;
        }
    }
    
    apply_alpha(surfaceToDraw, this->alpha);
    draw_surface(surfaceToDraw, this->x, this->y);

    if (isRotozoomed && surfaceToDraw != this->surface) {
        rotozoom_destroy(surfaceToDraw);
    }

    for (int i = 0; i < this->childCount; i++) {
        if (this->children[i] != NULL) {
            this->children[i]->draw();
        }
    }
}