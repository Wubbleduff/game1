#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

struct PlatformWin32NetworkClient
{
    SOCKET server_socket;

    struct addrinfo server_addr;

    char server_port_str[8];

    u32 player_id;
};

struct PlatformWin32NetworkClient* platform_win32_get_network_client();

void platform_win32_network_client_init(const char* server_ipv4_addr, const u16 server_port);

struct PlayerInput;
struct GameState;
void platform_win32_network_client_update(struct GameState* game_state, const struct PlayerInput* player_input);

