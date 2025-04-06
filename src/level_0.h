#pragma once

#include "common.h"

#define LEVEL_0_NUM_WALLS 4

const f32 level_0_wall_pos_x[] = {
    -12.0f,
     12.0f,
    -12.0f,
     12.0f,
};
_Static_assert(LEVEL_0_NUM_WALLS == ARRAY_COUNT(level_0_wall_pos_x), "Mismatch array size.");

const f32 level_0_wall_pos_y[] = {
     12.0f,
     12.0f,
    -12.0f,
    -12.0f,
};
_Static_assert(LEVEL_0_NUM_WALLS == ARRAY_COUNT(level_0_wall_pos_y), "Mismatch array size.");

const f32 level_0_wall_width[] = {
    16.0f,
    16.0f,
    16.0f,
    16.0f,
};
_Static_assert(LEVEL_0_NUM_WALLS == ARRAY_COUNT(level_0_wall_width), "Mismatch array size.");

const f32 level_0_wall_height[] = {
    16.0f,
    16.0f,
    16.0f,
    16.0f,
};
_Static_assert(LEVEL_0_NUM_WALLS == ARRAY_COUNT(level_0_wall_height), "Mismatch array size.");

