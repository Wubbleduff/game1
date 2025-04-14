#pragma once

#include "common.h"

struct Camera
{
    f32 pos_x;
    f32 pos_y;
    f32 half_width;
    // h / w
    f32 aspect_ratio;
};

#define MAX_FRAME_BUFFER_WIDTH 2560
#define MAX_FRAME_BUFFER_HEIGHT 1440
struct FloatFrameBuffer
{
    f32 v[MAX_FRAME_BUFFER_HEIGHT][MAX_FRAME_BUFFER_WIDTH][4];
};

struct IntFrameBuffer
{
    u32 v[MAX_FRAME_BUFFER_HEIGHT][MAX_FRAME_BUFFER_WIDTH];
};

struct PlatformWin32Render
{
    _Static_assert(MAX_FRAME_BUFFER_WIDTH % 8 == 0, "Frame buffer storage width must be a multiple of 8 to allow unconditional 8 element access.");
    _Alignas(32) struct FloatFrameBuffer color_frame_buffer;
    _Alignas(32) struct FloatFrameBuffer light_frame_buffer;
    _Alignas(32) struct FloatFrameBuffer hdr_frame_buffer;
    _Alignas(32) struct IntFrameBuffer blit_frame_buffers[2];
    u32 frame_buffer_width;
    u32 frame_buffer_height;
    u32 cur_frame_buffer;

    HDC dib_handle;
    u32* dib_frame_buffer;

    struct Camera camera;
};

struct PlatformWin32Render* platform_win32_get_render();

void platform_win32_init_render();

u32* platform_win32_get_main_frame_buffer(
        u32* out_frame_buffer_width,
        u32* out_frame_buffer_height);

struct Camera* platform_win32_get_main_camera();

struct GameState;
void platform_win32_render_game_state(struct GameState* game_state, const u32 dense_player_id);


