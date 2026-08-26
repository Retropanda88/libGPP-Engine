#include <engine/engine.h>
#include <input/Input.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <font/gpp_font.h>

#define MAX_COMPONENTS 20
//#define TARGET_FPS 10
#define TARGET_FPS 60

#define DEG_TO_RAD (3.14159265358979323846f / 180.0f)

// --- Definición de Colores (ARGB de 32 bits) ---
#define COLOR_WHITE       0xFFFFFFFF
#define COLOR_CYAN        0xFF00FFFF
#define COLOR_LIGHT_GRAY  0xFFDCDCDC
#define COLOR_BLACK       0xFF000000

// Tablas de búsqueda para 360 grados
float sinTable[360];
float cosTable[360];

// Función auxiliar para convertir colores de 32 bits (ARGB) a 16 bits (RGB565) de la PS2
inline Uint16 RGB888_to_RGB565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

// Sobrecarga aceptando un Uint32 (ARGB)
inline Uint16 Color32To16(uint32_t color32) {
    uint8_t r = (color32 >> 16) & 0xFF;
    uint8_t g = (color32 >> 8) & 0xFF;
    uint8_t b = color32 & 0xFF;
    return RGB888_to_RGB565(r, g, b);
}

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

    union
    {
        SDL_Surface *surface;
        struct
        {
            char text[128];
            int fontSize;
            uint32_t color; 
            GPP_Font *customFont; // Puntero opcional para usar una fuente específica por etiqueta
        } label;
    } data;
} UIComponent;

UIComponent uiPool[MAX_COMPONENTS];
int componentCount = 0;

// Fuentes del sistema
GPP_Font font(FONT_62, 16, Color32To16(COLOR_WHITE));         // Fuente principal de la UI 57
GPP_Font fontTitle(FONT_45, 14, Color32To16(COLOR_WHITE));    // Fuente secundaria para el título de la PS2 45

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
int x_l = 0;

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
    
    // Si la etiqueta tiene asignada una fuente personalizada, la usamos; si no, usamos la general
    GPP_Font *activeFont = comp->data.label.customFont ? comp->data.label.customFont : &font;

    activeFont->setSize(comp->data.label.fontSize);
    activeFont->setColor(Color32To16(comp->data.label.color));
    activeFont->drawText(drawX, drawY, comp->data.label.text);
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

    int startX = 5;
    int startY = 90;
    int spacingY = 50;          

    for (int i = 0; i < 7; i++)
    {
        int size_icon = 30;
        draw_surface_region(iconSheet, i * size_icon, 0, size_icon, size_icon + 10, startX, startY + (i * spacingY));
        
        uint32_t col = (i == currentSelectedIndex) ? COLOR_CYAN : COLOR_WHITE;
        int fontSize = (i == currentSelectedIndex) ? 20 : 16;

        font.setSize(fontSize);
        font.setColor(Color32To16(col));
        font.drawText(startX + 30, startY + (i * spacingY) + 10, items[i]);
    }

    for (int i = 0; i < componentCount; i++)
    {
        if (uiPool[i].isFloating)
        {
            int textX = uiPool[i].x + 10;
            int textY = uiPool[i].y + 35; 
            
            font.setSize(12);
            font.setColor(Color32To16(COLOR_LIGHT_GRAY)); 
            font.drawText(textX, textY, itemDescriptions[currentSelectedIndex]);
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
    
    c->data.surface = load_img(file);
    if (c->data.surface)
        apply_transparency(c->data.surface, 255, 0, 255);
    apply_alpha(c->data.surface, alpha);
    c->drawFunc = drawPanel;
}

void addLabelCustom(UIComponent * parent, int relX, int relY, const char *text, int size, uint32_t color, GPP_Font *customFont)
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
    c->data.label.color = color;
    c->data.label.customFont = customFont;
    
    c->drawFunc = drawLabel;
}

void addLabel(UIComponent * parent, int relX, int relY, const char *text, int size, uint32_t color)
{
    addLabelCustom(parent, relX, relY, text, size, color, NULL);
}

bool initInterface()
{
    font.setSize(16);
    font.setColor(Color32To16(COLOR_WHITE)); 
    font.drawText(200, 200, "NOW LOADING...");
    Render();

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

    addLabel(p1, 15, 20, "Audio: Test-Tones (44.1khz, 16bits)", 16, COLOR_WHITE);
    addLabel(p1, 15, 50, "Input: [D-PAD: OK, X/O: OK]", 16, COLOR_WHITE);
    
    // Aquí usamos addLabelCustom para asignarle la fuente 'fontTitle' (FONT_62) manteniendo el tamaño 14
    addLabelCustom(p2, 1, 42, "SYSTEM:Playstation 2", 10, COLOR_WHITE, &fontTitle);

    if (!sndSelect.Load("menu/sfx/select.wav"))
    {
        printf("Aviso: No se pudo cargar sfx/select.wav\n");
    }

    if (!sndMove.Load("menu/sfx/move.wav"))
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

    mixer.init(44100, 2, 512);

    initInterface();
    mixer.setMusicVolume(60);
    mixer.playMusic("menu/music/background.wav", true);

    int down_l = 0;
    int up_l = 0;

    while (!isDone)
    {
        Input::update();

        int downPressed = Input::isDown(0, BUTTON_DOWN);
        int upPressed   = Input::isDown(0, BUTTON_UP);
        int aPressed    = Input::isDown(0, BUTTON_A);
        
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

        if (press_x)
        {
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
    }

    off_video();
    shoutdown_sistem();
    return 0;
}