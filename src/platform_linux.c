#define _POSIX_C_SOURCE 200809L
#include "internal.h"

#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/fb.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#ifdef UCOMUSE_HAVE_X11
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#endif

#define UCOMUSE_MAX_KEYBOARDS 64

static int g_uinput_fd = -1;
static int g_keyboard_fds[UCOMUSE_MAX_KEYBOARDS];
static int g_keyboard_count;
static int32_t g_tracked_x;
static int32_t g_tracked_y;
static int g_screen_width = 1920;
static int g_screen_height = 1080;
static int g_use_native;

#ifdef UCOMUSE_HAVE_X11
static Display *g_display;
static Window g_root;
static int g_x11_screen;
#endif

static int ucomuse_emit(int type, int code, int value)
{
    struct input_event event;
    ssize_t written;
    memset(&event, 0, sizeof(event));
    event.type = (unsigned short)type;
    event.code = (unsigned short)code;
    event.value = value;
    written = write(g_uinput_fd, &event, sizeof(event));
    return written == (ssize_t)sizeof(event);
}

static int ucomuse_emit_sync(void)
{
    return ucomuse_emit(EV_SYN, SYN_REPORT, 0);
}

static int ucomuse_evdev_code(ucomuse_key key)
{
    switch (key) {
    case UCOMUSE_KEY_1: return KEY_1;
    case UCOMUSE_KEY_2: return KEY_2;
    case UCOMUSE_KEY_3: return KEY_3;
    case UCOMUSE_KEY_4: return KEY_4;
    case UCOMUSE_KEY_5: return KEY_5;
    case UCOMUSE_KEY_6: return KEY_6;
    case UCOMUSE_KEY_7: return KEY_7;
    case UCOMUSE_KEY_8: return KEY_8;
    case UCOMUSE_KEY_9: return KEY_9;
    case UCOMUSE_KEY_0: return KEY_0;
    case UCOMUSE_KEY_A: return KEY_A;
    case UCOMUSE_KEY_B: return KEY_B;
    case UCOMUSE_KEY_C: return KEY_C;
    case UCOMUSE_KEY_D: return KEY_D;
    case UCOMUSE_KEY_E: return KEY_E;
    case UCOMUSE_KEY_F: return KEY_F;
    case UCOMUSE_KEY_G: return KEY_G;
    case UCOMUSE_KEY_H: return KEY_H;
    case UCOMUSE_KEY_I: return KEY_I;
    case UCOMUSE_KEY_J: return KEY_J;
    case UCOMUSE_KEY_K: return KEY_K;
    case UCOMUSE_KEY_L: return KEY_L;
    case UCOMUSE_KEY_M: return KEY_M;
    case UCOMUSE_KEY_N: return KEY_N;
    case UCOMUSE_KEY_O: return KEY_O;
    case UCOMUSE_KEY_P: return KEY_P;
    case UCOMUSE_KEY_Q: return KEY_Q;
    case UCOMUSE_KEY_R: return KEY_R;
    case UCOMUSE_KEY_S: return KEY_S;
    case UCOMUSE_KEY_T: return KEY_T;
    case UCOMUSE_KEY_U: return KEY_U;
    case UCOMUSE_KEY_V: return KEY_V;
    case UCOMUSE_KEY_W: return KEY_W;
    case UCOMUSE_KEY_X: return KEY_X;
    case UCOMUSE_KEY_Y: return KEY_Y;
    case UCOMUSE_KEY_Z: return KEY_Z;
    case UCOMUSE_KEY_ESCAPE: return KEY_ESC;
    case UCOMUSE_KEY_ENTER: return KEY_ENTER;
    case UCOMUSE_KEY_TAB: return KEY_TAB;
    case UCOMUSE_KEY_BACKSPACE: return KEY_BACKSPACE;
    case UCOMUSE_KEY_INSERT: return KEY_INSERT;
    case UCOMUSE_KEY_DELETE: return KEY_DELETE;
    case UCOMUSE_KEY_RIGHT: return KEY_RIGHT;
    case UCOMUSE_KEY_LEFT: return KEY_LEFT;
    case UCOMUSE_KEY_DOWN: return KEY_DOWN;
    case UCOMUSE_KEY_UP: return KEY_UP;
    case UCOMUSE_KEY_PAGE_UP: return KEY_PAGEUP;
    case UCOMUSE_KEY_PAGE_DOWN: return KEY_PAGEDOWN;
    case UCOMUSE_KEY_HOME: return KEY_HOME;
    case UCOMUSE_KEY_END: return KEY_END;
    case UCOMUSE_KEY_CAPS_LOCK: return KEY_CAPSLOCK;
    case UCOMUSE_KEY_SCROLL_LOCK: return KEY_SCROLLLOCK;
    case UCOMUSE_KEY_NUM_LOCK: return KEY_NUMLOCK;
    case UCOMUSE_KEY_PRINT_SCREEN: return KEY_SYSRQ;
    case UCOMUSE_KEY_PAUSE: return KEY_PAUSE;
    case UCOMUSE_KEY_LEFT_SHIFT: return KEY_LEFTSHIFT;
    case UCOMUSE_KEY_LEFT_CONTROL: return KEY_LEFTCTRL;
    case UCOMUSE_KEY_LEFT_ALT: return KEY_LEFTALT;
    case UCOMUSE_KEY_LEFT_SUPER: return KEY_LEFTMETA;
    case UCOMUSE_KEY_RIGHT_SHIFT: return KEY_RIGHTSHIFT;
    case UCOMUSE_KEY_RIGHT_CONTROL: return KEY_RIGHTCTRL;
    case UCOMUSE_KEY_RIGHT_ALT: return KEY_RIGHTALT;
    case UCOMUSE_KEY_RIGHT_SUPER: return KEY_RIGHTMETA;
    case UCOMUSE_KEY_MENU: return KEY_MENU;
    case UCOMUSE_KEY_SPACE: return KEY_SPACE;
    case UCOMUSE_KEY_MINUS: return KEY_MINUS;
    case UCOMUSE_KEY_EQUAL: return KEY_EQUAL;
    case UCOMUSE_KEY_LEFT_BRACKET: return KEY_LEFTBRACE;
    case UCOMUSE_KEY_RIGHT_BRACKET: return KEY_RIGHTBRACE;
    case UCOMUSE_KEY_BACKSLASH: return KEY_BACKSLASH;
    case UCOMUSE_KEY_SEMICOLON: return KEY_SEMICOLON;
    case UCOMUSE_KEY_APOSTROPHE: return KEY_APOSTROPHE;
    case UCOMUSE_KEY_GRAVE: return KEY_GRAVE;
    case UCOMUSE_KEY_COMMA: return KEY_COMMA;
    case UCOMUSE_KEY_PERIOD: return KEY_DOT;
    case UCOMUSE_KEY_SLASH: return KEY_SLASH;
    case UCOMUSE_KEY_F1: return KEY_F1;
    case UCOMUSE_KEY_F2: return KEY_F2;
    case UCOMUSE_KEY_F3: return KEY_F3;
    case UCOMUSE_KEY_F4: return KEY_F4;
    case UCOMUSE_KEY_F5: return KEY_F5;
    case UCOMUSE_KEY_F6: return KEY_F6;
    case UCOMUSE_KEY_F7: return KEY_F7;
    case UCOMUSE_KEY_F8: return KEY_F8;
    case UCOMUSE_KEY_F9: return KEY_F9;
    case UCOMUSE_KEY_F10: return KEY_F10;
    case UCOMUSE_KEY_F11: return KEY_F11;
    case UCOMUSE_KEY_F12: return KEY_F12;
    case UCOMUSE_KEY_KP_0: return KEY_KP0;
    case UCOMUSE_KEY_KP_1: return KEY_KP1;
    case UCOMUSE_KEY_KP_2: return KEY_KP2;
    case UCOMUSE_KEY_KP_3: return KEY_KP3;
    case UCOMUSE_KEY_KP_4: return KEY_KP4;
    case UCOMUSE_KEY_KP_5: return KEY_KP5;
    case UCOMUSE_KEY_KP_6: return KEY_KP6;
    case UCOMUSE_KEY_KP_7: return KEY_KP7;
    case UCOMUSE_KEY_KP_8: return KEY_KP8;
    case UCOMUSE_KEY_KP_9: return KEY_KP9;
    case UCOMUSE_KEY_KP_DECIMAL: return KEY_KPDOT;
    case UCOMUSE_KEY_KP_DIVIDE: return KEY_KPSLASH;
    case UCOMUSE_KEY_KP_MULTIPLY: return KEY_KPASTERISK;
    case UCOMUSE_KEY_KP_SUBTRACT: return KEY_KPMINUS;
    case UCOMUSE_KEY_KP_ADD: return KEY_KPPLUS;
    case UCOMUSE_KEY_KP_ENTER: return KEY_KPENTER;
    case UCOMUSE_KEY_KP_EQUAL: return KEY_KPEQUAL;
    default: return -1;
    }
}

