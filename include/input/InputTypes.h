#ifndef INPUT_TYPES_H
#define INPUT_TYPES_H

// --------------------------------------------------
// Tipos básicos
// --------------------------------------------------
#include <engine/types.h>
typedef int bool32;

#define INPUT_TRUE  1
#define INPUT_FALSE 0

// --------------------------------------------------
// Configuración del sistema
// --------------------------------------------------

// Máximo de jugadores soportados por el engine
#define INPUT_MAX_PLAYERS 2

// Máximo de botones abstractos
#define INPUT_MAX_BUTTONS 32

// --------------------------------------------------
// Botones abstractos del engine
// (Independientes de la plataforma)
// --------------------------------------------------

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

// --------------------------------------------------
// Estado simple (opcional si lo necesitas)
// --------------------------------------------------

enum InputState
{
    INPUT_UP = 0,
    INPUT_DOWN
};

#endif