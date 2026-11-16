#ifndef UNICOMUSE_INTERNAL_H
#define UNICOMUSE_INTERNAL_H

#include <stdint.h>
#include <unicomuse/key.h>
#include <unicomuse/mouse.h>

ucomuse_status ucomuse_platform_init(void);
void ucomuse_platform_shutdown(void);
ucomuse_status ucomuse_platform_get_position(int32_t *x, int32_t *y);
ucomuse_status ucomuse_platform_move_to(int32_t x, int32_t y);
ucomuse_status ucomuse_platform_move_by(int32_t dx, int32_t dy);
ucomuse_status ucomuse_platform_button(ucomuse_mouse_button button, int down);
ucomuse_status ucomuse_platform_wheel(int delta);
ucomuse_status ucomuse_platform_key_pressed(ucomuse_key key, int *pressed);
ucomuse_status ucomuse_platform_get_key(ucomuse_key *key);
void ucomuse_platform_sleep(uint32_t milliseconds);

#endif
