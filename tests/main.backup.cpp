#include <engine/engine.h>
#include <input/Input.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_COMPONENTS 20
#define TARGET_FPS 10

#define DEG_TO_RAD (3.14159265358979323846f / 180.0f)

// Tablas de búsqueda para 360 grados
float sinTable[360];
float cosTable[360];

typedef enum
{ TYPE_PANEL, TYPE_LABEL } ComponentType;

typedef struct UIComponent
{
    int16_t x, y;
    int16_t base_x, base_y;
    int angle; 
    bool isFloating;
    struct UIComponent *parent;
    ComponentType type;
    void (*drawFunc) (struct UIComponent *);

    GPPCachedText cache; 

    union
    {
        SDL_Surface *surface;
        struct
        {
            char text[128];
            int fontSize;
            uint8_t r, g, b;
        } label;
    } data;
} UIComponent;

UIComponent uiPool[MAX_COMPONENTS];
int componentCount = 0;
GPPFont font;
static int isDone = 0;
SDL_Surface *iconSheet = NULL;  

// Variables de estado para el control de navegación del menú
int currentSelectedIndex = 0;

// Variables de control para el flanco del botón A
int a_l = 0;

// ⚡ Variables de auto-repetición ajustadas para mayor velocidad y respuesta inmediata
int navRepeatTimer = 0;
#define NAV_INITIAL_DELAY 10  // Reducido a 15 frames (~0.16s) para que el salto empiece casi de inmediato
#define NAV_REPEAT_INTERVAL 3 // Velocidad de repetición más rápida entre cada movimiento

void initMathLUT()
{
    for (int i = 0; i < 360; i++)
    {
        sinTable[i] = sinf((float)i * DEG_TO_RAD);
        cosTable[i] = cosf((float)i * DEG_TO_RAD);
    }
}

// --- Funciones de Dibujo ---
void drawPanel(UIComponent * comp)
{
    if (comp->data.surface)
        draw_surface(comp->data.surface, comp->x, comp->y);
}

void drawLabel(UIComponent * comp)
{
    int drawX = comp->x;
    int drawY = comp->y;
    if (comp->parent)
    {
        drawX += comp->parent->x;
        drawY += comp->parent->y;
    }
    
    font.drawCached(&comp->cache, comp->data.label.text, drawX, drawY,
                    comp->data.label.fontSize, comp->data.label.r, comp->data.label.g,
                    comp->data.label.b);
}

// --- Animación ---
void updateAnimations()
{
    for (int i = 0; i < componentCount; i++)
    {
        if (uiPool[i].isFloating)
        {
            uiPool[i].angle = (uiPool[i].angle + 12) % 360;

            float sinVal = sinTable[uiPool[i].angle];
            float cosVal = cosTable[uiPool[i].angle];

            uiPool[i].y = uiPool[i].base_y + (int)(4.0f * sinVal);
            uiPool[i].x = uiPool[i].base_x + (int)(2.0f * cosVal);
        }
    }
}

// --- Gestión de Menú (Sprite Sheet con Navegación) ---
void drawMenu()
{
    if (!iconSheet)
        return;

    const char *items[] =
        { "[HOME]", "[TESTS]", "[EMUS]", "[MUSIC]", "[IMAGES]", "[FILES]", "[INFO]" };

    static GPPCachedText menuCache[7];

    int startX = 5;
    int startY = 90;
    int spacingY = 50;          

    for (int i = 0; i < 7; i++)
    {
        int size_icon = 30;
        draw_surface_region(iconSheet, i * size_icon, 0, size_icon, size_icon + 10, startX, startY + (i * spacingY));
        
        uint8_t r = 255;
        uint8_t g = 255;
        uint8_t b = (i == currentSelectedIndex) ? 0 : 255;

        int fontSize = (i == currentSelectedIndex) ? 22 : 20;

        font.drawCached(&menuCache[i], items[i], startX + 30, startY + (i * spacingY) + 10, fontSize, r, g, b);
    }
}

// --- Gestión de Componentes ---
void addPanel(int x, int y, const char *file, u8 alpha, bool floating)
{
    if (componentCount >= MAX_COMPONENTS)
        return;
    UIComponent *c = &uiPool[componentCount++];
    c->type = TYPE_PANEL;
    c->base_x = x;
    c->base_y = y;
    c->x = x;
    c->y = y;
    c->isFloating = floating;
    c->angle = rand() % 360; 
    c->parent = NULL;
    
    c->cache.surface = NULL;
    c->cache.content[0] = '\0';
    
    c->data.surface = load_img(file);
    if (c->data.surface)
        apply_transparency(c->data.surface, 255, 0, 255);
    apply_alpha(c->data.surface, alpha);
    c->drawFunc = drawPanel;
}

