#include <inputs/InputSystem.h>

InputSystem::InputSystem()
: m_deviceCount(0)
{
    for (int i = 0; i < INPUT_MAX_DEVICES; i++)
        m_devices[i] = 0;
}

InputSystem::~InputSystem()
{
}

InputSystem& InputSystem::instance()
{
    static InputSystem s_instance;
    return s_instance;
}

void InputSystem::addDevice(InputDevice* device)
{
    if (m_deviceCount < INPUT_MAX_DEVICES)
    {
        m_devices[m_deviceCount++] = device;
    }
}

void InputSystem::update()
{
    for (int i = 0; i < m_deviceCount; i++)
    {
        m_devices[i]->update();
    }
}

int InputSystem::isDown(int deviceIndex, InputButton button) const
{
    if (deviceIndex >= m_deviceCount)
        return 0;

    return m_devices[deviceIndex]->isDown(button);
}

int InputSystem::isPressed(int deviceIndex, InputButton button) const
{
    if (deviceIndex >= m_deviceCount)
        return 0;

    return m_devices[deviceIndex]->isPressed(button);
}

int InputSystem::isReleased(int deviceIndex, InputButton button) const
{
    if (deviceIndex >= m_deviceCount)
        return 0;

    return m_devices[deviceIndex]->isReleased(button);
}