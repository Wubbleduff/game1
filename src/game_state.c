
#include "common.h"

#include "platform.h"
#include "game_state.h"

void init_game_state(struct GameState* game_state)
{
    game_state->num_humans = 2;
    game_state->human_pos_x[0] = 0.0f;
    game_state->human_pos_y[0] = 0.0f;

    game_state->human_pos_x[1] = 0.0f;
    game_state->human_pos_y[1] = 0.0f;
}

void update_game_state(struct GameState* game_state, const struct GameState* prev_game_state)
{
    (void)prev_game_state;

    const f32 speed = 0.05f;
    game_state->human_pos_x[0] += (f32)is_keyboard_key_down(KB_F) * speed;
    game_state->human_pos_x[0] -= (f32)is_keyboard_key_down(KB_S) * speed;
    game_state->human_pos_y[0] += (f32)is_keyboard_key_down(KB_E) * speed;
    game_state->human_pos_y[0] -= (f32)is_keyboard_key_down(KB_D) * speed;

    game_state->human_pos_x[1] += (f32)is_keyboard_key_down(KB_L) * speed;
    game_state->human_pos_x[1] -= (f32)is_keyboard_key_down(KB_J) * speed;
    game_state->human_pos_y[1] += (f32)is_keyboard_key_down(KB_I) * speed;
    game_state->human_pos_y[1] -= (f32)is_keyboard_key_down(KB_K) * speed;
}

