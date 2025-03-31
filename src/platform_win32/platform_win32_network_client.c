
#include "common.h"
#include "platform_win32/platform_win32_network_client.h"

void platform_win32_start_client(const char* server_ipv4_addr, const u16 server_port)
{
    s32 err_code;
    s32 last_err;

    char server_port_str[8];
    {
        u32 num = 0;
        u16 a = server_port;
        while(a)
        {
            server_port_str[num++] = '0' + (char)(a % 10);
            a /= 10;
        }
        for(u32 i = 0; i < num / 2; i++)
        {
            char tmp = server_port_str[i];
            server_port_str[i] = server_port_str[num - i - 1];
            server_port_str[num - i- 1] = tmp;
        }
        server_port_str[num] = 0;
    }

    // Initialize Winsock
    WSADATA wsa_data;
    err_code = WSAStartup(MAKEWORD(2,2), &wsa_data);
    ASSERT(err_code == 0, "WSAStartup failed with error: %d", err_code);

    struct addrinfo *server_addr = NULL;
    struct addrinfo hints = {0};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    err_code = getaddrinfo(server_ipv4_addr, server_port_str, &hints, &server_addr);
    ASSERT(err_code == 0, "getaddrinfo failed with error: %d", err_code);
    ASSERT(server_addr != NULL, "getaddrinfo failed");

    SOCKET server_socket = socket(server_addr->ai_family, server_addr->ai_socktype, server_addr->ai_protocol);
    ASSERT(server_socket != INVALID_SOCKET, "socket failed with error: %ld", WSAGetLastError());
    u_long nonblocking = 1;
    err_code = ioctlsocket(server_socket, FIONBIO, &nonblocking);
    ASSERT(err_code == 0, "ioctlsocket failed with error: %d", WSAGetLastError());
    err_code = connect(server_socket, server_addr->ai_addr, (s32)server_addr->ai_addrlen);
    last_err = WSAGetLastError();
    ASSERT(err_code == 0 || last_err == WSAEWOULDBLOCK, "connect failed with error: %d", last_err);

    const char send_buf[64] = "Send data";
    const u32 num_send_buf = sizeof(send_buf);

    err_code = send(server_socket, send_buf, num_send_buf, 0);
    last_err = WSAGetLastError();
    ASSERT(err_code != SOCKET_ERROR ||
            last_err == WSAECONNRESET ||
            last_err == WSAENOTCONN, "send failed with error: %d", last_err);

}

