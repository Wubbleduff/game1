#pragma once

#include "common.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Input

enum KeyboardKey
{
    KB_NOT_SUPPORTED,
    KB_ESCAPE,
    KB_SPACE,
    KB_LCTRL,
    KB_0,
    KB_1,
    KB_2,
    KB_3,
    KB_4,
    KB_5,
    KB_6,
    KB_7,
    KB_8,
    KB_9,
    KB_A,
    KB_B,
    KB_C,
    KB_D,
    KB_E,
    KB_F,
    KB_G,
    KB_H,
    KB_I,
    KB_J,
    KB_K,
    KB_L,
    KB_M,
    KB_N,
    KB_O,
    KB_P,
    KB_Q,
    KB_R,
    KB_S,
    KB_T,
    KB_U,
    KB_V,
    KB_W,
    KB_X,
    KB_Y,
    KB_Z,

    NUM_KEYBOARD_KEYS,
};
u32 is_keyboard_key_down(enum KeyboardKey k);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
