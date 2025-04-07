
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

    const HGDIOBJ obj = SelectObject(render->dib_handle, dib_bitmap);
    ASSERT(obj, "SelectObject failed.");

    const int release_dc_success = ReleaseDC(hwnd, hdc);
    ASSERT(release_dc_success, "ReleaseDC failed.");

    render->camera.pos_x = 0.0f;
    render->camera.pos_y = 0.0f;
    render->camera.half_width = 15.0f;
    render->camera.aspect_ratio = (f32)client_rect.bottom / (f32)client_rect.right;
}

void platform_win32_render_game_state(struct GameState* game_state)
{
    struct PlatformWin32Common* win32_common = platform_win32_get_common();
    struct PlatformWin32Render* render = platform_win32_get_render();

    {
        const u32 color = 0xFF191E18;
        const __m256i color8 = _mm256_set1_epi32(color);
        ASSERT(render->frame_buffer_width * render->frame_buffer_height >= 8, "Frame buffer too small.");
        for(u64 i = 0; i < render->frame_buffer_width * render->frame_buffer_height - 8; i += 8)
        {
            _mm256_store_si256(
                    (__m256i*)(render->frame_buffer + i),
                    color8);
        }
        for(u64 i = render->frame_buffer_width * render->frame_buffer_height - 8; i < render->frame_buffer_width * render->frame_buffer_height; i++)
        {
            render->frame_buffer[i] = color;
        }
    }

    for(u32 i = 0; i < game_state->num_players; i++)
    {
        render_world_rect(
            game_state->player_pos_x[i],
            game_state->player_pos_y[i],
            0.7f,
            1.0f * (i + 1),
            0xFFC4A033
        );
    }

    for(u32 i = 0; i < game_state->num_walls; i++)
    {
        render_world_rect(
            game_state->wall_pos_x[i],
            game_state->wall_pos_y[i],
            game_state->wall_width[i],
            game_state->wall_height[i],
            0x7C4B0E
        );
    }

    memcpy(render->dib_frame_buffer, render->frame_buffer, render->frame_buffer_width * render->frame_buffer_height * sizeof(render->frame_buffer[0]));

    const HWND hwnd = win32_common->hwnd;
    HDC hdc = GetDC(hwnd);
    ASSERT(hdc, "GetDC failed.");

    const BOOL bit_blit_success = BitBlt(hdc, 0, 0, render->frame_buffer_width, render->frame_buffer_height, render->dib_handle, 0, 0, SRCCOPY);
    ASSERT(bit_blit_success, "BitBlt failed.");

    ReleaseDC(hwnd, hdc);
    const int release_dc_success = ReleaseDC(hwnd, hdc);
    ASSERT(release_dc_success, "ReleaseDC failed.");
}

static void render_rect_to_frame_buffer(
        u32* frame_buffer,
        const u32 frame_buffer_width,
        const u32 frame_buffer_height,
        const f32 cam_pos_x,
        const f32 cam_pos_y,
        const f32 cam_hw,
        const f32 cam_aspect_ratio,
        const f32 pos_x,
        const f32 pos_y,
        const f32 w,
        const f32 h,
        const u32 color)
{
    const f32 cam_hh = cam_hw * cam_aspect_ratio;

    const f32 cam_l = cam_pos_x - cam_hw;
    const f32 cam_b = cam_pos_y - cam_hh;

    const f32 fb_pos_x = ((pos_x - cam_l) / (cam_hw * 2.0f)) * frame_buffer_width;
    const f32 fb_pos_y = (1.0f - ((pos_y - cam_b) / (cam_hh * 2.0f))) * frame_buffer_height;
    const f32 fb_w = (w / (cam_hw * 2.0f)) * frame_buffer_width;
    const f32 fb_h = (h / (cam_hh * 2.0f)) * frame_buffer_height;

    const __m256i color8 = _mm256_set1_epi32(color);

    const s32 l = (s32)round_neg_inf(max_f32(fb_pos_x - fb_w * 0.5f, 0.0f));
    const s32 r = (s32)round_neg_inf(min_f32(fb_pos_x + fb_w * 0.5f, (f32)frame_buffer_width));
    const s32 b = (s32)round_neg_inf(max_f32(fb_pos_y - fb_h * 0.5f, 0.0f));
    const s32 t = (s32)round_neg_inf(min_f32(fb_pos_y + fb_h * 0.5f, (f32)frame_buffer_height));
    for(s32 y = b; y < t; y++)
    {
        for(s32 x = l; x < r - 8; x += 8)
        {
            _mm256_storeu_si256(
                    (__m256i*)(frame_buffer + y * frame_buffer_width + x),
                    color8);
        }
        for(s32 x = max_s32(r - 8, l); x < r; x++)
        {
            frame_buffer[y * frame_buffer_width + x] = color;
        }
    }
}

void render_world_rect(
        const f32 pos_x,
        const f32 pos_y,
        const f32 w,
        const f32 h,
        const u32 color)
{
    struct PlatformWin32Render* render = platform_win32_get_render();
    render_rect_to_frame_buffer(
            render->frame_buffer,
            render->frame_buffer_width,
            render->frame_buffer_height,
            render->camera.pos_x,
            render->camera.pos_y,
            render->camera.half_width,
            render->camera.aspect_ratio,
            pos_x,
            pos_y,
            w,
            h,
            color);
}

