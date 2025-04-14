
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "render.h"
#include "platform_win32/platform_win32_common.h"
#include "platform_win32/platform_win32_render.h"
#include "math.h"
#include "game_state.h"

void platform_win32_init_render()
{
    struct PlatformWin32Common* win32_common = platform_win32_get_common();
    struct PlatformWin32Render* render = platform_win32_get_render();
    
    const HWND hwnd = win32_common->hwnd;

    // Create DIB
    HDC hdc = GetDC(hwnd);
    ASSERT(hdc, "GetDC failed.");

    RECT client_rect;
    const BOOL get_client_rect_success = GetClientRect(hwnd, &client_rect);
    ASSERT(get_client_rect_success, "GetClientRect failed.");

    const u32 dib_row_byte_width = ((client_rect.right * 4 + 3) & -4);
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(bmi);
    bmi.bmiHeader.biWidth = client_rect.right;
    bmi.bmiHeader.biHeight = -client_rect.bottom;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = dib_row_byte_width * client_rect.bottom;
    bmi.bmiHeader.biXPelsPerMeter = 0;
    bmi.bmiHeader.biYPelsPerMeter = 0;
    render->dib_handle = CreateCompatibleDC(hdc);
    ASSERT(render->dib_handle, "CreateCompatibleDC failed.");

    HBITMAP dib_bitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (VOID **)&render->dib_frame_buffer, NULL, 0);
    ASSERT(dib_bitmap, "CreateDIBSection failed.");

    render->frame_buffer_width = client_rect.right;
    render->frame_buffer_height = client_rect.bottom;
    render->cur_frame_buffer = 0;

    const HGDIOBJ obj = SelectObject(render->dib_handle, dib_bitmap);
    ASSERT(obj, "SelectObject failed.");

    const int release_dc_success = ReleaseDC(hwnd, hdc);
    ASSERT(release_dc_success, "ReleaseDC failed.");

    render->camera.pos_x = 0.0f;
    render->camera.pos_y = 0.0f;
    render->camera.half_width = 15.0f;
    render->camera.aspect_ratio = (f32)client_rect.bottom / (f32)client_rect.right;
}

