
#include "common.h"

#include "math.h"
#include "game_input.h"
#include "game_state.h"

#include "level_0.h"

void init_game_state(struct GameState* game_state)
{
    game_state->num_players = 0;
    game_state->num_walls = 0;
}

void update_game_state(struct GameState* game_state, const struct GameState* prev_game_state, const struct GameInput* game_input)
{
    (void)prev_game_state;

    for(u32 i_player = 0; i_player < game_input->num_players; i_player++)
    {
        game_state->sparse_player_id[i_player] = game_input->sparse_player_id[i_player];
        const f32 speed = 0.05f;
        v2 move_dir = make_v2(
            ((f32)game_input->player_input[i_player].move_x * (1.0f/254.0f) - 0.5f) * 2.0f,
            ((f32)game_input->player_input[i_player].move_y * (1.0f/254.0f) - 0.5f) * 2.0f
        );
        move_dir = v2_scale(v2_normalize_or_zero(move_dir), speed);
        game_state->player_pos_x[i_player] += move_dir.x;
        game_state->player_pos_y[i_player] += move_dir.y;
    }
    game_state->num_players = game_input->num_players;

    game_state->num_walls = LEVEL_0_NUM_WALLS;
    COPY(game_state->wall_pos_x, level_0_wall_pos_x, LEVEL_0_NUM_WALLS);
    COPY(game_state->wall_pos_y, level_0_wall_pos_y, LEVEL_0_NUM_WALLS);
    COPY(game_state->wall_width, level_0_wall_width, LEVEL_0_NUM_WALLS);
    COPY(game_state->wall_height, level_0_wall_height, LEVEL_0_NUM_WALLS);

}

