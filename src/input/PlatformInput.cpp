/*
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
}*/

#include <input/PlatformInput.h>
#include <input/InputSystem.h>

// ======================================================
// Detección automática de plataforma (SIN Makefile)
// ======================================================

#if defined(__ANDROID__)
    #define PLATFORM_ANDROID
#elif defined(__psp__)
    #define PLATFORM_PSP
#else
    #define PLATFORM_PC
#endif


// ======================================================
// Includes por plataforma
// ======================================================

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_PC)
    #include <SDL/SDL.h>
#endif

#if defined(PLATFORM_PSP)
    #include <pspctrl.h>
#endif


// ======================================================
// Inicialización
// ======================================================

void PlatformInput::initHardware()
{
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_PC)

    SDL_EnableKeyRepeat(0, 0);

#elif defined(PLATFORM_PSP)

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

#endif
}


// ======================================================
// Detectar dispositivos
// ======================================================

void PlatformInput::detectDevices(InputSystem* system)
{
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_PC)

    system->setConnected(0, INPUT_TRUE);
    system->setConnected(1, SDL_NumJoysticks() > 0 ? INPUT_TRUE : INPUT_FALSE);

#elif defined(PLATFORM_PSP)

    system->setConnected(0, INPUT_TRUE);
    system->setConnected(1, INPUT_FALSE);

#endif
}


// ======================================================
// Poll hardware
// ======================================================

void PlatformInput::poll(InputSystem* system)
{
#if defined(PLATFORM_ANDROID) || defined(PLATFORM_PC)

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // Mantener estado interno SDL actualizado
    }

    Uint8* keys = SDL_GetKeyState(NULL);
    u32 maskPlayer0 = 0;

    // -------------------------
    // Teclado → Player 0
    // -------------------------

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

    // -------------------------
    // Joystick → Player 1
    // -------------------------

    static SDL_Joystick* joystick = NULL;

    if (SDL_NumJoysticks() > 0)
    {
        if (!joystick)
            joystick = SDL_JoystickOpen(0);

        if (joystick)
        {
            SDL_JoystickUpdate();

            u32 maskPlayer1 = 0;
            const int DEADZONE = 8000;

            if (SDL_JoystickGetButton(joystick, 0))
                maskPlayer1 |= (1u << BUTTON_A);

            if (SDL_JoystickGetButton(joystick, 1))
                maskPlayer1 |= (1u << BUTTON_B);

            if (SDL_JoystickGetButton(joystick, 2))
                maskPlayer1 |= (1u << BUTTON_X);

            if (SDL_JoystickGetButton(joystick, 3))
                maskPlayer1 |= (1u << BUTTON_Y);

            Sint16 axisX = SDL_JoystickGetAxis(joystick, 0);
            Sint16 axisY = SDL_JoystickGetAxis(joystick, 1);

            if (axisX < -DEADZONE) maskPlayer1 |= (1u << BUTTON_LEFT);
            if (axisX >  DEADZONE) maskPlayer1 |= (1u << BUTTON_RIGHT);
            if (axisY < -DEADZONE) maskPlayer1 |= (1u << BUTTON_UP);
            if (axisY >  DEADZONE) maskPlayer1 |= (1u << BUTTON_DOWN);

            system->setPlayerState(1, maskPlayer1);
        }
    }
    else
    {
        system->setPlayerState(1, 0);
    }

#elif defined(PLATFORM_PSP)

    SceCtrlData pad;
    sceCtrlReadBufferPositive(&pad, 1);

    u32 maskPlayer0 = 0;

    if (pad.Buttons & PSP_CTRL_CROSS)     maskPlayer0 |= (1u << BUTTON_A);
    if (pad.Buttons & PSP_CTRL_CIRCLE)    maskPlayer0 |= (1u << BUTTON_B);
    if (pad.Buttons & PSP_CTRL_SQUARE)    maskPlayer0 |= (1u << BUTTON_X);
    if (pad.Buttons & PSP_CTRL_TRIANGLE)  maskPlayer0 |= (1u << BUTTON_Y);

    if (pad.Buttons & PSP_CTRL_LTRIGGER)  maskPlayer0 |= (1u << BUTTON_L1);
    if (pad.Buttons & PSP_CTRL_RTRIGGER)  maskPlayer0 |= (1u << BUTTON_R1);

    if (pad.Buttons & PSP_CTRL_START)     maskPlayer0 |= (1u << BUTTON_START);
    if (pad.Buttons & PSP_CTRL_SELECT)    maskPlayer0 |= (1u << BUTTON_SELECT);

    if (pad.Buttons & PSP_CTRL_UP)        maskPlayer0 |= (1u << BUTTON_UP);
    if (pad.Buttons & PSP_CTRL_DOWN)      maskPlayer0 |= (1u << BUTTON_DOWN);
    if (pad.Buttons & PSP_CTRL_LEFT)      maskPlayer0 |= (1u << BUTTON_LEFT);
    if (pad.Buttons & PSP_CTRL_RIGHT)     maskPlayer0 |= (1u << BUTTON_RIGHT);

    const int DEADZONE = 30;

    if (pad.Lx < (128 - DEADZONE)) maskPlayer0 |= (1u << BUTTON_LEFT);
    if (pad.Lx > (128 + DEADZONE)) maskPlayer0 |= (1u << BUTTON_RIGHT);
    if (pad.Ly < (128 - DEADZONE)) maskPlayer0 |= (1u << BUTTON_UP);
    if (pad.Ly > (128 + DEADZONE)) maskPlayer0 |= (1u << BUTTON_DOWN);

    system->setPlayerState(0, maskPlayer0);

#endif
}
