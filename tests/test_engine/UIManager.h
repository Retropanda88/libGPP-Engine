#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "UIElement.h"

#define MAX_UI_ELEMENTS 50

class UIManager {
private:
    UIElement* elements[MAX_UI_ELEMENTS];
    int elementCount;

public:
    UIManager();
    ~UIManager();
    
    bool addElement(UIElement* element);
    void updateAll();
    void drawAll();
};

#endif