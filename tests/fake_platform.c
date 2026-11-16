#include "fake_platform.h"
#include "internal.h"

int32_t fake_x;
int32_t fake_y;
int fake_button;
int fake_button_down;
int fake_button_calls;
int fake_wheel;
int fake_sleep_count;
ucomuse_key fake_key = UCOMUSE_KEY_NONE;

ucomuse_status ucomuse_platform_init(void)
{
    fake_x = 0;
    fake_y = 0;
    fake_button_calls = 0;
    return UCOMUSE_STATUS_OK;
}

void ucomuse_platform_shutdown(void)
{
}

ucomuse_status ucomuse_platform_get_position(int32_t *x, int32_t *y)
{
    *x = fake_x;
    *y = fake_y;
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_move_to(int32_t x, int32_t y)
{
    fake_x = x;
    fake_y = y;
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_move_by(int32_t dx, int32_t dy)
{
    fake_x += dx;
    fake_y += dy;
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_button(ucomuse_mouse_button button, int down)
{
    fake_button = button;
    fake_button_down = down;
    ++fake_button_calls;
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_wheel(int delta)
{
    fake_wheel = delta;
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_key_pressed(ucomuse_key key, int *pressed)
{
    *pressed = key == fake_key;
    return UCOMUSE_STATUS_OK;
}

ucomuse_status ucomuse_platform_get_key(ucomuse_key *key)
{
    *key = fake_key;
    return UCOMUSE_STATUS_OK;
}

void ucomuse_platform_sleep(uint32_t milliseconds)
{
    if (milliseconds > 0U) {
        ++fake_sleep_count;
    }
}
