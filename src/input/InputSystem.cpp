#include "input/InputSystem.h"

// --------------------------------------------------
// Inicialización
// --------------------------------------------------

void InputSystem::init()
{
    // Inicializar estados de conexión
    for (int i = 0; i < INPUT_MAX_PLAYERS; ++i)
    {
        m_connected[i] = INPUT_FALSE;
        m_players[i].update(0); // Sin botones presionados
    }

    // Inicializar hardware
    m_platform.initHardware();
}


// --------------------------------------------------
// Update por frame
// --------------------------------------------------

void InputSystem::update()
{
    // Primero detectar dispositivos conectados
    m_platform.detectDevices(this);

    // Luego leer estado del hardware
    m_platform.poll(this);
}


// --------------------------------------------------
// Métodos usados por PlatformInput
// --------------------------------------------------

void InputSystem::setPlayerState(int player, u32 buttonMask)
{
    if (player < 0 || player >= INPUT_MAX_PLAYERS)
        return;

    m_players[player].update(buttonMask);
}


void InputSystem::setConnected(int player, int state)
{
    if (player < 0 || player >= INPUT_MAX_PLAYERS)
        return;

    m_connected[player] = state;
}


// --------------------------------------------------
// Consultas públicas
// --------------------------------------------------

int InputSystem::isDown(int player, InputButton button) const
{
    if (player < 0 || player >= INPUT_MAX_PLAYERS)
        return INPUT_FALSE;

    if (!m_connected[player])
        return INPUT_FALSE;

    return m_players[player].isDown(1u << button);
}


int InputSystem::isPressed(int player, InputButton button) const
{
    if (player < 0 || player >= INPUT_MAX_PLAYERS)
        return INPUT_FALSE;

    if (!m_connected[player])
        return INPUT_FALSE;

    return m_players[player].isPressed(1u << button);
}


int InputSystem::isReleased(int player, InputButton button) const
{
    if (player < 0 || player >= INPUT_MAX_PLAYERS)
        return INPUT_FALSE;

    if (!m_connected[player])
        return INPUT_FALSE;

    return m_players[player].isReleased(1u << button);
}


int InputSystem::isConnected(int player) const
{
    if (player < 0 || player >= INPUT_MAX_PLAYERS)
        return INPUT_FALSE;

    return m_connected[player];
}