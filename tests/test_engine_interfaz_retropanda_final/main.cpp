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

// --- Sistema de Audio ---
Cmixer mixer;
CSample sndMove;
CSample sndSelect;

// Variables de estado para el control de navegación del menú
int currentSelectedIndex = 0;

// Variables de control para los flancos de botones
int a_l = 0;
int x_l = 0; // Flanco para probar el control manual de música desde el input

// Variables de auto-repetición
int navRepeatTimer = 0;
#define NAV_INITIAL_DELAY 10  
#define NAV_REPEAT_INTERVAL 3 

const char *itemDescriptions[] = {
    "System main dashboard and overview.",
    "Run hardware, video and peripheral diagnostics.",
    "Launch compatible emulators and ROM files.",
    "Audio player and hardware test tones (44.1kHz).",
    "Manager and viewer for stored image assets.",
    "System file explorer and storage manager.",
    "Detailed system information, specs, and credits."
};

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

void drawMenu()
{
    if (!iconSheet)
        return;

    const char *items[] =
        { "[HOME]", "[TESTS]", "[EMUS]", "[MUSIC]", "[IMAGES]", "[FILES]", "[INFO]" };

    static GPPCachedText menuCache[7];
    static GPPCachedText descCache;

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

    for (int i = 0; i < componentCount; i++)
    {
        if (uiPool[i].isFloating)
        {
            int textX = uiPool[i].x + 10;
            int textY = uiPool[i].y + 35; 
            
            font.drawCached(&descCache, itemDescriptions[currentSelectedIndex], textX, textY, 20, 220, 220, 220);
            break;
        }
    }
}

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
    sysfontDrawString(200, 200, "NOW LOADING...", 0Xffffffff);
    Render();

    addPanel(0, 0, "mass:/menu/background.png", 255, false);
    addPanel(2, -2, "mass:/menu/pandlaLab.png", 255, false);

    iconSheet = load_img("mass:/menu/icons.png");
    if (iconSheet)
        apply_transparency(iconSheet, 255, 0, 255);

    addPanel(180, 80, "mass:/menu/textura1.png", 150, false);
    UIComponent *p1 = &uiPool[componentCount - 1];
    addPanel(0, 80, "mass:/menu/textura2.png", 255, false);
    addPanel(185, 320, "mass:/menu/textura3.png", 255, true);
    UIComponent *p3 = &uiPool[componentCount - 1];
    addPanel(470, 2, "mass:/menu/textura4.png", 105, false);
    UIComponent *p2 = &uiPool[componentCount - 1];

    addLabel(p1, 15, 20, "Audio: Test-Tones (44.1khz, 16bits)", 20, 255, 255, 255);
    addLabel(p1, 15, 50, "Input: [D-PAD: OK, X/O: OK]", 20, 255, 255, 255);
    addLabel(p2, 3, 30, "SYSTEM: Playstation 2", 16, 255, 255, 255);

    if (!sndSelect.Load("mass:/menu/sfx/select.wav"))
    {
        printf("Aviso: No se pudo cargar sfx/select.wav\n");
    }

    if (!sndMove.Load("mass:/menu/sfx/move.wav"))
    {
        printf("Aviso: No se pudo cargar sfx/move.wav\n");
    }

    return true;
}

int main(int argc, char **argv)
{
    memset(uiPool, 0, sizeof(uiPool));

    if (Init_Sistem("panda lab") < 0)
        return -1;

    if (Set_Video(320,240) < 0)
        return -1;
    
    startup();

    if (Set_Video(640,480) < 0)
        return -1;

    Input::init();
    initMathLUT();

    mixer.init(44100, 2, 2048);

    if (!font.init("mass:/menu/font.ttf"))
        printf("Error: Fuente\n");

    initInterface();

    mixer.playMusic("menu/music/background.wav", true);

    int down_l = 0;
    int up_l = 0;

    while (!isDone)
    {
        Input::update();

        int downPressed = Input::isDown(0, BUTTON_DOWN);
        int upPressed   = Input::isDown(0, BUTTON_UP);
        int aPressed    = Input::isDown(0, BUTTON_A);
        
        // PRUEBA DE INPUT MANUAL: Usamos el botón X (BUTTON_X) para controlar/reiniciar la música a voluntad
        int xPressed    = Input::isDown(0, BUTTON_X);
        int press_x     = (xPressed && !x_l);
        x_l = xPressed;

        int press_a  = (aPressed && !a_l);
        a_l = aPressed;

        int press_down = (downPressed && !down_l);
        int press_up   = (upPressed && !up_l);
        down_l = downPressed;
        up_l = upPressed;

        bool moveDown = false;
        bool moveUp = false;

        if (press_down)
        {
            moveDown = true;
            navRepeatTimer = 1;
        }
        else if (press_up)
        {
            moveUp = true;
            navRepeatTimer = 1;
        }
        else if (downPressed)
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
        else if (upPressed)
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
        else
        {
            navRepeatTimer = 0;
        }

        // Si presionas el botón X, disparas o reinicias la música manualmente desde aquí
        if (press_x)
        {
            //printf("Control manual: Reiniciando música de fondo por input...\n");
            mixer.playMusic("menu/music/background.wav", true);
        }

        if (moveDown)
        {
            currentSelectedIndex++;
            if (currentSelectedIndex > 6)
                currentSelectedIndex = 0; 
            
            if (sndMove.getData() != NULL) {
                mixer.playChannel(&sndMove, false, 100, -1);
            }
        }
        if (moveUp)
        {
            currentSelectedIndex--;
            if (currentSelectedIndex < 0)
                currentSelectedIndex = 6; 
            
            if (sndMove.getData() != NULL) {
                mixer.playChannel(&sndMove, false, 100, -1);
            }
        }

        if (press_a)
        {
            if (sndSelect.getData() != NULL) {
                mixer.playChannel(&sndSelect, false, 120, -1);
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

    sndMove.close();
    sndSelect.close();

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