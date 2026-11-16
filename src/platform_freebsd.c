#define _POSIX_C_SOURCE 199309L
#include "internal.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <limits.h>
#include <time.h>

static Display *g_display;
static Window g_root;
static int g_screen;

static KeySym ucomuse_keysym(ucomuse_key key)
{
    if ((key >= UCOMUSE_KEY_A && key <= UCOMUSE_KEY_Z) ||
        (key >= UCOMUSE_KEY_0 && key <= UCOMUSE_KEY_9) ||
        key == UCOMUSE_KEY_SPACE) {
        return (KeySym)key;
    }
    switch (key) {
    case UCOMUSE_KEY_APOSTROPHE: return XK_apostrophe;
    case UCOMUSE_KEY_COMMA: return XK_comma;
    case UCOMUSE_KEY_MINUS: return XK_minus;
    case UCOMUSE_KEY_PERIOD: return XK_period;
    case UCOMUSE_KEY_SLASH: return XK_slash;
    case UCOMUSE_KEY_SEMICOLON: return XK_semicolon;
    case UCOMUSE_KEY_EQUAL: return XK_equal;
    case UCOMUSE_KEY_LEFT_BRACKET: return XK_bracketleft;
    case UCOMUSE_KEY_BACKSLASH: return XK_backslash;
    case UCOMUSE_KEY_RIGHT_BRACKET: return XK_bracketright;
    case UCOMUSE_KEY_GRAVE: return XK_grave;
    case UCOMUSE_KEY_ESCAPE: return XK_Escape;
    case UCOMUSE_KEY_ENTER: return XK_Return;
    case UCOMUSE_KEY_TAB: return XK_Tab;
    case UCOMUSE_KEY_BACKSPACE: return XK_BackSpace;
    case UCOMUSE_KEY_INSERT: return XK_Insert;
    case UCOMUSE_KEY_DELETE: return XK_Delete;
    case UCOMUSE_KEY_RIGHT: return XK_Right;
    case UCOMUSE_KEY_LEFT: return XK_Left;
    case UCOMUSE_KEY_DOWN: return XK_Down;
    case UCOMUSE_KEY_UP: return XK_Up;
    case UCOMUSE_KEY_PAGE_UP: return XK_Prior;
    case UCOMUSE_KEY_PAGE_DOWN: return XK_Next;
    case UCOMUSE_KEY_HOME: return XK_Home;
    case UCOMUSE_KEY_END: return XK_End;
    case UCOMUSE_KEY_CAPS_LOCK: return XK_Caps_Lock;
    case UCOMUSE_KEY_SCROLL_LOCK: return XK_Scroll_Lock;
    case UCOMUSE_KEY_NUM_LOCK: return XK_Num_Lock;
    case UCOMUSE_KEY_PRINT_SCREEN: return XK_Print;
    case UCOMUSE_KEY_PAUSE: return XK_Pause;
    case UCOMUSE_KEY_LEFT_SHIFT: return XK_Shift_L;
    case UCOMUSE_KEY_RIGHT_SHIFT: return XK_Shift_R;
    case UCOMUSE_KEY_LEFT_CONTROL: return XK_Control_L;
    case UCOMUSE_KEY_RIGHT_CONTROL: return XK_Control_R;
    case UCOMUSE_KEY_LEFT_ALT: return XK_Alt_L;
    case UCOMUSE_KEY_RIGHT_ALT: return XK_Alt_R;
    case UCOMUSE_KEY_LEFT_SUPER: return XK_Super_L;
    case UCOMUSE_KEY_RIGHT_SUPER: return XK_Super_R;
    case UCOMUSE_KEY_MENU: return XK_Menu;
    case UCOMUSE_KEY_F1: return XK_F1;
    case UCOMUSE_KEY_F2: return XK_F2;
    case UCOMUSE_KEY_F3: return XK_F3;
    case UCOMUSE_KEY_F4: return XK_F4;
    case UCOMUSE_KEY_F5: return XK_F5;
    case UCOMUSE_KEY_F6: return XK_F6;
    case UCOMUSE_KEY_F7: return XK_F7;
    case UCOMUSE_KEY_F8: return XK_F8;
    case UCOMUSE_KEY_F9: return XK_F9;
    case UCOMUSE_KEY_F10: return XK_F10;
    case UCOMUSE_KEY_F11: return XK_F11;
    case UCOMUSE_KEY_F12: return XK_F12;
    case UCOMUSE_KEY_KP_0: return XK_KP_0;
    case UCOMUSE_KEY_KP_1: return XK_KP_1;
    case UCOMUSE_KEY_KP_2: return XK_KP_2;
    case UCOMUSE_KEY_KP_3: return XK_KP_3;
    case UCOMUSE_KEY_KP_4: return XK_KP_4;
    case UCOMUSE_KEY_KP_5: return XK_KP_5;
    case UCOMUSE_KEY_KP_6: return XK_KP_6;
    case UCOMUSE_KEY_KP_7: return XK_KP_7;
    case UCOMUSE_KEY_KP_8: return XK_KP_8;
    case UCOMUSE_KEY_KP_9: return XK_KP_9;
    case UCOMUSE_KEY_KP_DECIMAL: return XK_KP_Decimal;
    case UCOMUSE_KEY_KP_DIVIDE: return XK_KP_Divide;
    case UCOMUSE_KEY_KP_MULTIPLY: return XK_KP_Multiply;
    case UCOMUSE_KEY_KP_SUBTRACT: return XK_KP_Subtract;
    case UCOMUSE_KEY_KP_ADD: return XK_KP_Add;
    case UCOMUSE_KEY_KP_ENTER: return XK_KP_Enter;
    case UCOMUSE_KEY_KP_EQUAL: return XK_KP_Equal;
    default: return NoSymbol;
    }
}

