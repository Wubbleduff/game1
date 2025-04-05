#pragma once

#include "common.h"

struct PlatformWin32Input
{
#define MAX_KEYBOARD_KEYS 256
    u8 keyboard_buf[MAX_KEYBOARD_KEYS / 8];
    u8 prev_keyboard_buf[MAX_KEYBOARD_KEYS / 8];

    u8 mouse_button;
    u8 prev_mouse_button;

    s32 mouse_screen_x;
    s32 mouse_screen_y;
    s32 prev_mouse_screen_x;
    s32 prev_mouse_screen_y;
};

enum KeyboardKey
{
    KB_NOT_SUPPORTED,
    KB_ESCAPE,
    KB_SPACE,
    KB_LCTRL,
    KB_ALT,
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

enum MouseButton
{
    GAME_MB_LEFT,
    GAME_MB_RIGHT,
    GAME_MB_MIDDLE,

    NUM_MOUSE_BUTTONS
};

struct PlatformWin32Input* platform_win32_get_input();

void platform_win32_init_input();

enum KeyboardKey platform_win32_vk_code_to_keyboard_key(u64 vk_key_code);

void platform_win32_set_keyboard_key(const enum KeyboardKey k);
void platform_win32_clear_keyboard_key(const enum KeyboardKey k);

void platform_win32_set_mouse_button(const enum MouseButton m);
void platform_win32_clear_mouse_button(const enum MouseButton m);

void platform_win32_input_sample();
void platform_win32_input_end_frame();

u32 platform_win32_is_keyboard_key_down(enum KeyboardKey k);

void platform_win32_get_mouse_screen_position(s32* x, s32* y);
void platform_win32_get_mouse_screen_delta(s32* x, s32* y);

struct PlayerInput;
void platform_win32_input_to_player_input(struct PlayerInput* player_input);

