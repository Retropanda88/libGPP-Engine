#ifndef GPP_INPUT_H
#define GPP_INPUT_H

namespace gpp
{
    // Acciones lógicas (independientes del hardware)
    enum InputAction
    {
        ACTION_NONE = 0,

        ACTION_UP,
        ACTION_DOWN,
        ACTION_LEFT,
        ACTION_RIGHT,

        ACTION_CONFIRM,
        ACTION_CANCEL,
        ACTION_MENU,
        ACTION_BACK,

        ACTION_MAX
    };

    // Estados de una acción
    enum InputState
    {
        STATE_IDLE = 0,
        STATE_PRESSED,
        STATE_HELD,
        STATE_RELEASED
    };

    // Evento de entrada
    struct InputEvent
    {
        InputAction action;
        InputState  state;
    };

    class Input
    {
    public:
        Input();

        void init();
        void shutdown();

        // Debe llamarse una vez por frame
        void update();

        // Llamado por la capa de plataforma (PS2, GC, PSP, PC)
        void setActionState(InputAction action, bool pressed);

        // Consultas de estado
        bool isPressed(InputAction action) const;
        bool isHeld(InputAction action) const;
        bool isReleased(InputAction action) const;

        // Sistema de eventos tipo consola / Linux
        bool pollEvent(InputEvent& event);

    private:
        InputState m_states[ACTION_MAX];
        InputState m_prevStates[ACTION_MAX];
    };
}

#endif // GPP_INPUT_H