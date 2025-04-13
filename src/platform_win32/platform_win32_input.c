
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "platform_win32/platform_win32_input.h"
#include "platform_win32/platform_win32_common.h"
#include "game_input.h"

// Maps Win32 VK code to KeyboardKey enum.
const u32 win32_vk_code_to_keyboard_key[] = {
    // https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
    /* -              0x00 */  KB_NOT_SUPPORTED,
    /* VK_LBUTTON     0x01 */  KB_NOT_SUPPORTED,
    /* VK_RBUTTON     0x02 */  KB_NOT_SUPPORTED,
    /* VK_CANCEL      0x03 */  KB_NOT_SUPPORTED,
    /* VK_MBUTTON     0x04 */  KB_NOT_SUPPORTED,
    /* VK_XBUTTON1    0x05 */  KB_NOT_SUPPORTED,
    /* VK_XBUTTON2    0x06 */  KB_NOT_SUPPORTED,
    /* -              0x07 */  KB_NOT_SUPPORTED,
    /* VK_BACK        0x08 */  KB_NOT_SUPPORTED,
    /* VK_TAB         0x09 */  KB_NOT_SUPPORTED,
    /* -              0x0A */  KB_NOT_SUPPORTED,
    /* -              0x0B */  KB_NOT_SUPPORTED,
    /* VK_CLEAR       0x0C */  KB_NOT_SUPPORTED,
    /* VK_RETURN      0x0D */  KB_NOT_SUPPORTED,
    /* -              0x0E */  KB_NOT_SUPPORTED,
    /* -              0x0F */  KB_NOT_SUPPORTED,

    /* VK_SHIFT       0x10 */  KB_NOT_SUPPORTED,
    /* VK_CONTROL     0x11 */  KB_LCTRL,
    /* VK_MENU        0x12 */  KB_NOT_SUPPORTED,
    /* VK_PAUSE       0x13 */  KB_NOT_SUPPORTED,
    /* VK_CAPITAL     0x14 */  KB_NOT_SUPPORTED,
    /* VK_KANA        0x15 */  KB_NOT_SUPPORTED,
    /* VK_IME_ON      0x16 */  KB_NOT_SUPPORTED,
    /* VK_JUNJA       0x17 */  KB_NOT_SUPPORTED,
    /* VK_FINAL       0x18 */  KB_NOT_SUPPORTED,
    /* VK_HANJA       0x19 */  KB_NOT_SUPPORTED,
    /* VK_IME_OFF     0x1A */  KB_NOT_SUPPORTED,
    /* VK_ESCAPE      0x1B */  KB_ESCAPE,
    /* VK_CONVERT     0x1C */  KB_NOT_SUPPORTED,
    /* VK_NONCONVERT  0x1D */  KB_NOT_SUPPORTED,
    /* VK_ACCEPT      0x1E */  KB_NOT_SUPPORTED,
    /* VK_MODECHANGE  0x1F */  KB_NOT_SUPPORTED,

    /* VK_SPACE       0x20 */  KB_SPACE,
    /* VK_PRIOR       0x21 */  KB_NOT_SUPPORTED,
    /* VK_NEXT        0x22 */  KB_NOT_SUPPORTED,
    /* VK_END         0x23 */  KB_NOT_SUPPORTED,
    /* VK_HOME        0x24 */  KB_NOT_SUPPORTED,
    /* VK_LEFT        0x25 */  KB_NOT_SUPPORTED,
    /* VK_UP          0x26 */  KB_NOT_SUPPORTED,
    /* VK_RIGHT       0x27 */  KB_NOT_SUPPORTED,
    /* VK_DOWN        0x28 */  KB_NOT_SUPPORTED,
    /* VK_SELECT      0x29 */  KB_NOT_SUPPORTED,
    /* VK_PRINT       0x2A */  KB_NOT_SUPPORTED,
    /* VK_EXECUTE     0x2B */  KB_NOT_SUPPORTED,
    /* VK_SNAPSHOT    0x2C */  KB_NOT_SUPPORTED,
    /* VK_INSERT      0x2D */  KB_NOT_SUPPORTED,
    /* VK_DELETE      0x2E */  KB_NOT_SUPPORTED,
    /* VK_HELP        0x2F */  KB_NOT_SUPPORTED,

    /* 0 key          0x30 */  KB_0,
    /* 1 key          0x31 */  KB_1,
    /* 2 key          0x32 */  KB_2,
    /* 3 key          0x33 */  KB_3,
    /* 4 key          0x34 */  KB_4,
    /* 5 key          0x35 */  KB_5,
    /* 6 key          0x36 */  KB_6,
    /* 7 key          0x37 */  KB_7,
    /* 8 key          0x38 */  KB_8,
    /* 9 key          0x39 */  KB_9,
    /* Undefined      0x3A */  KB_NOT_SUPPORTED,
    /* Undefined      0x3B */  KB_NOT_SUPPORTED,
    /* Undefined      0x3C */  KB_NOT_SUPPORTED,
    /* Undefined      0x3D */  KB_NOT_SUPPORTED,
    /* Undefined      0x3E */  KB_NOT_SUPPORTED,
    /* Undefined      0x3F */  KB_NOT_SUPPORTED,

    /* Undefined      0x40 */  KB_NOT_SUPPORTED,
    /* A key          0x41 */  KB_A,
    /* B key          0x42 */  KB_B,
    /* C key          0x43 */  KB_C,
    /* D key          0x44 */  KB_D,
    /* E key          0x45 */  KB_E,
    /* F key          0x46 */  KB_F,
    /* G key          0x47 */  KB_G,
    /* H key          0x48 */  KB_H,
    /* I key          0x49 */  KB_I,
    /* J key          0x4A */  KB_J,
    /* K key          0x4B */  KB_K,
    /* L key          0x4C */  KB_L,
    /* M key          0x4D */  KB_M,
    /* N key          0x4E */  KB_N,
    /* O key          0x4F */  KB_O,

    /* P key          0x50 */  KB_P,
    /* Q key          0x51 */  KB_Q,
    /* R key          0x52 */  KB_R,
    /* S key          0x53 */  KB_S,
    /* T key          0x54 */  KB_T,
    /* U key          0x55 */  KB_U,
    /* V key          0x56 */  KB_V,
    /* W key          0x57 */  KB_W,
    /* X key          0x58 */  KB_X,
    /* Y key          0x59 */  KB_Y,
    /* Z key          0x5A */  KB_Z,
    /* VK_LWIN        0x5B */  KB_NOT_SUPPORTED,
    /* VK_RWIN        0x5C */  KB_NOT_SUPPORTED,
    /* VK_APPS        0x5D */  KB_NOT_SUPPORTED,
    /* -              0x5E */  KB_NOT_SUPPORTED,
    /* VK_SLEEP       0x5F */  KB_NOT_SUPPORTED,

    /* VK_NUMPAD0     0x60 */  KB_NOT_SUPPORTED,
    /* VK_NUMPAD1     0x61 */  KB_NOT_SUPPORTED,
    /* VK_NUMPAD2     0x62 */  KB_NOT_SUPPORTED,
    /* VK_NUMPAD3     0x63 */  KB_NOT_SUPPORTED,
    /* VK_NUMPAD4     0x64 */  KB_NOT_SUPPORTED,
    /* VK_NUMPAD5     0x65 */  KB_NOT_SUPPORTED,
    /* VK_NUMPAD6     0x66 */  KB_NOT_SUPPORTED,
    /* VK_NUMPAD7     0x67 */  KB_NOT_SUPPORTED,
    /* VK_NUMPAD8     0x68 */  KB_NOT_SUPPORTED,
    /* VK_NUMPAD9     0x69 */  KB_NOT_SUPPORTED,
    /* VK_MULTIPLY    0x6A */  KB_NOT_SUPPORTED,
    /* VK_ADD         0x6B */  KB_NOT_SUPPORTED,
    /* VK_SEPARATOR   0x6C */  KB_NOT_SUPPORTED,
    /* VK_SUBTRACT    0x6D */  KB_NOT_SUPPORTED,
    /* VK_DECIMAL     0x6E */  KB_NOT_SUPPORTED,
    /* VK_DIVIDE      0x6F */  KB_NOT_SUPPORTED,

    /* VK_F1          0x70 */  KB_NOT_SUPPORTED,
    /* VK_F2          0x71 */  KB_NOT_SUPPORTED,
    /* VK_F3          0x72 */  KB_NOT_SUPPORTED,
    /* VK_F4          0x73 */  KB_NOT_SUPPORTED,
    /* VK_F5          0x74 */  KB_NOT_SUPPORTED,
    /* VK_F6          0x75 */  KB_NOT_SUPPORTED,
    /* VK_F7          0x76 */  KB_NOT_SUPPORTED,
    /* VK_F8          0x77 */  KB_NOT_SUPPORTED,
    /* VK_F9          0x78 */  KB_NOT_SUPPORTED,
    /* VK_F10         0x79 */  KB_NOT_SUPPORTED,
    /* VK_F11         0x7A */  KB_NOT_SUPPORTED,
    /* VK_F12         0x7B */  KB_NOT_SUPPORTED,
    /* VK_F13         0x7C */  KB_NOT_SUPPORTED,
    /* VK_F14         0x7D */  KB_NOT_SUPPORTED,
    /* VK_F15         0x7E */  KB_NOT_SUPPORTED,
    /* VK_F16         0x7F */  KB_NOT_SUPPORTED,

    /* VK_F17         0x80 */  KB_NOT_SUPPORTED,
    /* VK_F18         0x81 */  KB_NOT_SUPPORTED,
    /* VK_F19         0x82 */  KB_NOT_SUPPORTED,
    /* VK_F20         0x83 */  KB_NOT_SUPPORTED,
    /* VK_F21         0x84 */  KB_NOT_SUPPORTED,
    /* VK_F22         0x85 */  KB_NOT_SUPPORTED,
    /* VK_F23         0x86 */  KB_NOT_SUPPORTED,
    /* VK_F24         0x87 */  KB_NOT_SUPPORTED,
    /* -              0x88 */  KB_NOT_SUPPORTED,
    /* -              0x89 */  KB_NOT_SUPPORTED,
    /* -              0x8A */  KB_NOT_SUPPORTED,
    /* -              0x8B */  KB_NOT_SUPPORTED,
    /* -              0x8C */  KB_NOT_SUPPORTED,
    /* -              0x8D */  KB_NOT_SUPPORTED,
    /* -              0x8E */  KB_NOT_SUPPORTED,
    /* -              0x8F */  KB_NOT_SUPPORTED,

    /* VK_NUMLOCK     0x90 */  KB_NOT_SUPPORTED,
    /* VK_SCROLL      0x91 */  KB_NOT_SUPPORTED,
    /* -              0x92 */  KB_NOT_SUPPORTED,
    /* -              0x93 */  KB_NOT_SUPPORTED,
    /* -              0x94 */  KB_NOT_SUPPORTED,
    /* -              0x95 */  KB_NOT_SUPPORTED,
    /* -              0x96 */  KB_NOT_SUPPORTED,
    /* -              0x97 */  KB_NOT_SUPPORTED,
    /* -              0x98 */  KB_NOT_SUPPORTED,
    /* -              0x99 */  KB_NOT_SUPPORTED,
    /* -              0x9A */  KB_NOT_SUPPORTED,
    /* -              0x9B */  KB_NOT_SUPPORTED,
    /* -              0x9C */  KB_NOT_SUPPORTED,
    /* -              0x9D */  KB_NOT_SUPPORTED,
    /* -              0x9E */  KB_NOT_SUPPORTED,
    /* -              0x9F */  KB_NOT_SUPPORTED,

    /* VK_LSHIFT      0xA0 */  KB_NOT_SUPPORTED,
    /* VK_RSHIFT      0xA1 */  KB_NOT_SUPPORTED,
    /* VK_LCONTROL    0xA2 */  KB_LCTRL,
    /* VK_RCONTROL    0xA3 */  KB_NOT_SUPPORTED,
    /* VK_LMENU       0xA4 */  KB_ALT,
};
_Static_assert(ARRAY_COUNT(win32_vk_code_to_keyboard_key) == 0xA5, "VK map is not the expected size.");

