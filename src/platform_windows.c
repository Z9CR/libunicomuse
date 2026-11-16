#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "internal.h"

#include <limits.h>

static int g_initialized;

static int ucomuse_virtual_key(ucomuse_key key)
{
    if ((key >= UCOMUSE_KEY_A && key <= UCOMUSE_KEY_Z) ||
        (key >= UCOMUSE_KEY_0 && key <= UCOMUSE_KEY_9)) {
        return (int)key;
    }
    if (key >= UCOMUSE_KEY_F1 && key <= UCOMUSE_KEY_F12) {
        return VK_F1 + (int)(key - UCOMUSE_KEY_F1);
    }
    if (key >= UCOMUSE_KEY_KP_0 && key <= UCOMUSE_KEY_KP_9) {
        return VK_NUMPAD0 + (int)(key - UCOMUSE_KEY_KP_0);
    }
    switch (key) {
    case UCOMUSE_KEY_APOSTROPHE: return VK_OEM_7;
    case UCOMUSE_KEY_COMMA: return VK_OEM_COMMA;
    case UCOMUSE_KEY_MINUS: return VK_OEM_MINUS;
    case UCOMUSE_KEY_PERIOD: return VK_OEM_PERIOD;
    case UCOMUSE_KEY_SLASH: return VK_OEM_2;
    case UCOMUSE_KEY_SEMICOLON: return VK_OEM_1;
    case UCOMUSE_KEY_EQUAL: return VK_OEM_PLUS;
    case UCOMUSE_KEY_LEFT_BRACKET: return VK_OEM_4;
    case UCOMUSE_KEY_BACKSLASH: return VK_OEM_5;
    case UCOMUSE_KEY_RIGHT_BRACKET: return VK_OEM_6;
    case UCOMUSE_KEY_GRAVE: return VK_OEM_3;
    case UCOMUSE_KEY_SPACE: return VK_SPACE;
    case UCOMUSE_KEY_ESCAPE: return VK_ESCAPE;
    case UCOMUSE_KEY_ENTER: return VK_RETURN;
    case UCOMUSE_KEY_TAB: return VK_TAB;
    case UCOMUSE_KEY_BACKSPACE: return VK_BACK;
    case UCOMUSE_KEY_INSERT: return VK_INSERT;
    case UCOMUSE_KEY_DELETE: return VK_DELETE;
    case UCOMUSE_KEY_RIGHT: return VK_RIGHT;
    case UCOMUSE_KEY_LEFT: return VK_LEFT;
    case UCOMUSE_KEY_DOWN: return VK_DOWN;
    case UCOMUSE_KEY_UP: return VK_UP;
    case UCOMUSE_KEY_PAGE_UP: return VK_PRIOR;
    case UCOMUSE_KEY_PAGE_DOWN: return VK_NEXT;
    case UCOMUSE_KEY_HOME: return VK_HOME;
    case UCOMUSE_KEY_END: return VK_END;
    case UCOMUSE_KEY_CAPS_LOCK: return VK_CAPITAL;
    case UCOMUSE_KEY_SCROLL_LOCK: return VK_SCROLL;
    case UCOMUSE_KEY_NUM_LOCK: return VK_NUMLOCK;
    case UCOMUSE_KEY_PRINT_SCREEN: return VK_SNAPSHOT;
    case UCOMUSE_KEY_PAUSE: return VK_PAUSE;
    case UCOMUSE_KEY_LEFT_SHIFT: return VK_LSHIFT;
    case UCOMUSE_KEY_LEFT_CONTROL: return VK_LCONTROL;
    case UCOMUSE_KEY_LEFT_ALT: return VK_LMENU;
    case UCOMUSE_KEY_LEFT_SUPER: return VK_LWIN;
    case UCOMUSE_KEY_RIGHT_SHIFT: return VK_RSHIFT;
    case UCOMUSE_KEY_RIGHT_CONTROL: return VK_RCONTROL;
    case UCOMUSE_KEY_RIGHT_ALT: return VK_RMENU;
    case UCOMUSE_KEY_RIGHT_SUPER: return VK_RWIN;
    case UCOMUSE_KEY_MENU: return VK_APPS;
    case UCOMUSE_KEY_KP_DECIMAL: return VK_DECIMAL;
    case UCOMUSE_KEY_KP_DIVIDE: return VK_DIVIDE;
    case UCOMUSE_KEY_KP_MULTIPLY: return VK_MULTIPLY;
    case UCOMUSE_KEY_KP_SUBTRACT: return VK_SUBTRACT;
    case UCOMUSE_KEY_KP_ADD: return VK_ADD;
    case UCOMUSE_KEY_KP_ENTER: return VK_RETURN;
    default: return 0;
    }
}