static ucomuse_status ucomuse_native_key_pressed(ucomuse_key key,
                                                  int *pressed)
{
    unsigned char bits[(KEY_CNT + 7) / 8];
    int code = ucomuse_evdev_code(key);
    int i;
    int queried = 0;
    if (code < 0) {
        return UCOMUSE_STATUS_NOT_SUPPORTED;
    }
    if (g_keyboard_count == 0) {
        return UCOMUSE_STATUS_ERROR;
    }
    *pressed = 0;
    for (i = 0; i < g_keyboard_count; ++i) {
        memset(bits, 0, sizeof(bits));
        if (ioctl(g_keyboard_fds[i], EVIOCGKEY(sizeof(bits)), bits) < 0) {
            continue;
        }
        queried = 1;
        if ((bits[code / 8] & (1U << (code % 8))) != 0U) {
            *pressed = 1;
            break;
        }
    }
    return queried ? UCOMUSE_STATUS_OK : UCOMUSE_STATUS_ERROR;
}

#ifdef UCOMUSE_HAVE_X11
static KeySym ucomuse_keysym(ucomuse_key key)
{
    if ((key >= UCOMUSE_KEY_A && key <= UCOMUSE_KEY_Z) ||
        (key >= UCOMUSE_KEY_0 && key <= UCOMUSE_KEY_9) ||
        key == UCOMUSE_KEY_SPACE) {
        return (KeySym)key;
    }
    switch (key) {
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
    default: return NoSymbol;
    }
}

