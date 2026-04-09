#ifndef PLAYER_INPUT_H
#define PLAYER_INPUT_H

#include <input/InputTypes.h>

class PlayerInput
{
public:

    PlayerInput()
        : m_currentButtons(0),
          m_previousButtons(0)
    {
    }

    // Actualiza el estado del jugador
    void update(u32 newMask)
    {
        m_previousButtons = m_currentButtons;
        m_currentButtons = newMask;
    }

    // Botón mantenido
    int isDown(u32 mask) const
    {
        return (m_currentButtons & mask) ? INPUT_TRUE : INPUT_FALSE;
    }

    // Botón presionado este frame
    int isPressed(u32 mask) const
    {
        return ((m_currentButtons & mask) &&
               !(m_previousButtons & mask))
               ? INPUT_TRUE
               : INPUT_FALSE;
    }

    // Botón soltado este frame
    int isReleased(u32 mask) const
    {
        return (!(m_currentButtons & mask) &&
                (m_previousButtons & mask))
                ? INPUT_TRUE
                : INPUT_FALSE;
    }

private:

    u32 m_currentButtons;
    u32 m_previousButtons;
};

#endif