
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "common.h"
#include "network.h"
#include "platform_win32/platform_win32_network_client.h"

void platform_win32_network_client_init(
        const char* server_ipv4_addr,
        const u16 server_port)
{
    struct PlatformWin32NetworkClient* client = platform_win32_get_network_client();

    s32 err_code;

    // Initialize Winsock
    WSADATA wsa_data;
    err_code = WSAStartup(MAKEWORD(2,2), &wsa_data);
    ASSERT(err_code == 0, "'WSAStartup' failed with error: %d", err_code);

    {
        ZERO_ARRAY(client->server_port_str);
        u32 num = 0;
        u16 a = server_port;
        while(a)
        {
            client->server_port_str[num++] = '0' + (char)(a % 10);
            a /= 10;
        }
        for(u32 i = 0; i < num / 2; i++)
        {
            char tmp = client->server_port_str[i];
            client->server_port_str[i] = client->server_port_str[num - i - 1];
            client->server_port_str[num - i- 1] = tmp;
        }
        client->server_port_str[num] = 0;
    }

    client->sparse_player_id = NETWORK_NO_USER_ID;

    struct addrinfo *server_addr = NULL;
    struct addrinfo hints = {0};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    err_code = getaddrinfo(server_ipv4_addr, client->server_port_str, &hints, &server_addr);
    ASSERT(err_code == 0, "'getaddrinfo' failed with error: %d", err_code);
    ASSERT(server_addr != NULL, "getaddrinfo failed");

    client->server_addr = *server_addr;

    client->server_socket = socket(client->server_addr.ai_family, client->server_addr.ai_socktype, client->server_addr.ai_protocol);
    ASSERT(client->server_socket != INVALID_SOCKET, "'socket' failed with error: %ld", WSAGetLastError());

    u_long nonblocking = 1;
    err_code = ioctlsocket(client->server_socket, FIONBIO, &nonblocking);
    ASSERT(err_code == 0, "ioctlsocket failed with error: %d", WSAGetLastError());
}

void platform_win32_network_client_update(struct GameState* game_state, const struct GameState* prev_game_state, const struct PlayerInput* player_input)
{
    struct PlatformWin32NetworkClient* client = platform_win32_get_network_client();

    // Always populate the next game state last known good.
    *game_state = *prev_game_state;

    s32 last_err;

    if(client->sparse_player_id == NETWORK_NO_USER_ID)
    {
        // Send a request for a player ID.
        struct NetworkPacket packet = {0};
        packet.network_packet_type = PACKET_REQUEST_ID;
        s32 sendto_result = sendto(client->server_socket, (char*)&packet, sizeof(packet), 0, client->server_addr.ai_addr, sizeof(*client->server_addr.ai_addr));
        last_err = WSAGetLastError();
        ASSERT(sendto_result != SOCKET_ERROR && last_err == 0, "'sendto' failed with error: %d", last_err);

        // Listen for response.
        struct NetworkPacket recv_packet = {0};
        const s32 recv_res = recvfrom(
                client->server_socket,
                (char*)&recv_packet,
                sizeof(recv_packet),
                0,
                NULL,
                NULL);
        last_err = WSAGetLastError();
        if(recv_res == SOCKET_ERROR)
        {
            // Expected with non-blocking API. Assert error codes are nominal.
            ASSERT(last_err == WSAEWOULDBLOCK ||
                   last_err == WSAECONNRESET ||
                   last_err == WSAENOTCONN, "'recvfrom' failed with error: %d", last_err);
        }
        else
        {
            // Recieved some bytes.
            ASSERT(recv_res >= 0, "'recvfrom' returned negative value that is not SOCKET_ERROR. Should not be possible at this point.");

            const u32 num_bytes_read = recv_res;
            ASSERT(num_bytes_read == sizeof(recv_packet), "Unexpected number of bytes from 'recvfrom'");

            // TODO(mfritz) sanitize

            // Eventually receive a player ID.
            if(recv_packet.network_packet_type == PACKET_RESPOND_ID)
            {
                client->sparse_player_id = recv_packet.sparse_player_id;
            }
        }
    }
    else
    {
        struct NetworkPacket packet = {0};
        packet.network_packet_type = PACKET_INPUTS;
        packet.sparse_player_id = client->sparse_player_id;
        packet.player_input = *player_input;

        s32 sendto_result = sendto(client->server_socket, (char*)&packet, sizeof(packet), 0, client->server_addr.ai_addr, sizeof(*client->server_addr.ai_addr));
        last_err = WSAGetLastError();
        ASSERT(sendto_result != SOCKET_ERROR && last_err == 0, "'sendto' failed with error: %d", last_err);

        struct NetworkPacket recv_packet = {0};
        const s32 recv_res = recvfrom(
                client->server_socket,
                (char*)&recv_packet,
                sizeof(recv_packet),
                0,
                NULL,
                NULL);
        last_err = WSAGetLastError();
        if(recv_res == SOCKET_ERROR)
        {
            // Expected with non-blocking API. Assert error codes are nominal.
            ASSERT(last_err == WSAEWOULDBLOCK ||
                   last_err == WSAECONNRESET ||
                   last_err == WSAENOTCONN, "'recvfrom' failed with error: %d", last_err);
        }
        else
        {
            // Recieved some bytes.
            ASSERT(recv_res >= 0, "'recvfrom' returned negative value that is not SOCKET_ERROR. Should not be possible at this point.");

            const u32 num_bytes_read = recv_res;
            ASSERT(num_bytes_read == sizeof(recv_packet), "Unexpected number of bytes from 'recvfrom'");

            // TODO(mfritz) sanitize

            if(recv_packet.network_packet_type == PACKET_GAME_STATE)
            {
                *game_state = recv_packet.game_state;
            }
        }
    }
}

