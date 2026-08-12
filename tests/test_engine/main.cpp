#include <engine/engine.h>
#include "UIElement.h"
#include "UIManager.h"

int main(int argc, char **argv) {

    if(Init_Sistem("Retro panda lab") < 0) return -1;
    if(Set_Video(640, 480) < 0) return -1;

    UIManager uiManager;

    UIElement panel(640/2, 480/2);

    panel.setImage("menu/textura3.png");
    panel.setRotation(0.0f);
    panel.setScale(0.01f);

    // 1. Iniciamos invisible y lanzamos el Fade-in hacia 255
    panel.setAlpha(0);
    panel.fadeTo(255, 2.0f);

    panel.setRotation(0.0f);
    panel.rotateTo(360.0f, 4.0f); // Gira suavemente hasta 90 grados a velocidad 2.0

    panel.zoomTo(1.0f, 0.02f);

    uiManager.addElement(&panel);

    // Variables de control de estados para la pausa de 3 segundos
    int state = 0;          // 0: Esperando fin de Fade-in
                            // 1: Esperando 3 segundos visible
                            // 2: Fade-out en curso
    int waitCounter = 0;
    int targetFrames = 180; // Asumiendo ~60 FPS (60 * 3 = 180 frames)

    bool running = true;
    while(running) {
        cls();
        //cls_rgb(255,255,255);
        uiManager.updateAll();
        uiManager.drawAll();
        Render();
    }

    off_video();
    shoutdown_sistem();

    return 0;
}