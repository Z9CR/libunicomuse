#ifndef UNICOMUSE_TEST_FAKE_PLATFORM_H
#define UNICOMUSE_TEST_FAKE_PLATFORM_H

#include <stdint.h>
#include <unicomuse/key.h>
#include <unicomuse/mouse.h>

extern int32_t fake_x;
extern int32_t fake_y;
extern int fake_button;
extern int fake_button_down;
extern int fake_button_calls;
extern int fake_wheel;
extern int fake_sleep_count;
extern ucomuse_key fake_key;

#endif
