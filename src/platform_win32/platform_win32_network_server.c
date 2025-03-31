
#include "common.h"
#include "platform_win32/platform_win32_network_server.h"

void platform_win32_start_server(const char* listen_ipv4_addr, const u16 listen_port)
{
    struct PlatformWin32NetworkServer* server = platform_win32_get_network_server();

    s32 err_code;

    char listen_port_str[8];
    {
        u32 num = 0;
        u16 a = listen_port;
        while(a)
        {
            listen_port_str[num++] = '0' + (char)(a % 10);
            a /= 10;
        }
        for(u32 i = 0; i < num / 2; i++)
        {
            char tmp = listen_port_str[i];
            listen_port_str[i] = listen_port_str[num - i - 1];
            listen_port_str[num - i- 1] = tmp;
        }
        listen_port_str[num] = 0;
    }

    // Initialize Winsock
    WSADATA wsa_data;
    err_code = WSAStartup(MAKEWORD(2,2), &wsa_data);
    ASSERT(err_code == 0, "WSAStartup failed with error: %d", err_code);

    SOCKET listen_socket = INVALID_SOCKET;

    struct addrinfo hints = {0};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    struct addrinfo *addr = NULL;
    err_code = getaddrinfo(listen_ipv4_addr, listen_port_str, &hints, &addr);
    ASSERT(err_code == 0, "getaddrinfo failed with error: %d", err_code);
    ASSERT(addr != NULL, "getaddrinfo failed");

    listen_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    ASSERT(listen_socket != INVALID_SOCKET, "socket failed with error: %ld", WSAGetLastError());

    u_long nonblocking = 1;
    err_code = ioctlsocket(listen_socket, FIONBIO, &nonblocking);
    ASSERT(err_code == 0, "ioctlsocket failed with error: %d", WSAGetLastError());

    err_code = bind(listen_socket, addr->ai_addr, (s32)addr->ai_addrlen);
    ASSERT(err_code == 0, "bind failed with error: %d", WSAGetLastError());

    err_code = listen(listen_socket, SOMAXCONN);
    ASSERT(err_code == 0, "listen failed with error: %d", WSAGetLastError());

    freeaddrinfo(addr);

    server->listen_socket = listen_socket;
}

