
/*
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
*/

#include <input/PlatformInput.h>
#include <input/InputSystem.h>

// ======================================================
// Detección automática de plataforma
// ======================================================

#if defined(__ANDROID__)
    #define PLATFORM_ANDROID
#elif defined(__psp__)
    #define PLATFORM_PSP
#elif defined(__EE__)
    #define PLATFORM_PS2
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

#if defined(PLATFORM_PS2)
    #include <tamtypes.h>
    #include <kernel.h>
    #include <sifrpc.h>
    #include <loadfile.h>
    #include <iopcontrol.h>
    #include <libpad.h>
    #include <input/sio2man.h>
    #include <input/padman.h>

    extern unsigned char sio2man_irx[];
    extern unsigned int sio2man_irx_size;

    extern unsigned char padman_irx[];
    extern unsigned int padman_irx_size;

    static unsigned char padBuf[256] __attribute__((aligned(64)));
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


#elif defined(PLATFORM_PS2)

    SifInitRpc(0);

    // Reset IOP (IMPORTANTE en hardware real)
    //SifIopReset("", 0);
    //while (!SifIopSync()) {}

    SifInitRpc(0);

    // Cargar módulos IRX embebidos
    int ret;

    ret = SifExecModuleBuffer(sio2man_irx, sio2man_irx_size, 0, NULL, NULL);
    if (ret < 0) {
        printf("Error loading SIO2MAN\n");
    }

    ret = SifExecModuleBuffer(padman_irx, padman_irx_size, 0, NULL, NULL);
    if (ret < 0) {
        printf("Error loading PADMAN\n");
    }

    padInit(0);

    if (!padPortOpen(0, 0, padBuf)) {
        printf("padPortOpen failed\n");
    }

    // Esperar a que el mando esté estable
    int state;
    do {
        state = padGetState(0, 0);
    } while (state != PAD_STATE_STABLE &&
             state != PAD_STATE_FINDCTP1);

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

#elif defined(PLATFORM_PS2)

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
    while (SDL_PollEvent(&event)) {}

    Uint8* keys = SDL_GetKeyState(NULL);
    u32 maskPlayer0 = 0;

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


#elif defined(PLATFORM_PS2)

    struct padButtonStatus buttons;

    if (padRead(0, 0, &buttons) != 0)
    {
        u32 maskPlayer0 = 0;

        u16 btn = ~buttons.btns; // PS2 usa bits invertidos

        if (btn & PAD_CROSS)     maskPlayer0 |= (1u << BUTTON_A);
        if (btn & PAD_CIRCLE)    maskPlayer0 |= (1u << BUTTON_B);
        if (btn & PAD_SQUARE)    maskPlayer0 |= (1u << BUTTON_X);
        if (btn & PAD_TRIANGLE)  maskPlayer0 |= (1u << BUTTON_Y);

        if (btn & PAD_L1)        maskPlayer0 |= (1u << BUTTON_L1);
        if (btn & PAD_R1)        maskPlayer0 |= (1u << BUTTON_R1);

        if (btn & PAD_START)     maskPlayer0 |= (1u << BUTTON_START);
        if (btn & PAD_SELECT)    maskPlayer0 |= (1u << BUTTON_SELECT);

        if (btn & PAD_UP)        maskPlayer0 |= (1u << BUTTON_UP);
        if (btn & PAD_DOWN)      maskPlayer0 |= (1u << BUTTON_DOWN);
        if (btn & PAD_LEFT)      maskPlayer0 |= (1u << BUTTON_LEFT);
        if (btn & PAD_RIGHT)     maskPlayer0 |= (1u << BUTTON_RIGHT);

        system->setPlayerState(0, maskPlayer0);
    }

#endif
}