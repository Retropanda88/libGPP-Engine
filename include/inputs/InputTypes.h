#ifndef INPUT_TYPES_H
#define INPUT_TYPES_H

// Máximo de botones abstractos soportados
#define INPUT_MAX_BUTTONS 32

// Botones abstractos del engine
enum InputButton
{
    BUTTON_A = 0,
    BUTTON_B,
    BUTTON_X,
    BUTTON_Y,

    BUTTON_L1,
    BUTTON_R1,

    BUTTON_START,
    BUTTON_SELECT,

    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,

    BUTTON_COUNT
};

// Estado simple
enum InputState
{
    INPUT_UP = 0,
    INPUT_DOWN
};

#endif