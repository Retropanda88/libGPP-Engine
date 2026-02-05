#pragma once

#ifdef PSP_BUILD

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>

#define printf pspDebugScreenPrintf




PSP_MODULE_INFO("test engine", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(20480);

// -------- Exit Callback --------
static int ExitCallback(int arg1, int arg2, void *common)
{
    sceKernelExitGame();
    return 0;
}


void PSP_Exit()
{
    sceKernelExitGame();
}


static int CallbackThread(SceSize args, void *argp)
{
    int cbid = sceKernelCreateCallback("Exit Callback", ExitCallback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

static int PSP_SetupCallbacks()
{
    int thid = sceKernelCreateThread(
        "update_thread",
        CallbackThread,
        0x11,
        0xFA0,
        0,
        0
    );

    if (thid >= 0)
        sceKernelStartThread(thid, 0, 0);

    return thid;
}

#endif // PSP_BUILD