void platform_win32_init_input()
{
    struct PlatformWin32Input* input = platform_win32_get_input();
    ZERO_ARRAY(input->keyboard_buf);
    ZERO_ARRAY(input->prev_keyboard_buf);
    input->mouse_screen_x = 0;
    input->mouse_screen_y = 0;
    input->prev_mouse_screen_x = 0;
    input->prev_mouse_screen_y = 0;
}

enum KeyboardKey platform_win32_vk_code_to_keyboard_key(u64 vk_key_code)
{
    ASSERT(vk_key_code < ARRAY_COUNT(win32_vk_code_to_keyboard_key), "Invalid vk key code %llu", vk_key_code);
    return win32_vk_code_to_keyboard_key[vk_key_code];
}

void platform_win32_input_sample()
{
    struct PlatformWin32Common* common = platform_win32_get_common();
    struct PlatformWin32Input* input = platform_win32_get_input();

    POINT p;
    GetCursorPos(&p);
    ScreenToClient(common->hwnd, &p);

    input->mouse_screen_x = p.x;
    input->mouse_screen_y = p.y;
}

void platform_win32_input_end_frame()
{
    struct PlatformWin32Input* input = platform_win32_get_input();
    COPY_ARRAY(input->prev_keyboard_buf, input->keyboard_buf);
    input->prev_mouse_button = input->mouse_button;
    input->prev_mouse_screen_x = input->mouse_screen_x;
    input->prev_mouse_screen_y = input->mouse_screen_y;
}

