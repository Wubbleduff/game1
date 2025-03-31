
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "platform_win32/platform_win32_common.h"
#include "platform_win32/platform_win32_render.h"
#include "platform_win32/platform_win32_input.h"

void assert_fn(const char* file, int line, const u64 c, const char* msg)
{
    (void)file;
    (void)line;
    (void)msg;
    if(!c)
    {
        DebugBreak();
    }
}

s64 platform_win32_get_time_ns()
{
    const struct PlatformWin32Common* common = platform_win32_get_common();
    LARGE_INTEGER cy;
    QueryPerformanceCounter(&cy);
    return cy.QuadPart * 1000000000LL / common->clock_freq_hz;
}

static LRESULT CALLBACK wndproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;

    switch (uMsg)
    {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
        break;

        case WM_CLOSE: 
        {
            DestroyWindow(hwnd);
            return 0;
        }  
        break;

        case WM_PAINT:
        {
            ValidateRect(hwnd, 0);
        }
        break;

        case WM_KEYDOWN: 
        {
            const enum KeyboardKey k = platform_win32_vk_code_to_keyboard_key(wParam);
            platform_win32_set_keyboard_key(k);
        }
        break;

        case WM_KEYUP:
        {
            const enum KeyboardKey k = platform_win32_vk_code_to_keyboard_key(wParam);
            platform_win32_clear_keyboard_key(k);
        }
        break;

        case WM_LBUTTONDOWN:
        {
            // g_mouse_button[0] = true;
        }
        break;
        case WM_LBUTTONUP:
        {
            // g_mouse_button[0] = false;
        }
        break;

        case WM_RBUTTONDOWN:
        {
            // g_mouse_button[1] = true;
        }
        break;
        case WM_RBUTTONUP:
        {
            // g_mouse_button[1] = false;
        }
        break;

        default:
        {
            result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        break;
    }

    return result;
}

void platform_win32_init_common()
{
    struct PlatformWin32Common* common = platform_win32_get_common();

    const HMODULE hinstance = GetModuleHandle(0);
    ASSERT(hinstance, "GetModuleHandle failed.");

    const char window_class_name[] = "Window Class";
    WNDCLASS wc = {};
    wc.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    wc.lpfnWndProc   = wndproc;
    wc.hInstance     = hinstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = window_class_name;
    const ATOM register_class_result = RegisterClass(&wc);
    ASSERT(register_class_result, "RegisterClass failed.");

    const u32 monitor_width = GetSystemMetrics(SM_CXSCREEN);
    const u32 monitor_height = GetSystemMetrics(SM_CYSCREEN);
    const u32 window_width = 1400;
    const u32 window_height = (window_width * 3) / 4;
    const u32 window_x = monitor_width / 2 - window_width / 2;
    const u32 window_y = monitor_height / 2 - window_height / 2;

    common->hwnd = CreateWindowEx(
        0,
        window_class_name,
        "Game",
        WS_POPUP | WS_VISIBLE,
        window_x,
        window_y,
        window_width,
        window_height,
        NULL,
        NULL,
        hinstance,
        NULL);
    ASSERT(common->hwnd, "CreateWindowEx failed.");

    LARGE_INTEGER clock_freq;
    QueryPerformanceFrequency(&clock_freq);
    common->clock_freq_hz = clock_freq.QuadPart;

    platform_win32_init_render();
    platform_win32_init_input();

}


