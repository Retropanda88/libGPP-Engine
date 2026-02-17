#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

#include <inputs/InputDevice.h>

#define INPUT_MAX_DEVICES 4

class InputSystem
{
public:
    static InputSystem& instance();

    void addDevice(InputDevice* device);
    void update();

    int isDown(int deviceIndex, InputButton button) const;
    int isPressed(int deviceIndex, InputButton button) const;
    int isReleased(int deviceIndex, InputButton button) const;

private:
    InputSystem();
    ~InputSystem();

    // Evitar copias
    InputSystem(const InputSystem&);
    InputSystem& operator=(const InputSystem&);

private:
    InputDevice* m_devices[INPUT_MAX_DEVICES];
    int m_deviceCount;
};

#endif