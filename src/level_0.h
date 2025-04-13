#pragma once

#include "common.h"

#define LEVEL_0_NUM_WALLS 5

const f32 level_0_wall_pos_x[] = {
    -6.0f,
     1.0f,
    -5.0f,
     4.0f,
     12.0f,
};
_Static_assert(LEVEL_0_NUM_WALLS == ARRAY_COUNT(level_0_wall_pos_x), "Mismatch array size.");

const f32 level_0_wall_pos_y[] = {
     3.0f,
     6.0f,
    -8.0f,
    -7.0f,
     5.0f,
};
_Static_assert(LEVEL_0_NUM_WALLS == ARRAY_COUNT(level_0_wall_pos_y), "Mismatch array size.");

const f32 level_0_wall_width[] = {
    1.0f,
    2.0f,
    2.0f,
    3.0f,
    1.0f,
};
_Static_assert(LEVEL_0_NUM_WALLS == ARRAY_COUNT(level_0_wall_width), "Mismatch array size.");

const f32 level_0_wall_height[] = {
    3.0f,
    2.0f,
    8.0f,
    7.0f,
    1.0f,
};
_Static_assert(LEVEL_0_NUM_WALLS == ARRAY_COUNT(level_0_wall_height), "Mismatch array size.");

