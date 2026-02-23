#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

#include <input/InputTypes.h>
#include <input/PlayerInput.h>
#include <input/PlatformInput.h>

class InputSystem
{
public:

    // Inicialización general
    void init();

    // Update por frame
    void update();

    // Consultas de estado
    int isDown(int player, InputButton button) const;
    int isPressed(int player, InputButton button) const;
    int isReleased(int player, InputButton button) const;

    int isConnected(int player) const;

    // Métodos usados por PlatformInput
    void setPlayerState(int player, u32 buttonMask);
    void setConnected(int player, int state);

private:

    // Slots lógicos fijos
    PlayerInput m_players[INPUT_MAX_PLAYERS];

    // Estado de conexión por jugador
    int m_connected[INPUT_MAX_PLAYERS];

    // Capa de hardware
    PlatformInput m_platform;
};

#endif