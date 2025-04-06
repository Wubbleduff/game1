#pragma once

#include "common.h"


#define MAX_PLAYERS 16
#define MAX_WALLS 64

struct GameState
{
    u32 num_players;
    u32 sparse_player_id[MAX_PLAYERS];
    f32 player_pos_x[MAX_PLAYERS];
    f32 player_pos_y[MAX_PLAYERS];
    f32 player_vel_x[MAX_PLAYERS];
    f32 player_vel_y[MAX_PLAYERS];

    u32 num_walls;
    f32 wall_pos_x[MAX_WALLS];
    f32 wall_pos_y[MAX_WALLS];
    f32 wall_width[MAX_WALLS];
    f32 wall_height[MAX_WALLS];
};

void init_game_state(struct GameState* game_state);

struct GameInput;
void update_game_state(struct GameState* game_state, const struct GameState* prev_game_state, const struct GameInput* game_input);

