#ifndef UNICOMUSE_KEY_H
#define UNICOMUSE_KEY_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Key values follow the GLFW key numbering convention where practical. */
typedef enum ucomuse_key {
    UCOMUSE_KEY_NONE = 0,
    UCOMUSE_KEY_SPACE = 32,
    UCOMUSE_KEY_APOSTROPHE = 39,
    UCOMUSE_KEY_COMMA = 44,
    UCOMUSE_KEY_MINUS = 45,
    UCOMUSE_KEY_PERIOD = 46,
    UCOMUSE_KEY_SLASH = 47,
    UCOMUSE_KEY_0 = 48,
    UCOMUSE_KEY_1 = 49,
    UCOMUSE_KEY_2 = 50,
    UCOMUSE_KEY_3 = 51,
    UCOMUSE_KEY_4 = 52,
    UCOMUSE_KEY_5 = 53,
    UCOMUSE_KEY_6 = 54,
    UCOMUSE_KEY_7 = 55,
    UCOMUSE_KEY_8 = 56,
    UCOMUSE_KEY_9 = 57,
    UCOMUSE_KEY_SEMICOLON = 59,
    UCOMUSE_KEY_EQUAL = 61,
    UCOMUSE_KEY_A = 65,
    UCOMUSE_KEY_B = 66,
    UCOMUSE_KEY_C = 67,
    UCOMUSE_KEY_D = 68,
    UCOMUSE_KEY_E = 69,
    UCOMUSE_KEY_F = 70,
    UCOMUSE_KEY_G = 71,
    UCOMUSE_KEY_H = 72,
    UCOMUSE_KEY_I = 73,
    UCOMUSE_KEY_J = 74,
    UCOMUSE_KEY_K = 75,
    UCOMUSE_KEY_L = 76,
    UCOMUSE_KEY_M = 77,
    UCOMUSE_KEY_N = 78,
    UCOMUSE_KEY_O = 79,
    UCOMUSE_KEY_P = 80,
    UCOMUSE_KEY_Q = 81,
    UCOMUSE_KEY_R = 82,
    UCOMUSE_KEY_S = 83,
    UCOMUSE_KEY_T = 84,
    UCOMUSE_KEY_U = 85,
    UCOMUSE_KEY_V = 86,
    UCOMUSE_KEY_W = 87,
    UCOMUSE_KEY_X = 88,
    UCOMUSE_KEY_Y = 89,
    UCOMUSE_KEY_Z = 90,
    UCOMUSE_KEY_LEFT_BRACKET = 91,
    UCOMUSE_KEY_BACKSLASH = 92,
    UCOMUSE_KEY_RIGHT_BRACKET = 93,
    UCOMUSE_KEY_GRAVE = 96,
    UCOMUSE_KEY_ESCAPE = 256,
    UCOMUSE_KEY_ENTER = 257,
    UCOMUSE_KEY_TAB = 258,
    UCOMUSE_KEY_BACKSPACE = 259,
    UCOMUSE_KEY_INSERT = 260,
    UCOMUSE_KEY_DELETE = 261,
    UCOMUSE_KEY_RIGHT = 262,
    UCOMUSE_KEY_LEFT = 263,
    UCOMUSE_KEY_DOWN = 264,
    UCOMUSE_KEY_UP = 265,
    UCOMUSE_KEY_PAGE_UP = 266,
    UCOMUSE_KEY_PAGE_DOWN = 267,
    UCOMUSE_KEY_HOME = 268,
    UCOMUSE_KEY_END = 269,
    UCOMUSE_KEY_CAPS_LOCK = 280,
    UCOMUSE_KEY_SCROLL_LOCK = 281,
    UCOMUSE_KEY_NUM_LOCK = 282,
    UCOMUSE_KEY_PRINT_SCREEN = 283,
    UCOMUSE_KEY_PAUSE = 284,
    UCOMUSE_KEY_F1 = 290,
    UCOMUSE_KEY_F2 = 291,
    UCOMUSE_KEY_F3 = 292,
    UCOMUSE_KEY_F4 = 293,
    UCOMUSE_KEY_F5 = 294,
    UCOMUSE_KEY_F6 = 295,
    UCOMUSE_KEY_F7 = 296,
    UCOMUSE_KEY_F8 = 297,
    UCOMUSE_KEY_F9 = 298,
    UCOMUSE_KEY_F10 = 299,
    UCOMUSE_KEY_F11 = 300,
    UCOMUSE_KEY_F12 = 301,
    UCOMUSE_KEY_KP_0 = 320,
    UCOMUSE_KEY_KP_1 = 321,
    UCOMUSE_KEY_KP_2 = 322,
    UCOMUSE_KEY_KP_3 = 323,
    UCOMUSE_KEY_KP_4 = 324,
    UCOMUSE_KEY_KP_5 = 325,
    UCOMUSE_KEY_KP_6 = 326,
    UCOMUSE_KEY_KP_7 = 327,
    UCOMUSE_KEY_KP_8 = 328,
    UCOMUSE_KEY_KP_9 = 329,
    UCOMUSE_KEY_KP_DECIMAL = 330,
    UCOMUSE_KEY_KP_DIVIDE = 331,
    UCOMUSE_KEY_KP_MULTIPLY = 332,
    UCOMUSE_KEY_KP_SUBTRACT = 333,
    UCOMUSE_KEY_KP_ADD = 334,
    UCOMUSE_KEY_KP_ENTER = 335,
    UCOMUSE_KEY_KP_EQUAL = 336,
    UCOMUSE_KEY_LEFT_SHIFT = 340,
    UCOMUSE_KEY_LEFT_CONTROL = 341,
    UCOMUSE_KEY_LEFT_ALT = 342,
    UCOMUSE_KEY_LEFT_SUPER = 343,
    UCOMUSE_KEY_RIGHT_SHIFT = 344,
    UCOMUSE_KEY_RIGHT_CONTROL = 345,
    UCOMUSE_KEY_RIGHT_ALT = 346,
    UCOMUSE_KEY_RIGHT_SUPER = 347,
    UCOMUSE_KEY_MENU = 348
} ucomuse_key;

/* Reports one currently held key, or UCOMUSE_KEY_NONE if none are held.
 * When multiple keys are held, the returned key follows a backend-defined
 * scan order. `key` must not be NULL.
 */
ucomuse_status ucomuse_key_get_pressed(ucomuse_key *key);

/* Sets *pressed to 1 if key is currently held, otherwise 0.
 * `pressed` must not be NULL.
 */
ucomuse_status ucomuse_key_is_pressed(ucomuse_key key, int *pressed);

#ifdef __cplusplus
}
#endif

#endif
