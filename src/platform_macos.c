#define _POSIX_C_SOURCE 199309L
#include "internal.h"

#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <limits.h>
#include <math.h>
#include <time.h>

static int g_initialized;

static CGKeyCode ucomuse_mac_keycode(ucomuse_key key)
{
    static const CGKeyCode letters[] = {
        0x00, 0x0B, 0x08, 0x02, 0x0E, 0x03, 0x05, 0x04, 0x22, 0x26, 0x28,
        0x25, 0x2E, 0x2D, 0x1F, 0x23, 0x0C, 0x0F, 0x01, 0x11, 0x20, 0x09,
        0x0D, 0x07, 0x10, 0x06
    };
    static const CGKeyCode digits[] = {
        0x1D, 0x12, 0x13, 0x14, 0x15, 0x17, 0x16, 0x1A, 0x1C, 0x19
    };
    if (key >= UCOMUSE_KEY_A && key <= UCOMUSE_KEY_Z) {
        return letters[key - UCOMUSE_KEY_A];
    }
    if (key >= UCOMUSE_KEY_0 && key <= UCOMUSE_KEY_9) {
        return digits[key == UCOMUSE_KEY_0 ? 0 : key - UCOMUSE_KEY_1 + 1];
    }
    if (key >= UCOMUSE_KEY_F1 && key <= UCOMUSE_KEY_F12) {
        static const CGKeyCode fkeys[] = {
            0x7A, 0x78, 0x63, 0x76, 0x60, 0x61, 0x62, 0x64, 0x65, 0x6D,
            0x67, 0x6F
        };
        return fkeys[key - UCOMUSE_KEY_F1];
    }
    if (key >= UCOMUSE_KEY_KP_0 && key <= UCOMUSE_KEY_KP_9) {
        static const CGKeyCode keypad[] = {
            0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5B, 0x5C
        };
        return keypad[key - UCOMUSE_KEY_KP_0];
    }
    switch (key) {
    case UCOMUSE_KEY_APOSTROPHE: return 0x27;
    case UCOMUSE_KEY_COMMA: return 0x2B;
    case UCOMUSE_KEY_MINUS: return 0x1B;
    case UCOMUSE_KEY_PERIOD: return 0x2F;
    case UCOMUSE_KEY_SLASH: return 0x2C;
    case UCOMUSE_KEY_SEMICOLON: return 0x29;
    case UCOMUSE_KEY_EQUAL: return 0x18;
    case UCOMUSE_KEY_LEFT_BRACKET: return 0x21;
    case UCOMUSE_KEY_BACKSLASH: return 0x2A;
    case UCOMUSE_KEY_RIGHT_BRACKET: return 0x1E;
    case UCOMUSE_KEY_GRAVE: return 0x32;
    case UCOMUSE_KEY_SPACE: return 0x31;
    case UCOMUSE_KEY_LEFT_SHIFT: return 0x38;
    case UCOMUSE_KEY_RIGHT_SHIFT: return 0x3C;
    case UCOMUSE_KEY_LEFT_CONTROL: return 0x3B;
    case UCOMUSE_KEY_RIGHT_CONTROL: return 0x3E;
    case UCOMUSE_KEY_LEFT_ALT: return 0x3A;
    case UCOMUSE_KEY_RIGHT_ALT: return 0x3D;
    case UCOMUSE_KEY_LEFT_SUPER: return 0x37;
    case UCOMUSE_KEY_RIGHT_SUPER: return 0x36;
    case UCOMUSE_KEY_ESCAPE: return 0x35;
    case UCOMUSE_KEY_ENTER: return 0x24;
    case UCOMUSE_KEY_TAB: return 0x30;
    case UCOMUSE_KEY_BACKSPACE: return 0x33;
    case UCOMUSE_KEY_INSERT: return 0x72;
    case UCOMUSE_KEY_DELETE: return 0x75;
    case UCOMUSE_KEY_RIGHT: return 0x7C;
    case UCOMUSE_KEY_LEFT: return 0x7B;
    case UCOMUSE_KEY_DOWN: return 0x7D;
    case UCOMUSE_KEY_UP: return 0x7E;
    case UCOMUSE_KEY_PAGE_UP: return 0x74;
    case UCOMUSE_KEY_PAGE_DOWN: return 0x79;
    case UCOMUSE_KEY_HOME: return 0x73;
    case UCOMUSE_KEY_END: return 0x77;
    case UCOMUSE_KEY_CAPS_LOCK: return 0x39;
    case UCOMUSE_KEY_SCROLL_LOCK: return 0x6B;
    case UCOMUSE_KEY_NUM_LOCK: return 0x47;
    case UCOMUSE_KEY_PRINT_SCREEN: return 0x69;
    case UCOMUSE_KEY_PAUSE: return 0x71;
    case UCOMUSE_KEY_KP_DECIMAL: return 0x41;
    case UCOMUSE_KEY_KP_DIVIDE: return 0x4B;
    case UCOMUSE_KEY_KP_MULTIPLY: return 0x43;
    case UCOMUSE_KEY_KP_SUBTRACT: return 0x4E;
    case UCOMUSE_KEY_KP_ADD: return 0x45;
    case UCOMUSE_KEY_KP_ENTER: return 0x4C;
    case UCOMUSE_KEY_KP_EQUAL: return 0x51;
    default: return (CGKeyCode)0xFFFF;
    }
}

