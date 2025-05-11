#pragma once

#include "common.h"
#include "game_constants.h"

struct PlayerInput
{
    u8 move_x;
    u8 move_y;
    u8 shoot;
    f32 shoot_dir_x;
    f32 shoot_dir_y;
};

struct GameInput
{
    u32 num_players;
    u32 sparse_player_id[MAX_ACTIVE_PLAYERS];
    f32 player_move_x[MAX_ACTIVE_PLAYERS];
    f32 player_move_y[MAX_ACTIVE_PLAYERS];
    // TODO(mfritz) bitarray
    u8 player_shoot[MAX_ACTIVE_PLAYERS];
    f32 player_shoot_dir_x[MAX_ACTIVE_PLAYERS];
    f32 player_shoot_dir_y[MAX_ACTIVE_PLAYERS];
};

static inline void game_input_init(struct GameInput* game_input)
{
    game_input->num_players = 0;
}

static inline void game_input_add_player_input(struct GameInput* game_input, const u32 sparse_player_id, const struct PlayerInput* player_input)
{
    const u32 num = game_input->num_players;
    ASSERT(num < MAX_ACTIVE_PLAYERS, "'game_input_add_player_input' overflow %u / %u", num, MAX_ACTIVE_PLAYERS);

    game_input->sparse_player_id[num] = sparse_player_id;
    game_input->player_move_x[num] = ((f32)player_input->move_x * (1.0f/254.0f) - 0.5f) * 2.0f;
    game_input->player_move_y[num] = ((f32)player_input->move_y * (1.0f/254.0f) - 0.5f) * 2.0f;
    game_input->player_shoot[num] = player_input->shoot;
    game_input->player_shoot_dir_x[num] = player_input->shoot_dir_x;
    game_input->player_shoot_dir_y[num] = player_input->shoot_dir_y;

    game_input->num_players++;
}


