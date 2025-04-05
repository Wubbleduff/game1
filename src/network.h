#pragma once

#include "common.h"
#include "game_input.h"
#include "game_state.h"

#define NETWORK_NO_USER_ID (u32_MAX)

enum NetworkPacketType
{
    PACKET_INPUTS,
    PACKET_GAME_STATE,
    PACKET_REQUEST_ID,
    PACKET_RESPOND_ID,

    NUM_NETWORK_PACKET_TYPE
};
_Static_assert(NUM_NETWORK_PACKET_TYPE < 256, "NetworkPacketType must fit in a u8.");

struct NetworkPacket
{
    u8 network_packet_type;

    u32 player_id;

    struct PlayerInput player_input;

    struct GameState game_state;
};
_Static_assert(sizeof(struct NetworkPacket) <= 65536, "NetworkPacket larger than a single datagram packet.");