void platform_win32_render_game_state(struct GameState* game_state, const u32 dense_player_id)
{
    struct PlatformWin32Common* win32_common = platform_win32_get_common();
    struct PlatformWin32Render* render = platform_win32_get_render();

    const u64 fb_width = render->frame_buffer_width;
    const u64 fb_height = render->frame_buffer_height;
    ASSERT(fb_width < MAX_FRAME_BUFFER_WIDTH, "Frame buffer width overflow.");
    ASSERT(fb_height < MAX_FRAME_BUFFER_HEIGHT, "Frame buffer height overflow.");
    _Static_assert(MAX_FRAME_BUFFER_WIDTH % 8 == 0, "Frame buffer stride not 8 aligned.");
    struct FloatFrameBuffer* color_fb = &render->color_frame_buffer;
    struct FloatFrameBuffer* light_fb = &render->light_frame_buffer;
    struct FloatFrameBuffer* hdr_fb = &render->hdr_frame_buffer;

    {
        const f32 r = 0.006f;
        const f32 g = 0.04f;
        const f32 b = 0.0f;
        const __m256 color = _mm256_setr_ps(r, g, b, 1.0f, r, g, b, 1.0f);
        for(u64 y = 0; y < fb_height; y++)
        {
            for(u64 x = 0; x < fb_width; x += 2)
            {
                _mm256_store_ps(&color_fb->v[y][x][0], color);
            }
        }

        for(u64 y = 0; y < fb_height; y++)
        {
            for(u64 x = 0; x < fb_width; x += 2)
            {
                _mm256_store_ps(&light_fb->v[y][x][0], _mm256_set1_ps(0.1f));
            }
        }
    }

    for(u32 i = 0; i < game_state->num_players; i++)
    {
        render_world_rect(
            game_state->player_pos_x[i],
            game_state->player_pos_y[i],
            0.7f,
            1.0f * (i + 1),
            2.0f,
            1.2f,
            0.0f,
            1.0f);
    }

    for(u32 i = 0; i < game_state->num_walls; i++)
    {
        render_world_rect(
            game_state->wall_pos_x[i],
            game_state->wall_pos_y[i],
            game_state->wall_width[i],
            game_state->wall_height[i],
            0.6f,
            0.2f,
            0.01f,
            1.0f);
    }

    const f32 player_x = game_state->player_pos_x[dense_player_id];
    const f32 player_y = game_state->player_pos_y[dense_player_id];
    const f32 cam_x = render->camera.pos_x;
    const f32 cam_y = render->camera.pos_y;
    const f32 cam_hw = render->camera.half_width;
    const f32 cam_hh = cam_hw * render->camera.aspect_ratio;
    const f32 world_pixel_width = (2.0f*cam_hw) * (1.0f / (f32)fb_width);
    const f32 world_pixel_height = (2.0f*cam_hh) * (1.0f / (f32)fb_height);

    {
        _Static_assert(MAX_FRAME_BUFFER_WIDTH % 8 == 0, "Frame buffer stride not 8 aligned.");
        const __m256 base_p_x = _mm256_setr_ps(
            cam_x - cam_hw + world_pixel_width * 0.0f - player_x,
            cam_x - cam_hw + world_pixel_width * 0.0f - player_x,
            cam_x - cam_hw + world_pixel_width * 0.0f - player_x,
            cam_x - cam_hw + world_pixel_width * 0.0f - player_x,
            cam_x - cam_hw + world_pixel_width * 1.0f - player_x,
            cam_x - cam_hw + world_pixel_width * 1.0f - player_x,
            cam_x - cam_hw + world_pixel_width * 1.0f - player_x,
            cam_x - cam_hw + world_pixel_width * 1.0f - player_x
        );
        __m256 p_x = base_p_x;
        __m256 p_y = _mm256_set1_ps(cam_y + cam_hh - player_y);
        for(u64 y = 0; y < fb_height; y++)
        {
            for(u64 x = 0; x < fb_width; x += 2)
            {
                __m256 l = _mm256_sqrt_ps(_mm256_fmadd_ps(p_x, p_x, _mm256_mul_ps(p_y, p_y)));
                l = _mm256_fnmadd_ps(l, _mm256_set1_ps(2.0f), _mm256_set1_ps(20.0f));
                l = _mm256_min_ps(_mm256_max_ps(l, _mm256_set1_ps(0.0f)), _mm256_set1_ps(10.0f));

                __m256 result = _mm256_loadu_ps(light_fb->v[y][x]);
                result = _mm256_add_ps(result, l);
                _mm256_storeu_ps(light_fb->v[y][x], result);

                p_x = _mm256_add_ps(p_x, _mm256_set1_ps(2.0f * world_pixel_width));
            }

            p_x = base_p_x;
            p_y = _mm256_sub_ps(p_y, _mm256_set1_ps(world_pixel_height));
        }
    }

    {
        /*
         *
         * p0, p1, q0, q1
         *
         * p_n = {p.y, -p.x}
         * q_n = {q.y, -q.x}
         *
         * intersects = ((q0 - p0)*p_n < 0.0f) ^ ((q1 - p0)*p_n < 0.0f)  && 
         *              ((p0 - q0)*q_n < 0.0f) ^ ((p1 - q0)*q_n < 0.0f)
         *
         * Assuming p0 is origin.
         *
         * intersects = (q0*p_n < 0.0f) ^ (q1*p_n < 0.0f)  && 
         *              (-q0*q_n < 0.0f) ^ ((p1 - q0)*q_n < 0.0f)
         *
         * Only count intersections facing away from the wall.
         * intersects &= p1*qn > 0.0
         *
         */


        __m256 wall_x[MAX_WALLS][4];
        __m256 wall_y[MAX_WALLS][4];
        __m256 wall_nx[MAX_WALLS][4];
        __m256 wall_ny[MAX_WALLS][4];
        u32 num_walls = game_state->num_walls;
        for(u64 i = 0; i < game_state->num_walls; i++)
        {
            wall_x[i][0] = _mm256_set1_ps(game_state->wall_pos_x[i] - game_state->wall_width[i]  * 0.5f - player_x - world_pixel_width);
            wall_y[i][0] = _mm256_set1_ps(game_state->wall_pos_y[i] - game_state->wall_height[i] * 0.5f - player_y);
            wall_x[i][1] = _mm256_set1_ps(game_state->wall_pos_x[i] + game_state->wall_width[i]  * 0.5f - player_x);
            wall_y[i][1] = _mm256_set1_ps(game_state->wall_pos_y[i] - game_state->wall_height[i] * 0.5f - player_y);
            wall_x[i][2] = _mm256_set1_ps(game_state->wall_pos_x[i] + game_state->wall_width[i]  * 0.5f - player_x);
            wall_y[i][2] = _mm256_set1_ps(game_state->wall_pos_y[i] + game_state->wall_height[i] * 0.5f - player_y + world_pixel_height);
            wall_x[i][3] = _mm256_set1_ps(game_state->wall_pos_x[i] - game_state->wall_width[i]  * 0.5f - player_x - world_pixel_width);
            wall_y[i][3] = _mm256_set1_ps(game_state->wall_pos_y[i] + game_state->wall_height[i] * 0.5f - player_y + world_pixel_height);

            wall_nx[i][0] = _mm256_sub_ps(wall_y[i][0], wall_y[i][1]);
            wall_ny[i][0] = _mm256_sub_ps(wall_x[i][1], wall_x[i][0]);
            wall_nx[i][1] = _mm256_sub_ps(wall_y[i][1], wall_y[i][2]);
            wall_ny[i][1] = _mm256_sub_ps(wall_x[i][2], wall_x[i][1]);
            wall_nx[i][2] = _mm256_sub_ps(wall_y[i][2], wall_y[i][3]);
            wall_ny[i][2] = _mm256_sub_ps(wall_x[i][3], wall_x[i][2]);
            wall_nx[i][3] = _mm256_sub_ps(wall_y[i][3], wall_y[i][0]);
            wall_ny[i][3] = _mm256_sub_ps(wall_x[i][0], wall_x[i][3]);
        }

        const __m256 base_p1_x = _mm256_setr_ps(
            cam_x - cam_hw + world_pixel_width * 0.0f - player_x,
            cam_x - cam_hw + world_pixel_width * 1.0f - player_x,
            cam_x - cam_hw + world_pixel_width * 2.0f - player_x,
            cam_x - cam_hw + world_pixel_width * 3.0f - player_x,
            cam_x - cam_hw + world_pixel_width * 4.0f - player_x,
            cam_x - cam_hw + world_pixel_width * 5.0f - player_x,
            cam_x - cam_hw + world_pixel_width * 6.0f - player_x,
            cam_x - cam_hw + world_pixel_width * 7.0f - player_x
        );
        __m256 p1_x = base_p1_x;
        __m256 p1_y = _mm256_set1_ps(cam_y + cam_hh - player_y);
        _Static_assert(MAX_FRAME_BUFFER_WIDTH % 8 == 0, "Frame buffer stride not 8 aligned.");
        for(u64 i_y = 0; i_y < fb_height; i_y++)
        {
            for(u64 i_x = 0; i_x < fb_width; i_x += 8)
            {
                const __m256 p_nx = p1_y;
                const __m256 p_ny = _mm256_mul_ps(p1_x, _mm256_set1_ps(-1.0f));

                __m256 occluded = _mm256_set1_ps(0.0f);

                for(u64 i_wall = 0; i_wall < num_walls; i_wall++)
                {
                    for(u64 i_edge = 0; i_edge < 4; i_edge++)
                    {
                        const __m256 q0_x = wall_x[i_wall][i_edge];
                        const __m256 q0_y = wall_y[i_wall][i_edge];
                        const __m256 q1_x = wall_x[i_wall][(i_edge+1) & 3];
                        const __m256 q1_y = wall_y[i_wall][(i_edge+1) & 3];
                        const __m256 q_nx = wall_nx[i_wall][i_edge];
                        const __m256 q_ny = wall_ny[i_wall][i_edge];

                        const __m256 d0 = _mm256_fmadd_ps(q0_x, p_nx, _mm256_mul_ps(q0_y, p_ny));
                        const __m256 d1 = _mm256_fmadd_ps(q1_x, p_nx, _mm256_mul_ps(q1_y, p_ny));
                        const __m256 d2 = _mm256_fmadd_ps(_mm256_mul_ps(q0_x, _mm256_set1_ps(-1.0f)), q_nx, _mm256_mul_ps(_mm256_mul_ps(q0_y, _mm256_set1_ps(-1.0f)), q_ny));
                        const __m256 d3 = _mm256_fmadd_ps(_mm256_sub_ps(p1_x, q0_x), q_nx, _mm256_mul_ps(_mm256_sub_ps(p1_y, q0_y), q_ny));

                        const __m256 d4 = _mm256_fmadd_ps(p1_x, q_nx, _mm256_mul_ps(p1_y, q_ny));

                        const __m256 intersect = _mm256_and_ps(
                                      _mm256_xor_ps(d0, d1),
                                      _mm256_xor_ps(d2, d3));
                        const __m256 mask = _mm256_and_ps(intersect, d4);

                        occluded = _mm256_or_ps(occluded, mask);
                    }
                }

                // Expand from an 8 element ymm to 4 2 element ymm's.
                {
                    __m256 acegbdfh = _mm256_permutevar8x32_ps(occluded, _mm256_setr_epi32(0, 2, 4, 6, 1, 3, 5, 7));
                    __m256 aaaabbbb = _mm256_shuffle_ps(acegbdfh, acegbdfh, 0x00);
                    __m256 ccccdddd = _mm256_shuffle_ps(acegbdfh, acegbdfh, 0x55);
                    __m256 eeeeffff = _mm256_shuffle_ps(acegbdfh, acegbdfh, 0xAA);
                    __m256 gggghhhh = _mm256_shuffle_ps(acegbdfh, acegbdfh, 0xFF);

                    __m256 e01 = _mm256_loadu_ps(light_fb->v[i_y][i_x + 0]);
                    _mm256_storeu_ps(
                        light_fb->v[i_y][i_x + 0],
                        _mm256_blendv_ps(e01, _mm256_set1_ps(0.0f), aaaabbbb)
                    );
                    __m256 e23 = _mm256_loadu_ps(light_fb->v[i_y][i_x + 2]);
                    _mm256_storeu_ps(
                        light_fb->v[i_y][i_x + 2],
                        _mm256_blendv_ps(e23, _mm256_set1_ps(0.0f), ccccdddd)
                    );
                    __m256 e45 = _mm256_loadu_ps(light_fb->v[i_y][i_x + 4]);
                    _mm256_storeu_ps(
                        light_fb->v[i_y][i_x + 4],
                        _mm256_blendv_ps(e45, _mm256_set1_ps(0.0f), eeeeffff)
                    );
                    __m256 e67 = _mm256_loadu_ps(light_fb->v[i_y][i_x + 6]);
                    _mm256_storeu_ps(
                        light_fb->v[i_y][i_x + 6],
                        _mm256_blendv_ps(e67, _mm256_set1_ps(0.0f), gggghhhh)
                    );
                }

                p1_x = _mm256_add_ps(p1_x, _mm256_set1_ps(8.0f * world_pixel_width));
            }

            p1_x = base_p1_x;
            p1_y = _mm256_sub_ps(p1_y, _mm256_set1_ps(world_pixel_height));
        }
    }

    _Static_assert(MAX_FRAME_BUFFER_WIDTH % 8 == 0, "Frame buffer stride not 8 aligned.");
    for(u64 y = 0; y < fb_height; y++)
    {
        for(u64 x = 0; x < fb_width; x += 2)
        {
            const __m256 c = _mm256_loadu_ps(color_fb->v[y][x]);
            const __m256 l = _mm256_loadu_ps(light_fb->v[y][x]);
            _mm256_storeu_ps(
                hdr_fb->v[y][x],
                _mm256_mul_ps(c, l)
            );
        }
    }

    for(u64 y = 0; y < fb_height; y++)
    {
        for(u64 x = 0; x < fb_width; x++)
        {
            for(u64 c = 0; c < 3; c++)
            {
                const f32 v = hdr_fb->v[y][x][c];
                hdr_fb->v[y][x][c] = v / (v + 1.0f);
            }
        }
    }

    struct IntFrameBuffer* blit_fb = &render->blit_frame_buffers[render->cur_frame_buffer];
    for(u64 y = 0; y < fb_height; y++)
    {
        for(u64 x = 0; x < fb_width; x++)
        {
            const u32 r = (u32)(hdr_fb->v[y][x][0] * 255.0f);
            const u32 g = (u32)(hdr_fb->v[y][x][1] * 255.0f);
            const u32 b = (u32)(hdr_fb->v[y][x][2] * 255.0f);
            const u32 a = (u32)(hdr_fb->v[y][x][3] * 255.0f);
            blit_fb->v[y][x] = a << 24U | r << 16U | g << 8U | b;
        }
    }


    // Copy frame buffer to DIB. Target DIB is not guaranteed 8 element aligned.
    for(u64 y = 0; y < fb_height; y++)
    {
        for(u64 x = 0; x < fb_width - 8; x += 8)
        {
            _mm256_storeu_si256(
                    (__m256i*)(render->dib_frame_buffer + y*fb_width + x),
                    _mm256_loadu_si256((const __m256i*)(&blit_fb->v[y][x]))
                    );
        }
        _mm256_storeu_si256(
                (__m256i*)(render->dib_frame_buffer + y*fb_width + fb_width-8),
                _mm256_loadu_si256((const __m256i*)(&blit_fb->v[y][fb_width-8]))
                );
    }

    const HWND hwnd = win32_common->hwnd;
    HDC hdc = GetDC(hwnd);
    ASSERT(hdc, "GetDC failed.");

    const BOOL bit_blit_success = BitBlt(hdc, 0, 0, render->frame_buffer_width, render->frame_buffer_height, render->dib_handle, 0, 0, SRCCOPY);
    ASSERT(bit_blit_success, "BitBlt failed.");

    ReleaseDC(hwnd, hdc);
    const int release_dc_success = ReleaseDC(hwnd, hdc);
    ASSERT(release_dc_success, "ReleaseDC failed.");

    render->cur_frame_buffer = (render->cur_frame_buffer + 1) & 1;
}

