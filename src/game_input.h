#pragma once

#include "common.h"
#include "game_constants.h"

struct PlayerInput
{
    u8 move_x;
    u8 move_y;
};

struct GameInput
{
    u32 num_players;
    u32 player_id[MAX_ACTIVE_PLAYERS];
    struct PlayerInput player_input[MAX_ACTIVE_PLAYERS];
};


