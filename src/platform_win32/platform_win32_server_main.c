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
#include "platform_win32/platform_win32_network_server.h"

struct MainMemory
{
    struct PlatformWin32Common win32_common;

    struct PlatformWin32Render render;

    struct PlatformWin32Input input;

    struct PlatformWin32NetworkServer network_server;

    u32 next_game_state_idx;
    struct GameState game_state[2];
};

static struct MainMemory g_main_memory;

struct PlatformWin32Render* platform_win32_get_render()
{
    return &g_main_memory.render;
}

struct PlatformWin32NetworkServer* platform_win32_get_network_server()
{
    return &g_main_memory.network_server;
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

    platform_win32_start_server("192.168.1.2", 4242);

    g_main_memory.next_game_state_idx = 1;
    init_game_state(&g_main_memory.game_state[0]);

    s64 frame_timer_ns = 0;
    s64 last_frame_time_ns = platform_win32_get_time_ns();


    s64 frame_num = 0;
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

        if(frame_timer_ns < FRAME_DURATION_NS)
        {
            _mm_pause();
            continue;
        }
        frame_timer_ns -= FRAME_DURATION_NS;

        struct PlayerInput player_input;
        platform_win32_input_to_player_input(&player_input);

        // Add server local player input.
        const u32 sparse_player_id = 0;
        const u32 dense_player_id = 0;
        struct GameInput game_input;
        game_input_init(&game_input);
        game_input_add_player_input(&game_input, sparse_player_id, &player_input);

        platform_win32_server_receive_client_inputs(frame_num, &game_input);

        const u32 next_game_state_idx = g_main_memory.next_game_state_idx;
        const u32 prev_game_state_idx = (next_game_state_idx + 1) & 1;
        const struct GameState* prev_game_state = &g_main_memory.game_state[prev_game_state_idx];
        struct GameState* next_game_state = &g_main_memory.game_state[next_game_state_idx];
        update_game_state(next_game_state, prev_game_state, &game_input);

        {
            struct PlatformWin32Render* render = platform_win32_get_render();
            render->camera.pos_x = next_game_state->player_pos_x[dense_player_id];
            render->camera.pos_y = next_game_state->player_pos_y[dense_player_id];
        }

        platform_win32_server_send_game_state(next_game_state);

        platform_win32_render_game_state(next_game_state, dense_player_id);

        g_main_memory.next_game_state_idx = prev_game_state_idx;

        platform_win32_input_end_frame();
        frame_num++;
    }

    ExitProcess(0);
}


