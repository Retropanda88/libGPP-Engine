#ifndef INPUT_H
#define INPUT_H

#include <input/InputTypes.h>

// Fachada pública del sistema de Input
class Input
{
public:

    // Inicializa el sistema completo
    static void init();

    // Actualiza el sistema (llamar cada frame)
    static void update();

    // Estado por jugador
    static int isDown(int player, InputButton button);
    static int isPressed(int player, InputButton button);
    static int isReleased(int player, InputButton button);

    // Conectividad
    static int isConnected(int player);
};

#endif