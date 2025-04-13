#include "common.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "math.h"
#include "game_state.h"
#include "render.h"
#include "game_input.h"
#include "game_constants.h"
#include "platform_win32/platform_win32_common.h"
#include "platform_win32/platform_win32_render.h"
#include "platform_win32/platform_win32_input.h"
#include "platform_win32/platform_win32_network_client.h"

struct MainMemory
{
    struct PlatformWin32Common win32_common;

    struct PlatformWin32Render render;

    struct PlatformWin32Input input;

    struct PlatformWin32NetworkClient network_client;

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

struct PlatformWin32NetworkClient* platform_win32_get_network_client()
{
    return &g_main_memory.network_client;
}

struct PlatformWin32Common* platform_win32_get_common()
{
    return &g_main_memory.win32_common;
}


void WinMainCRTStartup()
{
    platform_win32_init_common();

    platform_win32_network_client_init("www.michaelscottfritz.com", 4242);

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

        platform_win32_input_sample();

        if(platform_win32_is_keyboard_key_down(KB_ESCAPE))
        {
            break;
        }

        if(platform_win32_is_keyboard_key_down(KB_LCTRL))
        {
            s32 mouse_dx;
            s32 mouse_dy;
            platform_win32_get_mouse_screen_delta(&mouse_dx, &mouse_dy);

            // TODO(mfritz) Move into common module
            struct PlatformWin32Common* common = platform_win32_get_common();
            const HWND hwnd = common->hwnd;
            RECT window_rect;
            const BOOL get_window_rect_result = GetWindowRect(hwnd, &window_rect);
            ASSERT(get_window_rect_result, "GetWindowRect failed.");

            s32 new_pos_x = window_rect.left + mouse_dx;
            s32 new_pos_y = window_rect.top + mouse_dy;

            const BOOL set_window_pos_result = SetWindowPos(
              hwnd,
              0,
              new_pos_x,
              new_pos_y,
              window_rect.right - window_rect.left,
              window_rect.bottom - window_rect.top,
              0);
            ASSERT(set_window_pos_result, "SetWindowPos failed.");
        }

        if(frame_timer_ns < FRAME_DURATION_NS)
        {
            _mm_pause();
            continue;
        }
        frame_timer_ns -= FRAME_DURATION_NS;

        struct PlayerInput player_input;
        platform_win32_input_to_player_input(&player_input);

        platform_win32_network_client_update(&g_main_memory.game_state, &player_input);

        // update_game_state(&g_main_memory.game_state, &g_main_memory.prev_game_state, &game_input);

        {
            struct PlatformWin32NetworkClient* client = platform_win32_get_network_client();
            struct GameState* game_state = &g_main_memory.game_state;
            struct PlatformWin32Render* render = platform_win32_get_render();
            const u32 dense_player_id = sparse_to_dense_player_id(game_state, client->sparse_player_id);
            render->camera.pos_x = game_state->player_pos_x[dense_player_id];
            render->camera.pos_y = game_state->player_pos_y[dense_player_id];
        }

        platform_win32_render_game_state(&g_main_memory.game_state);
        platform_win32_input_end_frame();
    }

    ExitProcess(0);
}


