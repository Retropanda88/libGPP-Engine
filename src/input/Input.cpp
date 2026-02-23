#include <input/Input.h>
#include <input/InputSystem.h>

// --------------------------------------------------
// Instancia interna única del sistema real
// --------------------------------------------------

static InputSystem g_inputSystem;


// --------------------------------------------------
// Inicialización
// --------------------------------------------------

void Input::init()
{
    g_inputSystem.init();
}


// --------------------------------------------------
// Update por frame
// --------------------------------------------------

void Input::update()
{
    g_inputSystem.update();
}


// --------------------------------------------------
// Consultas de estado
// --------------------------------------------------

int Input::isDown(int player, InputButton button)
{
    return g_inputSystem.isDown(player, button);
}


int Input::isPressed(int player, InputButton button)
{
    return g_inputSystem.isPressed(player, button);
}


int Input::isReleased(int player, InputButton button)
{
    return g_inputSystem.isReleased(player, button);
}


int Input::isConnected(int player)
{
    return g_inputSystem.isConnected(player);
}