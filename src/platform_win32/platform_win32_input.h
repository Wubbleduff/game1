#pragma once

#include "common.h"
#include "platform.h"

struct PlatformWin32Input
{
#define MAX_KEYBOARD_KEYS 256
    u8 keyboard_buf[MAX_KEYBOARD_KEYS / 8];
    u8 prev_keyboard_buf[MAX_KEYBOARD_KEYS / 8];
};

struct PlatformWin32Input* platform_win32_get_input();

void platform_win32_init_input();

enum KeyboardKey platform_win32_vk_code_to_keyboard_key(u64 vk_key_code);

void platform_win32_set_keyboard_key(const enum KeyboardKey k);
void platform_win32_clear_keyboard_key(const enum KeyboardKey k);

void platform_win32_input_end_frame();

