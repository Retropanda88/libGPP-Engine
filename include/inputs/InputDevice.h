#ifndef INPUT_DEVICE_H
#define INPUT_DEVICE_H

#include <inputs/InputTypes.h>

class InputDevice
{
public:
    InputDevice() {}
    virtual ~InputDevice() {}

    // Se llama una vez por frame
    virtual void update() = 0;

    // Estado actual
    virtual int isDown(InputButton button) const = 0;

    // Cambios de estado
    virtual int isPressed(InputButton button) const = 0;
    virtual int isReleased(InputButton button) const = 0;
};

#endif