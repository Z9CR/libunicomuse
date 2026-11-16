#include "internal.h"

#include <limits.h>
#include <math.h>
#include <stddef.h>

static int g_library_initialized = 0;

ucomuse_status ucomuse_init(void)
{
    ucomuse_status status;

    if (g_library_initialized) {
        return UCOMUSE_STATUS_OK;
    }
    status = ucomuse_platform_init();
    if (status == UCOMUSE_STATUS_OK) {
        g_library_initialized = 1;
    }
    return status;
}

void ucomuse_shutdown(void)
{
    if (g_library_initialized) {
        ucomuse_platform_shutdown();
        g_library_initialized = 0;
    }
}

ucomuse_status ucomuse_mouse_move_to(int32_t x, int32_t y, uint32_t time_ms)
{
    int32_t start_x;
    int32_t start_y;
    uint32_t frames;
    uint32_t i;
    ucomuse_status status;

    if (!g_library_initialized) {
        return UCOMUSE_STATUS_NOT_INITIALIZED;
    }
    if (time_ms == 0) {
        return ucomuse_platform_move_to(x, y);
    }
    status = ucomuse_platform_get_position(&start_x, &start_y);
    if (status != UCOMUSE_STATUS_OK) {
        return status;
    }
    frames = time_ms / 4U;
    if (frames == 0U) {
        frames = 1U;
    }
    for (i = 1U; i <= frames; ++i) {
        double t = (double)i / (double)frames;
        int32_t next_x = (int32_t)((double)start_x +
                                   ((double)x - start_x) * t);
        int32_t next_y = (int32_t)((double)start_y +
                                   ((double)y - start_y) * t);
        status = ucomuse_platform_move_to(next_x, next_y);
        if (status != UCOMUSE_STATUS_OK) {
            return status;
        }
        ucomuse_platform_sleep(4U);
    }
    return ucomuse_platform_move_to(x, y);
}

ucomuse_status ucomuse_mouse_translate(double angle_degrees, int32_t distance,
                                       uint32_t time_ms)
{
    double dx;
    double dy;
    double rounded_dx;
    double rounded_dy;
    double previous_x = 0.0;
    double previous_y = 0.0;
    uint32_t frames;
    uint32_t i;
    const double radians = angle_degrees * 3.14159265358979323846 / 180.0;

    if (!g_library_initialized) {
        return UCOMUSE_STATUS_NOT_INITIALIZED;
    }
    if (!isfinite(angle_degrees)) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
    dx = cos(radians) * distance;
    dy = sin(radians) * distance;
    rounded_dx = round(dx);
    rounded_dy = round(dy);
    if (rounded_dx < (double)INT32_MIN || rounded_dx > (double)INT32_MAX ||
        rounded_dy < (double)INT32_MIN || rounded_dy > (double)INT32_MAX) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
    if (time_ms == 0U) {
        return ucomuse_platform_move_by((int32_t)rounded_dx,
                                        (int32_t)rounded_dy);
    }
    frames = time_ms / 4U;
    if (frames == 0U) {
        frames = 1U;
    }
    for (i = 1U; i <= frames; ++i) {
        double current_x =
            round(rounded_dx * (double)i / (double)frames);
        double current_y =
            round(rounded_dy * (double)i / (double)frames);
        int32_t step_x = (int32_t)(current_x - previous_x);
        int32_t step_y = (int32_t)(current_y - previous_y);
        ucomuse_status status = ucomuse_platform_move_by(step_x, step_y);
        if (status != UCOMUSE_STATUS_OK) {
            return status;
        }
        previous_x = current_x;
        previous_y = current_y;
        ucomuse_platform_sleep(4U);
    }
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_mouse_click(ucomuse_mouse_button button)
{
    ucomuse_status status = ucomuse_mouse_press(button);
    if (status != UCOMUSE_STATUS_OK) {
        return status;
    }
    return ucomuse_mouse_release(button);
}

ucomuse_status ucomuse_mouse_press(ucomuse_mouse_button button)
{
    if (!g_library_initialized) {
        return UCOMUSE_STATUS_NOT_INITIALIZED;
    }
    if (button < UCOMUSE_MOUSE_LEFT || button > UCOMUSE_MOUSE_X2) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
    return ucomuse_platform_button(button, 1);
}

ucomuse_status ucomuse_mouse_release(ucomuse_mouse_button button)
{
    if (!g_library_initialized) {
        return UCOMUSE_STATUS_NOT_INITIALIZED;
    }
    if (button < UCOMUSE_MOUSE_LEFT || button > UCOMUSE_MOUSE_X2) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
    return ucomuse_platform_button(button, 0);
}

ucomuse_status ucomuse_mouse_wheel(ucomuse_wheel_direction direction,
                                   double notches)
{
    int delta;
    if (!g_library_initialized) {
        return UCOMUSE_STATUS_NOT_INITIALIZED;
    }
    if ((direction != UCOMUSE_WHEEL_UP && direction != UCOMUSE_WHEEL_DOWN) ||
        !isfinite(notches) || notches < 0.0 ||
        notches > (double)INT_MAX) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
    if (notches == 0.0) {
        return UCOMUSE_STATUS_OK;
    }
    delta = (int)round(notches);
    if (delta == 0) {
        delta = notches > 0.0 ? 1 : -1;
    }
    if (direction == UCOMUSE_WHEEL_DOWN) {
        delta = -delta;
    }
    return ucomuse_platform_wheel(delta);
}

ucomuse_status ucomuse_key_get_pressed(ucomuse_key *key)
{
    if (!g_library_initialized) {
        return UCOMUSE_STATUS_NOT_INITIALIZED;
    }
    if (key == NULL) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
    return ucomuse_platform_get_key(key);
}

ucomuse_status ucomuse_key_is_pressed(ucomuse_key key, int *pressed)
{
    if (!g_library_initialized) {
        return UCOMUSE_STATUS_NOT_INITIALIZED;
    }
    if (pressed == NULL || key <= UCOMUSE_KEY_NONE) {
        return UCOMUSE_STATUS_INVALID_ARGUMENT;
    }
    return ucomuse_platform_key_pressed(key, pressed);
}
