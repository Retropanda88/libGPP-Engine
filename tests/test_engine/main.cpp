#include <engine/engine.h>
#include "UIElement.h"
#include "UIManager.h"
#include "UIText.h" // ¡Incluimos nuestro nuevo sistema de textos!

int main(int argc, char **argv) {

    if(Init_Sistem("Retro panda lab") < 0) return -1;
    if(Set_Video(640, 480) < 0) return -1;

    UIManager uiManager;

    // 1. Creamos el panel principal
    UIElement panel(640/2, 480/2);
    panel.setImage("menu/textura3.png");
    panel.setRotation(0.0f);
    panel.setScale(0.01f);

    // Animaciones del panel
    panel.setAlpha(0);
    panel.fadeTo(255, 2.0f);
    panel.rotateTo(360.0f, 4.0f);
    panel.zoomTo(1.0f, 0.02f);

    // 2. Creamos nuestro texto como hijo del panel
    // (Posición relativa x: -80, y: -20 respecto al centro del panel)
    UIText tituloText(0, 0, "RETRO PANDA", FONT_32, &panel);
    tituloText.setFontColor(0xFFFFFFFF); // Blanco brillante

    // 3. Añadimos el panel al manager (los hijos se actualizan y dibujan solos)
    uiManager.addElement(&panel);

    // Variables de control de estados
    int state = 0;            // 0: Animando entrada, 1: Visible, 2: Saliendo
    int waitCounter = 0;
    int targetFrames = 180;   // ~3 segundos a 60 FPS

    bool running = true;
    while(running) {
        cls();

        // Actualizamos lógica y dibujamos
        uiManager.updateAll();
        uiManager.drawAll();

        // Opcional: Lógica básica de salida con ESC o start si tu engine lo soporta
        // (Por ahora corre de forma fluida)

        Render();
    }

    off_video();
    shoutdown_sistem();

    return 0;
}