static u32 is_keyboard_key_valid(const enum KeyboardKey k)
{
    return k >= KB_NOT_SUPPORTED && k < NUM_KEYBOARD_KEYS;
}

void platform_win32_set_keyboard_key(const enum KeyboardKey k)
{
    struct PlatformWin32Input* input = platform_win32_get_input();
    ASSERT(is_keyboard_key_valid(k), "Invalid keyboard key %u", k);
    input->keyboard_buf[k >> 3] |= (1 << (k & 7));
}

void platform_win32_clear_keyboard_key(const enum KeyboardKey k)
{
    struct PlatformWin32Input* input = platform_win32_get_input();
    ASSERT(is_keyboard_key_valid(k), "Invalid keyboard key %u", k);
    input->keyboard_buf[k >> 3] &= ~(1 << (k & 7));
}

u32 platform_win32_is_keyboard_key_down(const enum KeyboardKey k)
{
    struct PlatformWin32Input* input = platform_win32_get_input();
    ASSERT(is_keyboard_key_valid(k), "Invalid keyboard key %u", k);
    return (input->keyboard_buf[k >> 3] & (1 << (k & 7))) != 0;
}

static u32 is_valid_mouse_button(const enum MouseButton m)
{
    return m >= GAME_MB_LEFT && m < NUM_MOUSE_BUTTONS;
}

