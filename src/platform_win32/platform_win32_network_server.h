#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

struct PlatformWin32NetworkServer
{
    SOCKET listen_socket;
};

struct PlatformWin32NetworkServer* platform_win32_get_network_server();

void platform_win32_start_server(const char* listen_ipv4_addr, const u16 listen_port);

