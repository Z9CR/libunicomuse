#include "fake_platform.h"

#include <math.h>
#include <stddef.h>
#include <stdint.h>

int main(void)
{
    ucomuse_key key = UCOMUSE_KEY_NONE;
    int pressed = 0;

    if (ucomuse_mouse_move_to(10, 20, 0) !=
        UCOMUSE_STATUS_NOT_INITIALIZED) {
        return 1;
    }
    if (ucomuse_init() != UCOMUSE_STATUS_OK) {
        return 2;
    }
    if (ucomuse_mouse_move_to(10, 20, 0) != UCOMUSE_STATUS_OK ||
        fake_x != 10 || fake_y != 20) {
        return 3;
    }
    if (ucomuse_mouse_translate(0.0, 5, 0) != UCOMUSE_STATUS_OK ||
        fake_x != 15 || fake_y != 20) {
        return 4;
    }
    if (ucomuse_mouse_translate(NAN, 1, 0) !=
        UCOMUSE_STATUS_INVALID_ARGUMENT) {
        return 5;
    }
    if (ucomuse_mouse_translate(180.0, INT32_MIN, 0) !=
        UCOMUSE_STATUS_INVALID_ARGUMENT) {
        return 6;
    }
    if (ucomuse_mouse_move_to(35, 40, 8) != UCOMUSE_STATUS_OK ||
        fake_x != 35 || fake_y != 40 || fake_sleep_count != 2) {
        return 7;
    }
    if (ucomuse_mouse_wheel(UCOMUSE_WHEEL_UP, -1.0) !=
        UCOMUSE_STATUS_INVALID_ARGUMENT) {
        return 8;
    }
    if (ucomuse_mouse_wheel(UCOMUSE_WHEEL_DOWN, 2.0) !=
            UCOMUSE_STATUS_OK ||
        fake_wheel != -2) {
        return 9;
    }
    if (ucomuse_mouse_wheel(UCOMUSE_WHEEL_UP,
                            (double)UCOMUSE_MAX_WHEEL_NOTCHES) !=
            UCOMUSE_STATUS_OK ||
        fake_wheel != UCOMUSE_MAX_WHEEL_NOTCHES) {
        return 10;
    }
    if (ucomuse_mouse_wheel(UCOMUSE_WHEEL_UP,
                            (double)UCOMUSE_MAX_WHEEL_NOTCHES + 1.0) !=
            UCOMUSE_STATUS_INVALID_ARGUMENT ||
        fake_wheel != UCOMUSE_MAX_WHEEL_NOTCHES) {
        return 11;
    }
    if (ucomuse_mouse_click(UCOMUSE_MOUSE_X1) != UCOMUSE_STATUS_OK ||
        fake_button != UCOMUSE_MOUSE_X1 || fake_button_down != 0 ||
        fake_button_calls != 2) {
        return 12;
    }
    fake_key = UCOMUSE_KEY_LEFT_CONTROL;
    if (ucomuse_key_is_pressed(fake_key, &pressed) != UCOMUSE_STATUS_OK ||
        !pressed ||
        ucomuse_key_get_pressed(&key) != UCOMUSE_STATUS_OK ||
        key != fake_key) {
        return 13;
    }
    if (ucomuse_key_is_pressed(fake_key, NULL) !=
        UCOMUSE_STATUS_INVALID_ARGUMENT) {
        return 14;
    }
    pressed = 1;
    if (ucomuse_key_is_pressed((ucomuse_key)33, &pressed) !=
            UCOMUSE_STATUS_INVALID_ARGUMENT ||
        pressed != 1) {
        return 15;
    }
    ucomuse_shutdown();
    if (ucomuse_key_get_pressed(&key) != UCOMUSE_STATUS_NOT_INITIALIZED) {
        return 16;
    }
    return 0;
}