ucomuse_status ucomuse_platform_init(void)
{
    g_initialized = 1;
    return UCOMUSE_STATUS_OK;
}

void ucomuse_platform_shutdown(void)
{
    g_initialized = 0;
}

ucomuse_status ucomuse_platform_get_position(int32_t *x, int32_t *y)
{
    POINT point;
    if (!g_initialized) {
        return UCOMUSE_STATUS_NOT_INITIALIZED;
    }
    if (x == NULL || y == NULL) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
    if (!GetCursorPos(&point)) {
        return UCOMUSE_STATUS_ERROR;
    }
    *x = point.x;
    *y = point.y;
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_move_to(int32_t x, int32_t y)
{
    if (!g_initialized) {
        return UCOMUSE_STATUS_NOT_INITIALIZED;
    }
    return SetCursorPos(x, y) ? UCOMUSE_STATUS_OK : UCOMUSE_STATUS_ERROR;
}

ucomuse_status ucomuse_platform_move_by(int32_t dx, int32_t dy)
{
    POINT point;
    int64_t x;
    int64_t y;
    if (!g_initialized) {
        return UCOMUSE_STATUS_NOT_INITIALIZED;
    }
    if (!GetCursorPos(&point)) {
        return UCOMUSE_STATUS_ERROR;
    }
    x = (int64_t)point.x + dx;
    y = (int64_t)point.y + dy;
    if (x < INT32_MIN || x > INT32_MAX || y < INT32_MIN || y > INT32_MAX) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
    return ucomuse_platform_move_to((int32_t)x, (int32_t)y);
}

ucomuse_status ucomuse_platform_button(ucomuse_mouse_button button, int down)
{
    static const DWORD down_flags[] = {
        MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_MIDDLEDOWN,
        MOUSEEVENTF_XDOWN, MOUSEEVENTF_XDOWN
    };
    static const DWORD up_flags[] = {
        MOUSEEVENTF_LEFTUP, MOUSEEVENTF_RIGHTUP, MOUSEEVENTF_MIDDLEUP,
        MOUSEEVENTF_XUP, MOUSEEVENTF_XUP
    };
    INPUT input;
    if (!g_initialized) {
        return UCOMUSE_STATUS_NOT_INITIALIZED;
    }
    ZeroMemory(&input, sizeof(input));
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = down ? down_flags[button] : up_flags[button];
    if (button == UCOMUSE_MOUSE_X1) {
        input.mi.mouseData = XBUTTON1;
    } else if (button == UCOMUSE_MOUSE_X2) {
        input.mi.mouseData = XBUTTON2;
    }
    return SendInput(1, &input, sizeof(input)) == 1 ?
        UCOMUSE_STATUS_OK : UCOMUSE_STATUS_ERROR;
}

ucomuse_status ucomuse_platform_wheel(int delta)
{
    INPUT input;
    int64_t amount;
    if (!g_initialized) {
        return UCOMUSE_STATUS_NOT_INITIALIZED;
    }
    ZeroMemory(&input, sizeof(input));
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_WHEEL;
    amount = (int64_t)delta * WHEEL_DELTA;
    if (amount < INT32_MIN || amount > INT32_MAX) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
    input.mi.mouseData = (DWORD)(int32_t)amount;
    return SendInput(1, &input, sizeof(input)) == 1 ?
        UCOMUSE_STATUS_OK : UCOMUSE_STATUS_ERROR;
}

ucomuse_status ucomuse_platform_key_pressed(ucomuse_key key, int *pressed)
{
    int vk;
    if (!g_initialized) {
        return UCOMUSE_STATUS_NOT_INITIALIZED;
    }
    if (pressed == NULL) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
    vk = ucomuse_virtual_key(key);
    if (vk == 0) {
        return UCOMUSE_STATUS_NOT_SUPPORTED;
    }
    *pressed = (GetAsyncKeyState(vk) & 0x8000) != 0 ? 1 : 0;
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_get_key(ucomuse_key *key)
{
    int value;
    int queried = 0;
    if (key == NULL) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
    for (value = 32; value <= UCOMUSE_KEY_MENU; ++value) {
        int pressed = 0;
        ucomuse_status status = ucomuse_platform_key_pressed(
            (ucomuse_key)value, &pressed);
        if (status == UCOMUSE_STATUS_OK) {
            queried = 1;
        }
        if (status == UCOMUSE_STATUS_OK && pressed) {
            *key = (ucomuse_key)value;
            return UCOMUSE_STATUS_OK;
        }
    }
    if (!queried) {
        return UCOMUSE_STATUS_ERROR;
    }
    *key = UCOMUSE_KEY_NONE;
    return UCOMUSE_STATUS_OK;
}

void ucomuse_platform_sleep(uint32_t milliseconds)
{
    Sleep(milliseconds);
}
