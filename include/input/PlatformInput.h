#ifndef PLATFORM_INPUT_H
#define PLATFORM_INPUT_H

#include <input/InputTypes.h>

class InputSystem; // Forward declaration

class PlatformInput
{
public:

    // Inicializa el hardware
    void initHardware();

    // Detecta dispositivos conectados
    void detectDevices(InputSystem* system);

    // Lee estado actual del hardware
    void poll(InputSystem* system);
};

#endif