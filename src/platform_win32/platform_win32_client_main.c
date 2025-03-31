#include "common.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "platform.h"
#include "math.h"
#include "game_state.h"
#include "render.h"
#include "platform_win32/platform_win32_common.h"
#include "platform_win32/platform_win32_render.h"
#include "platform_win32/platform_win32_input.h"
#include "platform_win32/platform_win32_network_client.h"

#define FRAME_DURATION_NS 8333333

struct MainMemory
{
    struct PlatformWin32Common win32_common;

    struct PlatformWin32Render render;

    struct PlatformWin32Input input;

    struct GameState game_state;
    struct GameState prev_game_state;
};

static struct MainMemory g_main_memory;

struct PlatformWin32Render* platform_win32_get_render()
{
    return &g_main_memory.render;
}

struct PlatformWin32Input* platform_win32_get_input()
{
    return &g_main_memory.input;
}

struct PlatformWin32Common* platform_win32_get_common()
{
    return &g_main_memory.win32_common;
}


void WinMainCRTStartup()
{
    platform_win32_init_common();

    init_game_state(&g_main_memory.game_state);
    init_game_state(&g_main_memory.prev_game_state);

    s64 frame_timer_ns = 0;
    s64 last_frame_time_ns = platform_win32_get_time_ns();

    u64 running = 1;
    while(running)
    {
        const s64 frame_start_ns = platform_win32_get_time_ns();
        s64 frame_duration_ns = frame_start_ns - last_frame_time_ns;
        last_frame_time_ns = frame_start_ns;
        frame_timer_ns += frame_duration_ns;

        MSG msg;
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT)
            {
                running = 0;
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if(is_keyboard_key_down(KB_ESCAPE))
        {
            break;
        }

        if(frame_timer_ns < FRAME_DURATION_NS)
        {
            _mm_pause();
            continue;
        }
        frame_timer_ns -= FRAME_DURATION_NS;

        update_game_state(&g_main_memory.game_state, &g_main_memory.prev_game_state);

        platform_win32_render_game_state(&g_main_memory.game_state);
        platform_win32_input_end_frame();
    }

    ExitProcess(0);
}


