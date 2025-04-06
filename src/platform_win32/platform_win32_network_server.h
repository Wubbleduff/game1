#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "game_constants.h"

struct PlatformWin32NetworkServer
{
    SOCKET client_socket;

    u32 next_sparse_player_id;

    u32 num_players;
    u32 sparse_player_id[MAX_ACTIVE_PLAYERS];
    s64 player_last_frame_heard_from[MAX_ACTIVE_PLAYERS];
    struct PlayerInput player_input[MAX_ACTIVE_PLAYERS];
    struct sockaddr player_addr[MAX_ACTIVE_PLAYERS];
};

struct PlatformWin32NetworkServer* platform_win32_get_network_server();

void platform_win32_start_server(const char* listen_ipv4_addr, const u16 listen_port);

struct GameInput;
struct GameState;
void platform_win32_server_receive_client_inputs(const s64 frame_num, struct GameInput* GameInput);
void platform_win32_server_send_game_state(const struct GameState* game_state);