ucomuse_status ucomuse_platform_init(void)
{
    int event_base;
    int error_base;
    int major = 2;
    int minor = 2;
    XInitThreads();
    g_display = XOpenDisplay(NULL);
    if (g_display == NULL) {
        return UCOMUSE_STATUS_ERROR;
    }
    g_screen = DefaultScreen(g_display);
    g_root = RootWindow(g_display, g_screen);
    if (!XTestQueryExtension(g_display, &event_base, &error_base,
                             &major, &minor)) {
        XCloseDisplay(g_display);
        g_display = NULL;
        return UCOMUSE_STATUS_NOT_SUPPORTED;
    }
    return UCOMUSE_STATUS_OK;
}

void ucomuse_platform_shutdown(void)
{
    if (g_display != NULL) {
        XCloseDisplay(g_display);
        g_display = NULL;
    }
}

ucomuse_status ucomuse_platform_get_position(int32_t *x, int32_t *y)
{
    Window root_return;
    Window child_return;
    int root_x;
    int root_y;
    int win_x;
    int win_y;
    unsigned int mask;
    if (x == NULL || y == NULL) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
    if (g_display == NULL ||
        !XQueryPointer(g_display, g_root, &root_return, &child_return,
                       &root_x, &root_y, &win_x, &win_y, &mask)) {
        return UCOMUSE_STATUS_ERROR;
    }
    *x = root_x;
    *y = root_y;
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_move_to(int32_t x, int32_t y)
{
    if (g_display == NULL ||
        !XTestFakeMotionEvent(g_display, g_screen, x, y, CurrentTime)) {
        return UCOMUSE_STATUS_ERROR;
    }
    XFlush(g_display);
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_move_by(int32_t dx, int32_t dy)
{
    Window root_return;
    Window child_return;
    int root_x;
    int root_y;
    int win_x;
    int win_y;
    int64_t next_x;
    int64_t next_y;
    unsigned int mask;
    if (g_display == NULL ||
        !XQueryPointer(g_display, g_root, &root_return, &child_return,
                       &root_x, &root_y, &win_x, &win_y, &mask)) {
        return UCOMUSE_STATUS_ERROR;
    }
    next_x = (int64_t)root_x + dx;
    next_y = (int64_t)root_y + dy;
    if (next_x < INT32_MIN || next_x > INT32_MAX ||
        next_y < INT32_MIN || next_y > INT32_MAX) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
    if (!XTestFakeMotionEvent(g_display, g_screen, (int)next_x,
                              (int)next_y, CurrentTime)) {
        return UCOMUSE_STATUS_ERROR;
    }
    XFlush(g_display);
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_button(ucomuse_mouse_button button, int down)
{
    static const unsigned int buttons[] = {1, 3, 2, 8, 9};
    if (g_display == NULL ||
        !XTestFakeButtonEvent(g_display, buttons[button], down, CurrentTime)) {
        return UCOMUSE_STATUS_ERROR;
    }
    XFlush(g_display);
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_wheel(int delta)
{
    unsigned int button;
    int i;
    if (g_display == NULL) {
        return UCOMUSE_STATUS_ERROR;
    }
    button = delta > 0 ? 4U : 5U;
    for (i = 0; i < (delta > 0 ? delta : -delta); ++i) {
        if (!XTestFakeButtonEvent(g_display, button, True, CurrentTime) ||
            !XTestFakeButtonEvent(g_display, button, False, CurrentTime)) {
            return UCOMUSE_STATUS_ERROR;
        }
    }
    XFlush(g_display);
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_key_pressed(ucomuse_key key, int *pressed)
{
    char keys[32];
    KeySym sym;
    KeyCode code;
    if (pressed == NULL) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
    sym = ucomuse_keysym(key);
    if (sym == NoSymbol) {
        return UCOMUSE_STATUS_NOT_SUPPORTED;
    }
    code = XKeysymToKeycode(g_display, sym);
    if (code == 0) {
        *pressed = 0;
        return UCOMUSE_STATUS_OK;
    }
    XQueryKeymap(g_display, keys);
    *pressed = (keys[code >> 3] & (1 << (code & 7))) != 0;
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
    struct timespec delay;
    delay.tv_sec = (time_t)(milliseconds / 1000U);
    delay.tv_nsec = (long)(milliseconds % 1000U) * 1000000L;
    while (nanosleep(&delay, &delay) < 0) {
    }
}
