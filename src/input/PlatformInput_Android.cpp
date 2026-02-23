#include <input/PlatformInput.h>
#include <input/InputSystem.h>

#include <SDL/SDL.h>

// --------------------------------------------------
// Inicialización
// --------------------------------------------------

void PlatformInput::initHardware()
{
    // SDL debe estar inicializado antes (video/input)
    SDL_EnableKeyRepeat(0, 0);
}


// --------------------------------------------------
// Detectar dispositivos
// --------------------------------------------------

void PlatformInput::detectDevices(InputSystem* system)
{
    // En Android con SDL 1.2:
    // Siempre asumimos al menos teclado virtual

    // Player 0 siempre conectado
    system->setConnected(0, INPUT_TRUE);

    // Player 1 opcional (si hay joystick)
    if (SDL_NumJoysticks() > 0)
        system->setConnected(1, INPUT_TRUE);
    else
        system->setConnected(1, INPUT_FALSE);
}


// --------------------------------------------------
// Poll hardware
// --------------------------------------------------

void PlatformInput::poll(InputSystem* system)
{
    SDL_Event event;

    // Necesario para actualizar estado interno de teclado
    while (SDL_PollEvent(&event))
    {
        // Aquí podrías manejar quit si quieres
    }

    // Obtener estado actual del teclado
    Uint8* keys = SDL_GetKeyState(NULL);

    u32 maskPlayer0 = 0;

    // Mapear teclado a botones abstractos

    if (keys[SDLK_z])      maskPlayer0 |= (1u << BUTTON_A);
    if (keys[SDLK_x])      maskPlayer0 |= (1u << BUTTON_B);
    if (keys[SDLK_a])      maskPlayer0 |= (1u << BUTTON_X);
    if (keys[SDLK_s])      maskPlayer0 |= (1u << BUTTON_Y);

    if (keys[SDLK_q])      maskPlayer0 |= (1u << BUTTON_L1);
    if (keys[SDLK_w])      maskPlayer0 |= (1u << BUTTON_R1);

    if (keys[SDLK_RETURN]) maskPlayer0 |= (1u << BUTTON_START);
    if (keys[SDLK_RSHIFT]) maskPlayer0 |= (1u << BUTTON_SELECT);

    if (keys[SDLK_UP])     maskPlayer0 |= (1u << BUTTON_UP);
    if (keys[SDLK_DOWN])   maskPlayer0 |= (1u << BUTTON_DOWN);
    if (keys[SDLK_LEFT])   maskPlayer0 |= (1u << BUTTON_LEFT);
    if (keys[SDLK_RIGHT])  maskPlayer0 |= (1u << BUTTON_RIGHT);

    system->setPlayerState(0, maskPlayer0);

    // --------------------------------------------------
    // Player 1 (joystick si existe)
    // --------------------------------------------------

    if (SDL_NumJoysticks() > 0)
    {
        static SDL_Joystick* joystick = NULL;

        if (!joystick)
            joystick = SDL_JoystickOpen(0);

        if (joystick)
        {
            SDL_JoystickUpdate();

            u32 maskPlayer1 = 0;

            if (SDL_JoystickGetButton(joystick, 0))
                maskPlayer1 |= (1u << BUTTON_A);

            if (SDL_JoystickGetButton(joystick, 1))
                maskPlayer1 |= (1u << BUTTON_B);

            if (SDL_JoystickGetButton(joystick, 2))
                maskPlayer1 |= (1u << BUTTON_X);

            if (SDL_JoystickGetButton(joystick, 3))
                maskPlayer1 |= (1u << BUTTON_Y);

            // Ejes para direcciones
            Sint16 axisX = SDL_JoystickGetAxis(joystick, 0);
            Sint16 axisY = SDL_JoystickGetAxis(joystick, 1);

            if (axisX < -8000) maskPlayer1 |= (1u << BUTTON_LEFT);
            if (axisX >  8000) maskPlayer1 |= (1u << BUTTON_RIGHT);
            if (axisY < -8000) maskPlayer1 |= (1u << BUTTON_UP);
            if (axisY >  8000) maskPlayer1 |= (1u << BUTTON_DOWN);

            system->setPlayerState(1, maskPlayer1);
        }
    }
}