void render_world_rect(
        const f32 pos_x,
        const f32 pos_y,
        const f32 w,
        const f32 h,
        const f32 r,
        const f32 g,
        const f32 b,
        const f32 a)
{
    struct PlatformWin32Render* render = platform_win32_get_render();
    struct FloatFrameBuffer* color_fb = &render->color_frame_buffer;

    const u32 fb_width = render->frame_buffer_width;
    const u32 fb_height = render->frame_buffer_height;

    const f32 cam_hw = render->camera.half_width;
    const f32 cam_hh = cam_hw * render->camera.aspect_ratio;

    const f32 cam_l = render->camera.pos_x - cam_hw;
    const f32 cam_b = render->camera.pos_y - cam_hh;

    const f32 fb_pos_x = ((pos_x - cam_l) / (cam_hw * 2.0f)) * fb_width;
    const f32 fb_pos_y = (1.0f - ((pos_y - cam_b) / (cam_hh * 2.0f))) * fb_height;
    const f32 fb_w = (w / (cam_hw * 2.0f)) * fb_width;
    const f32 fb_h = (h / (cam_hh * 2.0f)) * fb_height;

    const __m256 color = _mm256_setr_ps(r, g, b, a, r, g, b, a);

    const s32 left = (s32)round_neg_inf(max_f32(fb_pos_x - fb_w * 0.5f, 0.0f));
    const s32 right = (s32)round_neg_inf(min_f32(fb_pos_x + fb_w * 0.5f, (f32)fb_width));
    const s32 bottom = (s32)round_neg_inf(max_f32(fb_pos_y - fb_h * 0.5f, 0.0f));
    const s32 top = (s32)round_neg_inf(min_f32(fb_pos_y + fb_h * 0.5f, (f32)fb_height));
    _Static_assert(MAX_FRAME_BUFFER_WIDTH % 8 == 0, "Frame buffer stride not 8 aligned.");
    for(s32 y = bottom; y < top; y++)
    {
        for(s32 x = left; x < right; x += 2)
        {
            _mm256_storeu_ps(&color_fb->v[y][x][0], color);
        }
    }
}

