
#include "common.h"

#include "math.h"
#include "game_input.h"
#include "game_state.h"

#include "level_0.h"

void init_game_state(struct GameState* game_state)
{
    game_state->num_players = 0;
    game_state->num_bullets = 0;
    game_state->num_walls = 0;

    // New players will default to dense_id = MAX_ACTIVE_PLAYERS.
    game_state->player_pos_x[MAX_ACTIVE_PLAYERS] = 0.0f;
    game_state->player_pos_y[MAX_ACTIVE_PLAYERS] = 0.0f;
    game_state->player_vel_x[MAX_ACTIVE_PLAYERS] = 0.0f;
    game_state->player_vel_y[MAX_ACTIVE_PLAYERS] = 0.0f;
    game_state->player_radius[MAX_ACTIVE_PLAYERS] = 0.5f;

    // Init static game state.
    {
        game_state->num_walls = LEVEL_0_NUM_WALLS;
        COPY(game_state->wall_pos_x, level_0_wall_pos_x, LEVEL_0_NUM_WALLS);
        COPY(game_state->wall_pos_y, level_0_wall_pos_y, LEVEL_0_NUM_WALLS);
        COPY(game_state->wall_width, level_0_wall_width, LEVEL_0_NUM_WALLS);
        COPY(game_state->wall_height, level_0_wall_height, LEVEL_0_NUM_WALLS);
    }
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

static void add_bullet(struct GameState* game_state, const f32 pos_x, const f32 pos_y, const f32 vel_x, const f32 vel_y)
{
    ASSERT(game_state->num_bullets < MAX_BULLETS, "'add_bullet' overflow %u / %u", game_state->num_bullets, MAX_BULLETS);
    const u32 n = game_state->num_bullets;
    game_state->bullet_pos_x[n] = pos_x;
    game_state->bullet_pos_y[n] = pos_y;
    game_state->bullet_vel_x[n] = vel_x;
    game_state->bullet_vel_y[n] = vel_y;
    game_state->bullet_is_alive[n] = 1;
    game_state->bullet_time_to_live[n] = 30;
    game_state->num_bullets++;
}

static void remove_bullet(struct GameState* game_state, const u32 i_bullet)
{
    ASSERT(i_bullet < game_state->num_bullets, "'remove_bullet' overflow %u / %u", i_bullet, game_state->num_bullets);
    const u32 n = game_state->num_bullets;
    game_state->bullet_pos_x[i_bullet] = game_state->bullet_pos_x[n - 1];
    game_state->bullet_pos_y[i_bullet] = game_state->bullet_pos_y[n - 1];
    game_state->bullet_vel_x[i_bullet] = game_state->bullet_vel_x[n - 1];
    game_state->bullet_vel_y[i_bullet] = game_state->bullet_vel_y[n - 1];
    game_state->bullet_is_alive[i_bullet] = game_state->bullet_is_alive[n - 1];
    game_state->bullet_time_to_live[i_bullet] = game_state->bullet_time_to_live[n - 1];
    game_state->num_bullets--;
}

void update_game_state(struct GameState* game_state, const struct GameState* prev_game_state, const struct GameInput* game_input)
{
    // Copy bullets.
    {
        game_state->num_bullets = prev_game_state->num_bullets;
        COPY(game_state->bullet_pos_x, prev_game_state->bullet_pos_x, game_state->num_bullets);
        COPY(game_state->bullet_pos_y, prev_game_state->bullet_pos_y, game_state->num_bullets);
        COPY(game_state->bullet_vel_x, prev_game_state->bullet_vel_x, game_state->num_bullets);
        COPY(game_state->bullet_vel_y, prev_game_state->bullet_vel_y, game_state->num_bullets);
        COPY(game_state->bullet_is_alive, prev_game_state->bullet_is_alive, game_state->num_bullets);
        COPY(game_state->bullet_time_to_live, prev_game_state->bullet_time_to_live, game_state->num_bullets);
    }
    
    // Copy players and do sparse to dense mapping.
    const u32 num_players = game_input->num_players;
    {
        u32 prev_game_state_dense_player_id[MAX_ACTIVE_PLAYERS] = {};

        // Map current dense player IDs to prev game state dense player IDs.
        for(u32 dense_player_id = 0; dense_player_id < num_players; dense_player_id++)
        {
            const u32 sparse_player_id = game_input->sparse_player_id[dense_player_id];

            prev_game_state_dense_player_id[dense_player_id] = sparse_to_dense_player_id(prev_game_state, sparse_player_id);
        }

        // Reuse the next game state's buffers to update physics in smaller increments.
        // Copy prev game state buffers into next game state buffers to start while also doing a sparse to dense mapping.
        for(u32 dense_player_id = 0; dense_player_id < num_players; dense_player_id++)
        {
            game_state->sparse_player_id[dense_player_id] = game_input->sparse_player_id[dense_player_id];
            const u32 prev_dense_player_id = prev_game_state_dense_player_id[dense_player_id];
            game_state->player_vel_x[dense_player_id] = prev_game_state->player_vel_x[prev_dense_player_id];
            game_state->player_vel_y[dense_player_id] = prev_game_state->player_vel_y[prev_dense_player_id];
            game_state->player_pos_x[dense_player_id] = prev_game_state->player_pos_x[prev_dense_player_id];
            game_state->player_pos_y[dense_player_id] = prev_game_state->player_pos_y[prev_dense_player_id];
            game_state->player_radius[dense_player_id] = prev_game_state->player_radius[prev_dense_player_id];
        }
        game_state->num_players = num_players;
    }

    // Check if player shot.
    for(u32 dense_player_id = 0; dense_player_id < num_players; dense_player_id++)
    {
        if(game_input->player_shoot[dense_player_id])
        {
            const f32 player_radius = game_state->player_radius[dense_player_id];
            add_bullet(
                game_state,
                game_state->player_pos_x[dense_player_id] + game_input->player_shoot_dir_x[dense_player_id] * (player_radius + 0.1f),
                game_state->player_pos_y[dense_player_id] + game_input->player_shoot_dir_y[dense_player_id] * (player_radius + 0.1f),
                game_input->player_shoot_dir_x[dense_player_id] * 50.0f,
                game_input->player_shoot_dir_y[dense_player_id] * 50.0f
            );
        }
    }

    // Update physics.
    {
        // Iteratively update physics using the new buffers.
        const u32 num_iterations = 16;
        const f32 sub_dt = FRAME_DURATION_NS * (1.0f / 1000000000.0f) * (1.0f / (f32)num_iterations);
        for(u32 iteration = 0; iteration < num_iterations; iteration++)
        {
            // Note: The order with which we process kinematics and collisions is important.

            // 1. Integrate forces into player velocity.
            for(u32 dense_player_id = 0; dense_player_id < num_players; dense_player_id++)
            {
                const f32 max_accel = 30.0f;
                const f32 drag = -8.0f;

                v2 vel = make_v2(game_state->player_vel_x[dense_player_id], game_state->player_vel_y[dense_player_id]);

                v2 accel = make_v2(game_input->player_move_x[dense_player_id], game_input->player_move_y[dense_player_id]);
                accel = v2_normalize_or_zero(accel);
                accel = v2_scale(accel, max_accel);

                // Drag.
                // a' = a + v * d;
                accel = v2_add(accel, v2_scale(vel, drag));

                // v' = a*t + v
                vel = v2_add(vel, v2_scale(accel, sub_dt));

                game_state->player_vel_x[dense_player_id] = vel.x;
                game_state->player_vel_y[dense_player_id] = vel.y;
            }

            // 2. Resolve bullet-wall collisions.
            for(u32 i_bullet = 0; i_bullet < game_state->num_bullets; i_bullet++)
            {
                const v2 bullet_pos = make_v2(game_state->bullet_pos_x[i_bullet], game_state->bullet_pos_y[i_bullet]);
                for(u32 i_wall = 0; i_wall < prev_game_state->num_walls; i_wall++)
                {
                    const f32 wall_left = prev_game_state->wall_pos_x[i_wall] - prev_game_state->wall_width[i_wall] * 0.5f;
                    const f32 wall_right = prev_game_state->wall_pos_x[i_wall] + prev_game_state->wall_width[i_wall] * 0.5f;
                    const f32 wall_bottom = prev_game_state->wall_pos_y[i_wall] - prev_game_state->wall_height[i_wall] * 0.5f;
                    const f32 wall_top = prev_game_state->wall_pos_y[i_wall] + prev_game_state->wall_height[i_wall] * 0.5f;

                    if(bullet_pos.x > wall_left && bullet_pos.x < wall_right &&
                       bullet_pos.y > wall_bottom && bullet_pos.y < wall_top)
                    {
                        game_state->bullet_is_alive[i_bullet] = 0;
                    }
                }
            }

            // 3. Resolve player-bullet collisions.
            for(u32 dense_player_id = 0; dense_player_id < num_players; dense_player_id++)
            {
                const v2 pos = make_v2(game_state->player_pos_x[dense_player_id], game_state->player_pos_y[dense_player_id]);
                const f32 player_radius = game_state->player_radius[dense_player_id];
                v2 vel = make_v2(game_state->player_vel_x[dense_player_id], game_state->player_vel_y[dense_player_id]);

                for(u32 i_bullet = 0; i_bullet < game_state->num_bullets; i_bullet++)
                {
                    const v2 bullet_pos = make_v2(game_state->bullet_pos_x[i_bullet], game_state->bullet_pos_y[i_bullet]);
                    const v2 bullet_vel = make_v2(game_state->bullet_vel_x[i_bullet], game_state->bullet_vel_y[i_bullet]);

                    const v2 n = v2_sub(pos, bullet_pos);
                    if(v2_dot(n, n) < square_f32(player_radius) && game_state->bullet_is_alive[i_bullet])
                    {
                        vel = v2_add(vel, v2_scale(bullet_vel, 0.4f));

                        game_state->bullet_is_alive[i_bullet] = 0;
                    }

                    game_state->player_vel_x[dense_player_id] = vel.x;
                    game_state->player_vel_y[dense_player_id] = vel.y;
                }
            }

            // 4. Resolve player-player collisions.
            for(u32 a_id = 0; a_id < num_players; a_id++)
            {
                const v2 a_pos = make_v2(game_state->player_pos_x[a_id], game_state->player_pos_y[a_id]);
                v2 a_vel = make_v2(game_state->player_vel_x[a_id], game_state->player_vel_y[a_id]);
                const f32 a_radius = game_state->player_radius[a_id];

                // Note: This technically makes us dependent on the order of updated players. We could fix this by introducing an intermediate buffer for
                //       position and velocity.
                for(u32 b_id = a_id + 1; b_id < num_players; b_id++)
                {
                    const v2 b_pos = make_v2(game_state->player_pos_x[b_id], game_state->player_pos_y[b_id]);
                    v2 b_vel = make_v2(game_state->player_vel_x[b_id], game_state->player_vel_y[b_id]);
                    const f32 b_radius = game_state->player_radius[b_id];

                    const v2 n = v2_sub(a_pos, b_pos);
                    const v2 rel_vel = v2_sub(a_vel, b_vel);
                    if(v2_dot(n, n) < square_f32(a_radius + b_radius) &&
                       v2_dot(rel_vel, n) < 0.0f)
                    {
                        const f32 j = v2_dot(v2_scale(rel_vel, -1.0f), n) / (v2_dot(n, n) * 2.0f);

                        a_vel = v2_add(a_vel, v2_scale(n, j));
                        b_vel = v2_add(b_vel, v2_scale(n, -j));

                        // Only need to write b_vel out because a_val is cached for this player and will be written at the very end.
                        game_state->player_vel_x[b_id] = b_vel.x;
                        game_state->player_vel_y[b_id] = b_vel.y;
                    }
                }

                game_state->player_vel_x[a_id] = a_vel.x;
                game_state->player_vel_y[a_id] = a_vel.y;
            }

            // 5. Resolve player-wall collisions.
            //    Do this after all player-player collisions so it's harder for players to move into walls.
            for(u32 dense_player_id = 0; dense_player_id < num_players; dense_player_id++)
            {
                const v2 pos = make_v2(game_state->player_pos_x[dense_player_id], game_state->player_pos_y[dense_player_id]);
                const f32 player_radius = game_state->player_radius[dense_player_id];
                v2 vel = make_v2(game_state->player_vel_x[dense_player_id], game_state->player_vel_y[dense_player_id]);

                for(u32 i_wall = 0; i_wall < prev_game_state->num_walls; i_wall++)
                {
                    const f32 wall_left = prev_game_state->wall_pos_x[i_wall] - prev_game_state->wall_width[i_wall] * 0.5f;
                    const f32 wall_right = prev_game_state->wall_pos_x[i_wall] + prev_game_state->wall_width[i_wall] * 0.5f;
                    const f32 wall_bottom = prev_game_state->wall_pos_y[i_wall] - prev_game_state->wall_height[i_wall] * 0.5f;
                    const f32 wall_top = prev_game_state->wall_pos_y[i_wall] + prev_game_state->wall_height[i_wall] * 0.5f;

                    v2 clamped_pos = pos;
                    clamped_pos.x = clamp_f32(pos.x, wall_left, wall_right);
                    clamped_pos.y = clamp_f32(pos.y, wall_bottom, wall_top);

                    const v2 n = v2_sub(pos, clamped_pos);
                    if(v2_dot(n, n) < square_f32(player_radius) &&
                       v2_dot(n, vel) < 0.0f)
                    {
                        const f32 j = v2_dot(v2_scale(vel, -1.0f), n) / v2_dot(n, n);
                        vel = v2_add(vel, v2_scale(n, j));
                    }
                }

                game_state->player_vel_x[dense_player_id] = vel.x;
                game_state->player_vel_y[dense_player_id] = vel.y;
            }

            // 6. Integrate velocity into position.
            for(u32 dense_player_id = 0; dense_player_id < num_players; dense_player_id++)
            {
                const v2 pos = make_v2(game_state->player_pos_x[dense_player_id], game_state->player_pos_y[dense_player_id]);
                const v2 vel = make_v2(game_state->player_vel_x[dense_player_id], game_state->player_vel_y[dense_player_id]);
                const v2 next_pos = v2_add(pos, v2_scale(vel, sub_dt));
                game_state->player_pos_x[dense_player_id] = next_pos.x;
                game_state->player_pos_y[dense_player_id] = next_pos.y;
            }
            for(u32 i_bullet = 0; i_bullet < game_state->num_bullets; i_bullet++)
            {
                const v2 pos = make_v2(game_state->bullet_pos_x[i_bullet], game_state->bullet_pos_y[i_bullet]);
                const v2 vel = make_v2(game_state->bullet_vel_x[i_bullet], game_state->bullet_vel_y[i_bullet]);
                const v2 next_pos = v2_add(pos, v2_scale(vel, sub_dt));
                game_state->bullet_pos_x[i_bullet] = next_pos.x;
                game_state->bullet_pos_y[i_bullet] = next_pos.y;
            }
        }
    }

    for(u32 i_bullet = 0; i_bullet < game_state->num_bullets;)
    {
        game_state->bullet_time_to_live[i_bullet]--;
        if(game_state->bullet_time_to_live[i_bullet] <= 0 || !game_state->bullet_is_alive[i_bullet])
        {
            remove_bullet(game_state, i_bullet);
        }
        else
        {
            i_bullet++;
        }
    }

}

