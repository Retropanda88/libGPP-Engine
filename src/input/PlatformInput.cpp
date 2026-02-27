

#include <input/PlatformInput.h>
#include <input/InputSystem.h>

 //======================================================
// Includes por plataforma
// ======================================================

#include <stdio.h>

#if defined(ANDROID_BUILD) || defined(PC_BUILD)
    #include <SDL/SDL.h>
#endif

#if defined(PSP_BUILD)
    #include <pspctrl.h>
#endif

#if defined(PS2_BUILD)
    #include <tamtypes.h>
    #include <kernel.h>
    #include <sifrpc.h>
    #include <loadfile.h>
    #include <iopcontrol.h>
    #include <libpad.h>
    //drivers
    #include <input/sio2man.h>
    #include <input/padman.h>

    /*extern unsigned char sio2man_irx[];
    extern unsigned int sio2man_irx_size;

    extern unsigned char padman_irx[];
    extern unsigned int padman_irx_size;*/

    static unsigned char padBuf[256] __attribute__((aligned(64)));
#endif


// ======================================================
// Inicialización
// ======================================================

void PlatformInput::initHardware()
{

#if defined(ANDROID_BUILD) || defined(PC_BUILD)

    SDL_EnableKeyRepeat(0, 0);


#elif defined(PSP_BUILD)

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);


#elif defined(PS2_BUILD)

    SifInitRpc(0);
    SifLoadFileInit();   

    int ret;

    ret = SifExecModuleBuffer((void*)sio2man_irx, sio2man_irx_size, 0, NULL, NULL);
    if (ret < 0)
        printf("SIO2MAN load failed: %d\n", ret);

    ret = SifExecModuleBuffer((void*)padman_irx, padman_irx_size, 0, NULL, NULL);
    if (ret < 0)
        printf("PADMAN load failed: %d\n", ret);

    printf("load ok modulos irx.....\n\n");

    padInit(0);

    int port = 0;
    int slot = 0;

    if (!padPortOpen(port, slot, padBuf))
    {
        printf("padPortOpen failed\n");
        return;
    }

    // Esperar a que el pad esté listo
    int state;
    while ((state = padGetState(port, slot)) != PAD_STATE_STABLE &&
           state != PAD_STATE_FINDCTP1)
    {
        if (state == PAD_STATE_DISCONN)
        {
            printf("Pad disconnected\n");
            return;
        }
    }

    //  Forzar modo DualShock
    padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

    while (padGetState(port, slot) != PAD_STATE_STABLE);

#endif
}


// ======================================================
// Detectar dispositivos
// ======================================================

void PlatformInput::detectDevices(InputSystem* system)
{

#if defined(ANDROID_BUILD) || defined(PC_BUILD)

    system->setConnected(0, INPUT_TRUE);
    system->setConnected(1, SDL_NumJoysticks() > 0 ? INPUT_TRUE : INPUT_FALSE);

#elif defined(PSP_BUILD)

    system->setConnected(0, INPUT_TRUE);
    system->setConnected(1, INPUT_FALSE);

#elif defined(PS2_BUILD)

    system->setConnected(0, INPUT_TRUE);
    system->setConnected(1, INPUT_FALSE);

#endif
}


// ======================================================
// Poll hardware
// ======================================================

void PlatformInput::poll(InputSystem* system)
{

#if defined(ANDROID_BUILD) || defined(PC_BUILD)

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


#elif defined(PSP_BUILD)

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


#elif defined(PS2_BUILD)

    struct padButtonStatus buttons;

    int port = 0;
    int slot = 0;

    int state = padGetState(port, slot);

    if (state != PAD_STATE_STABLE &&
        state != PAD_STATE_FINDCTP1)
        return;

    if (padRead(port, slot, &buttons) > 0)
    {
        // EXACTO como demo oficial
        u32 paddata = 0xffff ^ buttons.btns;
        u32 mask = 0;

        if (paddata & PAD_CROSS)     mask |= (1u << BUTTON_A);
        if (paddata & PAD_CIRCLE)    mask |= (1u << BUTTON_B);
        if (paddata & PAD_SQUARE)    mask |= (1u << BUTTON_X);
        if (paddata & PAD_TRIANGLE)  mask |= (1u << BUTTON_Y);

        if (paddata & PAD_L1)        mask |= (1u << BUTTON_L1);
        if (paddata & PAD_R1)        mask |= (1u << BUTTON_R1);

        if (paddata & PAD_START)     mask |= (1u << BUTTON_START);
        if (paddata & PAD_SELECT)    mask |= (1u << BUTTON_SELECT);

        if (paddata & PAD_UP)        mask |= (1u << BUTTON_UP);
        if (paddata & PAD_DOWN)      mask |= (1u << BUTTON_DOWN);
        if (paddata & PAD_LEFT)      mask |= (1u << BUTTON_LEFT);
        if (paddata & PAD_RIGHT)     mask |= (1u << BUTTON_RIGHT);

        system->setPlayerState(0, mask);
    }

#endif
}