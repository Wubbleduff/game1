
#include "common.h"
#include "network.h"
#include "game_input.h"
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
    ASSERT(err_code == 0, "'WSAStartup' failed with error: %d", err_code);

    struct addrinfo hints = {0};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    struct addrinfo *addr = NULL;
    err_code = getaddrinfo(listen_ipv4_addr, listen_port_str, &hints, &addr);
    ASSERT(err_code == 0, "'getaddrinfo' failed with error: %d", err_code);
    ASSERT(addr != NULL, "'getaddrinfo' failed");

    const SOCKET client_socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    ASSERT(client_socket != INVALID_SOCKET, "'socket' failed with error: %ld", WSAGetLastError());

    u_long nonblocking = 1;
    err_code = ioctlsocket(client_socket, FIONBIO, &nonblocking);
    ASSERT(err_code == 0, "'ioctlsocket' failed with error: %d", WSAGetLastError());

    err_code = bind(client_socket, addr->ai_addr, (s32)addr->ai_addrlen);
    ASSERT(err_code == 0, "'bind' failed with error: %d", WSAGetLastError());

    freeaddrinfo(addr);

    server->client_socket = client_socket;

    // Player ID 0 will be reserved for the server local player.
    server->next_sparse_player_id = 1;

    server->num_players = 0;
}

void platform_win32_server_receive_client_inputs(const s64 frame_num, struct GameInput* game_input)
{
    struct PlatformWin32NetworkServer* server = platform_win32_get_network_server();

    for(u32 iter = 0; iter < MAX_ACTIVE_PLAYERS * 2; iter++)
    {
        s32 last_err;
        struct NetworkPacket recv_packet = {0};
        struct sockaddr recv_addr = {0};
        s32 recv_addr_size = sizeof(struct sockaddr);
        const s32 recv_res = recvfrom(
                server->client_socket,
                (char*)&recv_packet,
                sizeof(recv_packet),
                0,
                &recv_addr,
                &recv_addr_size);
        ASSERT(recv_addr_size = sizeof(struct sockaddr), "Unexpected addr size");
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
            ASSERT(recv_res >= 0, "'recvfrom' returned 0 or negative value that is not SOCKET_ERROR. Should not be possible at this point.");

            const u32 num_bytes_read = recv_res;
            ASSERT(num_bytes_read == sizeof(recv_packet), "Unexpected number of bytes from 'recvfrom'");

            // TODO(mfritz) sanitize

            if(recv_packet.network_packet_type == PACKET_INPUTS)
            {
                u32 found_dense_player_id = u32_MAX;
                for(u32 i = 0; i < server->num_players; i++)
                {
                    found_dense_player_id = server->sparse_player_id[i] == recv_packet.sparse_player_id ? i : found_dense_player_id;
                }
                if(found_dense_player_id != u32_MAX)
                {
                    server->player_last_frame_heard_from[found_dense_player_id] = frame_num;
                    server->player_input[found_dense_player_id] = recv_packet.player_input;
                }
                else if(server->num_players < MAX_ACTIVE_PLAYERS)
                {
                    u32 new_dense_player_id = server->num_players;
                    server->sparse_player_id[new_dense_player_id] = recv_packet.sparse_player_id;
                    server->player_last_frame_heard_from[new_dense_player_id] = frame_num;
                    server->player_input[new_dense_player_id] = recv_packet.player_input;
                    server->player_addr[new_dense_player_id] = recv_addr;
                    server->num_players++;
                    ASSERT(server->num_players < MAX_ACTIVE_PLAYERS, "Active player overflow.");
                }
            }

            if(recv_packet.network_packet_type == PACKET_REQUEST_ID)
            {
                struct NetworkPacket packet = {0};
                packet.network_packet_type = PACKET_RESPOND_ID;
                packet.sparse_player_id = server->next_sparse_player_id;
                s32 sendto_result = sendto(server->client_socket, (char*)&packet, sizeof(packet), 0, &recv_addr, recv_addr_size);
                last_err = WSAGetLastError();
                ASSERT(sendto_result != SOCKET_ERROR && last_err == 0, "'sendto' failed with error: %d", last_err);
                server->next_sparse_player_id++;
                ASSERT(server->next_sparse_player_id < u32_MAX, "Player ID overflow.");
            }
        }
    }

    // Remove players we haven't heard from in a while.
    for(u32 dense_player_id = 0; dense_player_id < server->num_players; dense_player_id++)
    {
        if(frame_num - server->player_last_frame_heard_from[dense_player_id] > 32)
        {
            const u32 end = server->num_players - 1;
            server->sparse_player_id[dense_player_id] = server->sparse_player_id[end];
            server->player_last_frame_heard_from[dense_player_id] = server->player_last_frame_heard_from[end];
            server->player_input[dense_player_id] = server->player_input[end];
            server->num_players--;
        }
    }

    // Populate game input with players.
    for(u32 i_player = 0; i_player < server->num_players; i_player++)
    {
        game_input_add_player_input(game_input, server->sparse_player_id[i_player], server->player_input + i_player);
    }
}

void platform_win32_server_send_game_state(const struct GameState* game_state)
{
    struct PlatformWin32NetworkServer* server = platform_win32_get_network_server();

    for(u32 dense_player_id = 0; dense_player_id < server->num_players; dense_player_id++)
    {
        struct NetworkPacket packet = {0};
        packet.network_packet_type = PACKET_GAME_STATE;
        packet.game_state = *game_state;
        s32 sendto_result = sendto(server->client_socket, (char*)&packet, sizeof(packet), 0, &server->player_addr[dense_player_id], sizeof(server->player_addr[dense_player_id]));
        s32 last_err = WSAGetLastError();
        ASSERT(sendto_result != SOCKET_ERROR && last_err == 0, "'sendto' failed with error: %d", last_err);
    }
}