static ucomuse_status ucomuse_cursor(CGPoint *point)
{
    CGEventRef event = CGEventCreate(NULL);
    if (point == NULL) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
    if (event == NULL) {
        return UCOMUSE_STATUS_ERROR;
    }
    *point = CGEventGetLocation(event);
    CFRelease(event);
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_init(void)
{
    CFMutableDictionaryRef options;

    if (AXIsProcessTrusted()) {
        g_initialized = 1;
        return UCOMUSE_STATUS_OK;
    }

    options = CFDictionaryCreateMutable(
        kCFAllocatorDefault, 1, &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks);
    if (options == NULL) {
        return UCOMUSE_STATUS_ERROR;
    }
    CFDictionarySetValue(options, kAXTrustedCheckOptionPrompt, kCFBooleanTrue);
    (void)AXIsProcessTrustedWithOptions(options);
    CFRelease(options);

    if (!AXIsProcessTrusted()) {
        return UCOMUSE_STATUS_ERROR;
    }
    g_initialized = 1;
    return UCOMUSE_STATUS_OK;
}

void ucomuse_platform_shutdown(void)
{
    g_initialized = 0;
}

ucomuse_status ucomuse_platform_get_position(int32_t *x, int32_t *y)
{
    CGPoint point;
    if (!g_initialized) {
        return UCOMUSE_STATUS_NOT_INITIALIZED;
    }
    if (x == NULL || y == NULL) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
    if (ucomuse_cursor(&point) != UCOMUSE_STATUS_OK) {
        return UCOMUSE_STATUS_ERROR;
    }
    *x = (int32_t)point.x;
    *y = (int32_t)point.y;
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_move_to(int32_t x, int32_t y)
{
    CGEventRef event;
    if (!g_initialized) {
        return UCOMUSE_STATUS_NOT_INITIALIZED;
    }
    event = CGEventCreateMouseEvent(NULL, kCGEventMouseMoved,
                                    CGPointMake(x, y), kCGMouseButtonLeft);
    if (event == NULL) {
        return UCOMUSE_STATUS_ERROR;
    }
    CGEventPost(kCGHIDEventTap, event);
    CFRelease(event);
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_move_by(int32_t dx, int32_t dy)
{
    int32_t current_x;
    int32_t current_y;
    int64_t x;
    int64_t y;
    if (!g_initialized) {
        return UCOMUSE_STATUS_NOT_INITIALIZED;
    }
    if (ucomuse_platform_get_position(&current_x, &current_y) !=
        UCOMUSE_STATUS_OK) {
        return UCOMUSE_STATUS_ERROR;
    }
    x = (int64_t)current_x + dx;
    y = (int64_t)current_y + dy;
    if (x < INT32_MIN || x > INT32_MAX || y < INT32_MIN || y > INT32_MAX) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
    return ucomuse_platform_move_to((int32_t)x, (int32_t)y);
}

ucomuse_status ucomuse_platform_button(ucomuse_mouse_button button, int down)
{
    static const CGMouseButton buttons[] = {
        kCGMouseButtonLeft, kCGMouseButtonRight, kCGMouseButtonCenter,
        (CGMouseButton)3, (CGMouseButton)4
    };
    CGEventType type;
    CGEventRef event;
    CGPoint point;
    if (!g_initialized) {
        return UCOMUSE_STATUS_NOT_INITIALIZED;
    }
    if (button == UCOMUSE_MOUSE_LEFT) {
        type = down ? kCGEventLeftMouseDown : kCGEventLeftMouseUp;
    } else if (button == UCOMUSE_MOUSE_RIGHT) {
        type = down ? kCGEventRightMouseDown : kCGEventRightMouseUp;
    } else {
        type = down ? kCGEventOtherMouseDown : kCGEventOtherMouseUp;
    }
    if (ucomuse_cursor(&point) != UCOMUSE_STATUS_OK) {
        return UCOMUSE_STATUS_ERROR;
    }
    event = CGEventCreateMouseEvent(NULL, type, point,
                                    buttons[button]);
    if (event == NULL) {
        return UCOMUSE_STATUS_ERROR;
    }
    CGEventPost(kCGHIDEventTap, event);
    CFRelease(event);
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_wheel(int delta)
{
    CGEventRef event;
    if (!g_initialized) {
        return UCOMUSE_STATUS_NOT_INITIALIZED;
    }
    event = CGEventCreateScrollWheelEvent(NULL, kCGScrollEventUnitLine, 1,
                                           delta);
    if (event == NULL) {
        return UCOMUSE_STATUS_ERROR;
    }
    CGEventPost(kCGHIDEventTap, event);
    CFRelease(event);
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_key_pressed(ucomuse_key key, int *pressed)
{
    CGKeyCode code;
    if (!g_initialized) {
        return UCOMUSE_STATUS_NOT_INITIALIZED;
    }
    if (pressed == NULL) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
    code = ucomuse_mac_keycode(key);
    if (code == (CGKeyCode)0xFFFF) {
        return UCOMUSE_STATUS_NOT_SUPPORTED;
    }
    *pressed = CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState,
                                     code) ? 1 : 0;
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
