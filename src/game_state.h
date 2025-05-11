#pragma once

#include "common.h"
#include "game_constants.h"

#define MAX_BULLETS 1024
#define MAX_WALLS 64

struct GameState
{
    // Player data. Element 'MAX_ACTIVE_PLAYERS' is reserved for default player data (for when a player is added to the game). Ensure there is room.
    u32 num_players;
    u32 sparse_player_id[MAX_ACTIVE_PLAYERS + 8];
    f32 player_pos_x[MAX_ACTIVE_PLAYERS + 8];
    f32 player_pos_y[MAX_ACTIVE_PLAYERS + 8];
    f32 player_vel_x[MAX_ACTIVE_PLAYERS + 8];
    f32 player_vel_y[MAX_ACTIVE_PLAYERS + 8];
    f32 player_radius[MAX_ACTIVE_PLAYERS + 8];

    u32 num_bullets;
    f32 bullet_pos_x[MAX_BULLETS];
    f32 bullet_pos_y[MAX_BULLETS];
    f32 bullet_vel_x[MAX_BULLETS];
    f32 bullet_vel_y[MAX_BULLETS];
    u8 bullet_is_alive[MAX_BULLETS];
    s32 bullet_time_to_live[MAX_BULLETS];

    u32 num_walls;
    f32 wall_pos_x[MAX_WALLS];
    f32 wall_pos_y[MAX_WALLS];
    f32 wall_width[MAX_WALLS];
    f32 wall_height[MAX_WALLS];
};

void init_game_state(struct GameState* game_state);

u32 sparse_to_dense_player_id(const struct GameState* game_state, const u32 sparse_player_id);

struct GameInput;
void update_game_state(struct GameState* game_state, const struct GameState* prev_game_state, const struct GameInput* game_input);