static ucomuse_status ucomuse_x11_key_pressed(ucomuse_key key, int *pressed)
{
    char keys[32];
    KeyCode code;
    KeySym sym = ucomuse_keysym(key);
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
#endif

static ucomuse_status ucomuse_native_init(void)
{
    struct uinput_user_dev device;
    DIR *input_dir;
    struct dirent *entry;
    struct fb_var_screeninfo screen_info;
    int framebuffer;

    framebuffer = open("/dev/fb0", O_RDONLY);
    if (framebuffer >= 0) {
        if (ioctl(framebuffer, FBIOGET_VSCREENINFO, &screen_info) == 0 &&
            screen_info.xres > 1U && screen_info.yres > 1U) {
            g_screen_width = (int)screen_info.xres;
            g_screen_height = (int)screen_info.yres;
        }
        close(framebuffer);
    }
    g_uinput_fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (g_uinput_fd < 0) {
        return UCOMUSE_STATUS_ERROR;
    }
    if (ioctl(g_uinput_fd, UI_SET_EVBIT, EV_KEY) < 0 ||
        ioctl(g_uinput_fd, UI_SET_EVBIT, EV_REL) < 0 ||
        ioctl(g_uinput_fd, UI_SET_EVBIT, EV_ABS) < 0 ||
        ioctl(g_uinput_fd, UI_SET_EVBIT, EV_SYN) < 0 ||
        ioctl(g_uinput_fd, UI_SET_KEYBIT, BTN_LEFT) < 0 ||
        ioctl(g_uinput_fd, UI_SET_KEYBIT, BTN_RIGHT) < 0 ||
        ioctl(g_uinput_fd, UI_SET_KEYBIT, BTN_MIDDLE) < 0 ||
        ioctl(g_uinput_fd, UI_SET_KEYBIT, BTN_SIDE) < 0 ||
        ioctl(g_uinput_fd, UI_SET_KEYBIT, BTN_EXTRA) < 0 ||
        ioctl(g_uinput_fd, UI_SET_RELBIT, REL_X) < 0 ||
        ioctl(g_uinput_fd, UI_SET_RELBIT, REL_Y) < 0 ||
        ioctl(g_uinput_fd, UI_SET_RELBIT, REL_WHEEL) < 0 ||
        ioctl(g_uinput_fd, UI_SET_ABSBIT, ABS_X) < 0 ||
        ioctl(g_uinput_fd, UI_SET_ABSBIT, ABS_Y) < 0) {
        close(g_uinput_fd);
        g_uinput_fd = -1;
        return UCOMUSE_STATUS_ERROR;
    }
    memset(&device, 0, sizeof(device));
    (void)snprintf(device.name, UINPUT_MAX_NAME_SIZE, "libunicomuse");
    device.id.bustype = BUS_USB;
    device.id.vendor = 0x1145;
    device.id.product = 0x2222;
    device.absmin[ABS_X] = 0;
    device.absmax[ABS_X] = 65535;
    device.absmin[ABS_Y] = 0;
    device.absmax[ABS_Y] = 65535;
    if (write(g_uinput_fd, &device, sizeof(device)) !=
            (ssize_t)sizeof(device) ||
        ioctl(g_uinput_fd, UI_DEV_CREATE) < 0) {
        close(g_uinput_fd);
        g_uinput_fd = -1;
        return UCOMUSE_STATUS_ERROR;
    }

    input_dir = opendir("/dev/input");
    if (input_dir == NULL) {
        return UCOMUSE_STATUS_OK;
    }
    while ((entry = readdir(input_dir)) != NULL &&
           g_keyboard_count < UCOMUSE_MAX_KEYBOARDS) {
        char path[256];
        unsigned long evbits = 0;
        int fd;
        if (strncmp(entry->d_name, "event", 5) != 0) {
            continue;
        }
        if (snprintf(path, sizeof(path), "/dev/input/%s", entry->d_name) >=
            (int)sizeof(path)) {
            continue;
        }
        fd = open(path, O_RDONLY | O_NONBLOCK);
        if (fd < 0) {
            continue;
        }
        if (ioctl(fd, EVIOCGBIT(0, sizeof(evbits)), &evbits) == 0 &&
            (evbits & (1UL << EV_KEY)) != 0UL) {
            g_keyboard_fds[g_keyboard_count++] = fd;
        } else {
            close(fd);
        }
    }
    closedir(input_dir);
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_init(void)
{
    const char *wayland = getenv("WAYLAND_DISPLAY");
    const char *session = getenv("XDG_SESSION_TYPE");
    int wayland_session = (wayland != NULL && wayland[0] != '\0') ||
                          (session != NULL && strcmp(session, "wayland") == 0);
#ifdef UCOMUSE_HAVE_X11
    if (!wayland_session && getenv("DISPLAY") != NULL) {
        XInitThreads();
        g_display = XOpenDisplay(NULL);
        if (g_display != NULL) {
            int event_base;
            int error_base;
            int major = 2;
            int minor = 2;
            g_root = DefaultRootWindow(g_display);
            g_x11_screen = DefaultScreen(g_display);
            if (XTestQueryExtension(g_display, &event_base, &error_base,
                                    &major, &minor)) {
                g_use_native = 0;
                return UCOMUSE_STATUS_OK;
            }
            XCloseDisplay(g_display);
            g_display = NULL;
        }
    }
#else
    (void)wayland_session;
#endif
    g_tracked_x = 0;
    g_tracked_y = 0;
    if (ucomuse_native_init() != UCOMUSE_STATUS_OK) {
        return UCOMUSE_STATUS_ERROR;
    }
    g_use_native = 1;
    return UCOMUSE_STATUS_OK;
}

void ucomuse_platform_shutdown(void)
{
    int i;
    if (g_uinput_fd >= 0) {
        (void)ioctl(g_uinput_fd, UI_DEV_DESTROY);
        close(g_uinput_fd);
        g_uinput_fd = -1;
    }
    for (i = 0; i < g_keyboard_count; ++i) {
        close(g_keyboard_fds[i]);
        g_keyboard_fds[i] = -1;
    }
    g_keyboard_count = 0;
    g_tracked_x = 0;
    g_tracked_y = 0;
#ifdef UCOMUSE_HAVE_X11
    if (g_display != NULL) {
        XCloseDisplay(g_display);
        g_display = NULL;
    }
#endif
}

ucomuse_status ucomuse_platform_get_position(int32_t *x, int32_t *y)
{
    if (x == NULL || y == NULL) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
#ifdef UCOMUSE_HAVE_X11
    if (!g_use_native) {
        Window root_return;
        Window child_return;
        int root_x;
        int root_y;
        int win_x;
        int win_y;
        unsigned int mask;
        if (!XQueryPointer(g_display, g_root, &root_return, &child_return,
                           &root_x, &root_y, &win_x, &win_y, &mask)) {
            return UCOMUSE_STATUS_ERROR;
        }
        *x = root_x;
        *y = root_y;
        return UCOMUSE_STATUS_OK;
    }
#endif
    *x = g_tracked_x;
    *y = g_tracked_y;
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_move_to(int32_t x, int32_t y)
{
    int abs_x;
    int abs_y;
#ifdef UCOMUSE_HAVE_X11
    if (!g_use_native) {
        if (!XTestFakeMotionEvent(g_display, g_x11_screen, x, y, CurrentTime)) {
            return UCOMUSE_STATUS_ERROR;
        }
        XFlush(g_display);
        return UCOMUSE_STATUS_OK;
    }
#endif
    if (g_uinput_fd < 0) {
        return UCOMUSE_STATUS_ERROR;
    }
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= g_screen_width) x = g_screen_width - 1;
    if (y >= g_screen_height) y = g_screen_height - 1;
    abs_x = (int)(((int64_t)x * 65535 + (g_screen_width - 1) / 2) /
                  (g_screen_width - 1));
    abs_y = (int)(((int64_t)y * 65535 + (g_screen_height - 1) / 2) /
                  (g_screen_height - 1));
    if (!ucomuse_emit(EV_ABS, ABS_X, abs_x) ||
        !ucomuse_emit(EV_ABS, ABS_Y, abs_y) || !ucomuse_emit_sync()) {
        return UCOMUSE_STATUS_ERROR;
    }
    g_tracked_x = x;
    g_tracked_y = y;
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_move_by(int32_t dx, int32_t dy)
{
#ifdef UCOMUSE_HAVE_X11
    if (!g_use_native) {
        Window root_return;
        Window child_return;
        int root_x;
        int root_y;
        int win_x;
        int win_y;
        int64_t next_x;
        int64_t next_y;
        unsigned int mask;
        if (!XQueryPointer(g_display, g_root, &root_return, &child_return,
                           &root_x, &root_y, &win_x, &win_y, &mask)) {
            return UCOMUSE_STATUS_ERROR;
        }
        next_x = (int64_t)root_x + dx;
        next_y = (int64_t)root_y + dy;
        if (next_x < INT32_MIN || next_x > INT32_MAX ||
            next_y < INT32_MIN || next_y > INT32_MAX) {
            return UCOMUSE_STATUS_INVALID_ARGUMENT;
        }
        if (!XTestFakeMotionEvent(g_display, g_x11_screen, (int)next_x,
                                  (int)next_y, CurrentTime)) {
            return UCOMUSE_STATUS_ERROR;
        }
        XFlush(g_display);
        return UCOMUSE_STATUS_OK;
    }
#endif
    if (g_uinput_fd < 0) {
        return UCOMUSE_STATUS_ERROR;
    }
    if ((dx != 0 && !ucomuse_emit(EV_REL, REL_X, dx)) ||
        (dy != 0 && !ucomuse_emit(EV_REL, REL_Y, dy)) ||
        ((dx != 0 || dy != 0) && !ucomuse_emit_sync())) {
        return UCOMUSE_STATUS_ERROR;
    }
    if ((int64_t)g_tracked_x + dx < INT32_MIN) {
        g_tracked_x = INT32_MIN;
    } else if ((int64_t)g_tracked_x + dx > INT32_MAX) {
        g_tracked_x = INT32_MAX;
    } else {
        g_tracked_x += dx;
    }
    if ((int64_t)g_tracked_y + dy < INT32_MIN) {
        g_tracked_y = INT32_MIN;
    } else if ((int64_t)g_tracked_y + dy > INT32_MAX) {
        g_tracked_y = INT32_MAX;
    } else {
        g_tracked_y += dy;
    }
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_button(ucomuse_mouse_button button, int down)
{
    static const int evdev_buttons[] = {
        BTN_LEFT, BTN_RIGHT, BTN_MIDDLE, BTN_SIDE, BTN_EXTRA
    };
#ifdef UCOMUSE_HAVE_X11
    if (!g_use_native) {
        static const unsigned int x_buttons[] = {1, 3, 2, 8, 9};
        if (!XTestFakeButtonEvent(g_display, x_buttons[button], down, CurrentTime)) {
            return UCOMUSE_STATUS_ERROR;
        }
        XFlush(g_display);
        return UCOMUSE_STATUS_OK;
    }
#endif
    return ucomuse_emit(EV_KEY, evdev_buttons[button], down ? 1 : 0) &&
                   ucomuse_emit_sync()
        ? UCOMUSE_STATUS_OK
        : UCOMUSE_STATUS_ERROR;
}

ucomuse_status ucomuse_platform_wheel(int delta)
{
#ifdef UCOMUSE_HAVE_X11
    if (!g_use_native) {
        unsigned int button = delta > 0 ? 4U : 5U;
        int i;
        for (i = 0; i < abs(delta); ++i) {
            if (!XTestFakeButtonEvent(g_display, button, True, CurrentTime) ||
                !XTestFakeButtonEvent(g_display, button, False, CurrentTime)) {
                return UCOMUSE_STATUS_ERROR;
            }
        }
        XFlush(g_display);
        return UCOMUSE_STATUS_OK;
    }
#endif
    return ucomuse_emit(EV_REL, REL_WHEEL, delta) && ucomuse_emit_sync()
        ? UCOMUSE_STATUS_OK
        : UCOMUSE_STATUS_ERROR;
}

ucomuse_status ucomuse_platform_key_pressed(ucomuse_key key, int *pressed)
{
    if (pressed == NULL) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
#ifdef UCOMUSE_HAVE_X11
    if (!g_use_native) {
        return ucomuse_x11_key_pressed(key, pressed);
    }
#endif
    return ucomuse_native_key_pressed(key, pressed);
}

ucomuse_status ucomuse_platform_get_key(ucomuse_key *key)
{
    int value;
    int queried = 0;
    if (key == NULL) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
    for (value = 1; value <= UCOMUSE_KEY_MENU; ++value) {
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
