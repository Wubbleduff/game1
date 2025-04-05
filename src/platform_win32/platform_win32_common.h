#pragma once

#include "common.h"

struct PlatformWin32Common
{
    HWND hwnd;

    s64 clock_freq_hz;
};

struct PlatformWin32Common* platform_win32_get_common();

void platform_win32_init_common(const u32 window_x, const u32 window_y);

s64 platform_win32_get_time_ns();