void addLabel(UIComponent * parent, int relX, int relY, const char *text, int size, uint8_t r,
              uint8_t g, uint8_t b)
{
    if (componentCount >= MAX_COMPONENTS)
        return;
    UIComponent *c = &uiPool[componentCount++];
    c->type = TYPE_LABEL;
    c->x = relX;
    c->y = relY;
    c->parent = parent;
    c->isFloating = false;
    c->angle = 0; 
    
    strncpy(c->data.label.text, text, 127);
    c->data.label.fontSize = size;
    c->data.label.r = r;
    c->data.label.g = g;
    c->data.label.b = b;
    
    c->cache.surface = NULL;
    c->cache.content[0] = '\0';
    c->cache.size = 0;
    c->cache.r = 0;
    c->cache.g = 0;
    c->cache.b = 0;
    
    c->drawFunc = drawLabel;
}

bool initInterface()
{
    addPanel(0, 0, "menu/background.png", 255, false);
    addPanel(2, -2, "menu/pandlaLab.png", 255, false);

    iconSheet = load_img("menu/icons.png");
    if (iconSheet)
        apply_transparency(iconSheet, 255, 0, 255);

    addPanel(180, 80, "menu/textura1.png", 150, false);
    UIComponent *p1 = &uiPool[componentCount - 1];
    addPanel(0, 80, "menu/textura2.png", 255, false);
    addPanel(185, 320, "menu/textura3.png", 255, true);
    UIComponent *p3 = &uiPool[componentCount - 1];
    addPanel(470, 2, "menu/textura4.png", 105, false);
    UIComponent *p2 = &uiPool[componentCount - 1];

    addLabel(p1, 15, 20, "Audio: Test-Tones (44.1khz, 16bits)", 20, 255, 255, 255);
    addLabel(p1, 15, 50, "Input: [D-PAD: OK, X/O: OK]", 20, 255, 255, 255);
    addLabel(p2, 3, 30, "SYSTEM: Playstation 2", 16, 255, 255, 255);
    addLabel(p3, 10, 10, "Status: System Initialized", 20, 255, 255, 255);

    return true;
}

int main(int argc, char **argv)
{
    memset(uiPool, 0, sizeof(uiPool));
    
    if (Init_Sistem("panda lab") < 0)
        return -1;
    if (Set_Video(640,480) < 0)
        return -1;
    
    Input::init();

    printf("\n\n set video 640 * 480\n");
        
    initMathLUT();

    if (!font.init("menu/font.ttf"))
        printf("Error: Fuente\n");

    initInterface();

    while (!isDone)
    {
        Input::update();

        int downPressed = Input::isDown(0, BUTTON_DOWN);
        int upPressed   = Input::isDown(0, BUTTON_UP);
        int aPressed    = Input::isDown(0, BUTTON_A);

        int press_a = (aPressed && !a_l);
        a_l = aPressed;

        bool moveDown = false;
        bool moveUp = false;

        // ⚡ Lógica mejorada: Respuesta inmediata al primer frame y repetición fluida
        if (downPressed && !upPressed)
        {
            if (navRepeatTimer == 0)
            {
                moveDown = true; // Respuesta instantánea al presionar
                navRepeatTimer = 1;
            }
            else
            {
                navRepeatTimer++;
                if (navRepeatTimer > NAV_INITIAL_DELAY)
                {
                    if ((navRepeatTimer - NAV_INITIAL_DELAY) % NAV_REPEAT_INTERVAL == 0)
                    {
                        moveDown = true;
                    }
                }
            }
        }
        else if (upPressed && !downPressed)
        {
            if (navRepeatTimer == 0)
            {
                moveUp = true; // Respuesta instantánea al presionar
                navRepeatTimer = 1;
            }
            else
            {
                navRepeatTimer++;
                if (navRepeatTimer > NAV_INITIAL_DELAY)
                {
                    if ((navRepeatTimer - NAV_INITIAL_DELAY) % NAV_REPEAT_INTERVAL == 0)
                    {
                        moveUp = true;
                    }
                }
            }
        }
        else
        {
            navRepeatTimer = 0; // Reinicia cuando se suelta el botón
        }

        // Aplicar movimiento en el menú
        if (moveDown)
        {
            currentSelectedIndex++;
            if (currentSelectedIndex > 6)
                currentSelectedIndex = 0; 
        }
        if (moveUp)
        {
            currentSelectedIndex--;
            if (currentSelectedIndex < 0)
                currentSelectedIndex = 6; 
        }

        // Acción al presionar el botón A
        if (press_a)
        {
            printf("Item seleccionado: %d\n", currentSelectedIndex);
            if (currentSelectedIndex == 6) {
                isDone = 1; 
            }
        }

        cls_rgb(0, 0, 0); 
        
        updateAnimations();
        
        for (int i = 0; i < componentCount; i++)
        {
            if (uiPool[i].drawFunc)
                uiPool[i].drawFunc(&uiPool[i]);
        }
        
        drawMenu(); 
        
        Render();
        Fps_sincronizar(TARGET_FPS);
    }

    if (iconSheet)
        SDL_FreeSurface(iconSheet);
        
    for (int i = 0; i < componentCount; i++)
    {
        if (uiPool[i].type == TYPE_PANEL && uiPool[i].data.surface)
        {
            SDL_FreeSurface(uiPool[i].data.surface);
        }
        
        if (uiPool[i].cache.surface)
        {
            font.freeCachedText(&uiPool[i].cache);
        }
    }

    off_video();
    shoutdown_sistem();
    return 0;
}