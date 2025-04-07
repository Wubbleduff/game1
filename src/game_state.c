
#include "common.h"

#include "math.h"
#include "game_input.h"
#include "game_state.h"

#include "level_0.h"

void init_game_state(struct GameState* game_state)
{
    game_state->num_players = 0;
    game_state->num_walls = 0;

    game_state->player_pos_x[MAX_ACTIVE_PLAYERS] = 0.0f;
    game_state->player_pos_y[MAX_ACTIVE_PLAYERS] = 0.0f;
    game_state->player_vel_x[MAX_ACTIVE_PLAYERS] = 0.0f;
    game_state->player_vel_y[MAX_ACTIVE_PLAYERS] = 0.0f;
}

u32 sparse_to_dense_player_id(const struct GameState* game_state, const u32 sparse_player_id)
{
    u32 res = MAX_ACTIVE_PLAYERS;
    for(u32 i = 0; i < game_state->num_players; i++)
    {
        res = sparse_player_id == game_state->sparse_player_id[i] ? i : res;
    }
    return res;
}

void update_game_state(struct GameState* game_state, const struct GameState* prev_game_state, const struct GameInput* game_input)
{
    {
        _Static_assert(MAX_ACTIVE_PLAYERS >= 8, "Not wide enough.");
        _Static_assert(MAX_ACTIVE_PLAYERS % 8 == 0, "Not 8 aligned.");

        u32 prev_game_state_dense_player_id[MAX_ACTIVE_PLAYERS] = {};

        const u32 num_players = game_input->num_players;

        // Map current dense player IDs to prev game state dense player IDs.
        for(u32 dense_player_id = 0; dense_player_id < num_players; dense_player_id++)
        {
            const u32 sparse_player_id = game_input->sparse_player_id[dense_player_id];

            u32 result = MAX_ACTIVE_PLAYERS;
            for(u32 i_prev = 0; i_prev < prev_game_state->num_players; i_prev++)
            {
                result =
                    prev_game_state->sparse_player_id[i_prev] == sparse_player_id
                    ? i_prev
                    : result;
            }
            prev_game_state_dense_player_id[dense_player_id] = result;
        }

        for(u64 dense_player_id = 0; dense_player_id < game_input->num_players; dense_player_id += 8)
        {
            _mm256_store_si256(
                    (__m256i*)(game_state->sparse_player_id + dense_player_id),
                    _mm256_load_si256((const __m256i*)(game_input->sparse_player_id + dense_player_id)));

            const __m256i prev_dense_player_id = _mm256_load_si256((const __m256i*)(prev_game_state_dense_player_id + dense_player_id));

            const __m256 dt = _mm256_set1_ps(FRAME_DURATION_NS / 1000000000.0f);
            const __m256 max_accel = _mm256_set1_ps(400.0f);
            const __m256 player_drag = _mm256_set1_ps(-40.0f);

            const __m256 prev_player_pos_x = _mm256_i32gather_ps(prev_game_state->player_pos_x, prev_dense_player_id, 4);
            const __m256 prev_player_pos_y = _mm256_i32gather_ps(prev_game_state->player_pos_y, prev_dense_player_id, 4);
            const __m256 prev_player_vel_x = _mm256_i32gather_ps(prev_game_state->player_vel_x, prev_dense_player_id, 4);
            const __m256 prev_player_vel_y = _mm256_i32gather_ps(prev_game_state->player_vel_y, prev_dense_player_id, 4);

            __m256 accel_x = _mm256_load_ps(game_input->player_move_x + dense_player_id);
            __m256 accel_y = _mm256_load_ps(game_input->player_move_y + dense_player_id);
            const __m256 len = _mm256_sqrt_ps(_mm256_fmadd_ps(accel_x, accel_x, _mm256_mul_ps(accel_y, accel_y)));
            accel_x = _mm256_div_ps(accel_x, len);
            accel_y = _mm256_div_ps(accel_y, len);
            accel_x = _mm256_blendv_ps(
                    accel_x,
                    _mm256_set1_ps(0.0f),
                    _mm256_cmp_ps(len, _mm256_set1_ps(0.0f), _CMP_EQ_UQ)
                    );
            accel_y = _mm256_blendv_ps(
                    accel_y,
                    _mm256_set1_ps(0.0f),
                    _mm256_cmp_ps(len, _mm256_set1_ps(0.0f), _CMP_EQ_UQ)
                    );
            accel_x = _mm256_mul_ps(accel_x, max_accel);
            accel_y = _mm256_mul_ps(accel_y, max_accel);

            // Drag.
            accel_x = _mm256_fmadd_ps(prev_player_vel_x, player_drag, accel_x);
            accel_y = _mm256_fmadd_ps(prev_player_vel_y, player_drag, accel_y);

            // v' = a*t + v
            const __m256 player_vel_x = _mm256_fmadd_ps(accel_x, dt, prev_player_vel_x);
            const __m256 player_vel_y = _mm256_fmadd_ps(accel_y, dt, prev_player_vel_y);
            _mm256_store_ps(game_state->player_vel_x + dense_player_id, player_vel_x);
            _mm256_store_ps(game_state->player_vel_y + dense_player_id, player_vel_y);

            // p' = 0.5*a*t^2 + v*t + p
            const __m256 hdt2 = _mm256_mul_ps(_mm256_set1_ps(0.5f), _mm256_mul_ps(dt, dt));
            const __m256 player_pos_x = _mm256_fmadd_ps(accel_x, hdt2, _mm256_fmadd_ps(prev_player_vel_x, dt, prev_player_pos_x));
            const __m256 player_pos_y = _mm256_fmadd_ps(accel_y, hdt2, _mm256_fmadd_ps(prev_player_vel_y, dt, prev_player_pos_y));
            _mm256_store_ps(game_state->player_pos_x + dense_player_id, player_pos_x);
            _mm256_store_ps(game_state->player_pos_y + dense_player_id, player_pos_y);
        }
        game_state->num_players = num_players;
    }

    game_state->num_walls = LEVEL_0_NUM_WALLS;
    COPY(game_state->wall_pos_x, level_0_wall_pos_x, LEVEL_0_NUM_WALLS);
    COPY(game_state->wall_pos_y, level_0_wall_pos_y, LEVEL_0_NUM_WALLS);
    COPY(game_state->wall_width, level_0_wall_width, LEVEL_0_NUM_WALLS);
    COPY(game_state->wall_height, level_0_wall_height, LEVEL_0_NUM_WALLS);

}

