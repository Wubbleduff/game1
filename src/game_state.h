#pragma once

#include "common.h"

#define MAX_PLAYERS 4
#define MAX_ZOMBIES 8

struct GameState
{
    u32 num_humans;
    f32 human_pos_x[MAX_PLAYERS];
    f32 human_pos_y[MAX_PLAYERS];

    u32 num_zombies;
    f32 zombie_pos_x[MAX_ZOMBIES];
    f32 zombie_pos_y[MAX_ZOMBIES];
};

void init_game_state(struct GameState* game_state);

void update_game_state(struct GameState* game_state, const struct GameState* prev_game_state);