void platform_win32_set_mouse_button(const enum MouseButton m)
{
    struct PlatformWin32Input* input = platform_win32_get_input();
    ASSERT(is_valid_mouse_button(m), "Invalid mouse button %u", m);
    input->mouse_button |= (u8)(1U << m);
}

void platform_win32_clear_mouse_button(const enum MouseButton m)
{
    struct PlatformWin32Input* input = platform_win32_get_input();
    ASSERT(is_valid_mouse_button(m), "Invalid mouse button %u", m);
    input->mouse_button &= ~(u8)(1U << m);
}

u32 platform_win32_is_mouse_button_down(const enum MouseButton m)
{
    struct PlatformWin32Input* input = platform_win32_get_input();
    ASSERT(is_valid_mouse_button(m), "Invalid mouse button %u", m);
    return (input->mouse_button & (1U << m)) != 0;
}

void platform_win32_get_mouse_screen_position(s32* x, s32* y)
{
    struct PlatformWin32Input* input = platform_win32_get_input();
    *x = input->mouse_screen_x;
    *y = input->mouse_screen_y;
}

void platform_win32_get_mouse_screen_delta(s32* x, s32* y)
{
    struct PlatformWin32Input* input = platform_win32_get_input();
    *x = input->mouse_screen_x - input->prev_mouse_screen_x;
    *y = input->mouse_screen_y - input->prev_mouse_screen_y;
}

void platform_win32_input_to_player_input(struct PlayerInput* player_input)
{
    player_input->move_x =
        platform_win32_is_keyboard_key_down(KB_A) && platform_win32_is_keyboard_key_down(KB_D) ? 127
        : platform_win32_is_keyboard_key_down(KB_A) ? 0
        : platform_win32_is_keyboard_key_down(KB_D) ? 254
        : 127;

    player_input->move_y =
        platform_win32_is_keyboard_key_down(KB_W) && platform_win32_is_keyboard_key_down(KB_S) ? 127
        : platform_win32_is_keyboard_key_down(KB_S) ? 0
        : platform_win32_is_keyboard_key_down(KB_W) ? 254
        : 127;

}

