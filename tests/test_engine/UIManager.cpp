#include "UIManager.h"

UIManager::UIManager() {
    this->elementCount = 0;
    for (int i = 0; i < MAX_UI_ELEMENTS; i++) {
        this->elements[i] = NULL;
    }
}

UIManager::~UIManager() {}

bool UIManager::addElement(UIElement* element) {
    if (this->elementCount < MAX_UI_ELEMENTS && element != NULL) {
        this->elements[this->elementCount] = element;
        this->elementCount++;
        return true;
    }
    return false;
}

void UIManager::updateAll() {
    for (int i = 0; i < this->elementCount; i++) {
        if (this->elements[i] != NULL) {
            this->elements[i]->update();
        }
    }
}

void UIManager::drawAll() {
    for (int i = 0; i < this->elementCount; i++) {
        if (this->elements[i] != NULL) {
            this->elements[i]->draw();
        }
    }
}