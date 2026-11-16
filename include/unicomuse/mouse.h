#ifndef UNICOMUSE_MOUSE_H
#define UNICOMUSE_MOUSE_H

#include <stdint.h>
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ucomuse_mouse_button {
    UCOMUSE_MOUSE_LEFT = 0,
    UCOMUSE_MOUSE_RIGHT = 1,
    UCOMUSE_MOUSE_MIDDLE = 2,
    UCOMUSE_MOUSE_X1 = 3,
    UCOMUSE_MOUSE_X2 = 4
} ucomuse_mouse_button;

typedef enum ucomuse_wheel_direction {
    UCOMUSE_WHEEL_UP = 0,
    UCOMUSE_WHEEL_DOWN = 1
} ucomuse_wheel_direction;

/* Move to screen coordinates (origin at the primary display's top-left).
 * time_ms == 0 performs an immediate move; otherwise movement is smoothed.
 */
ucomuse_status ucomuse_mouse_move_to(int32_t x, int32_t y,
                                     uint32_t time_ms);
ucomuse_status ucomuse_mouse_translate(double angle_degrees, int32_t distance,
                                       uint32_t time_ms);
ucomuse_status ucomuse_mouse_click(ucomuse_mouse_button button);
ucomuse_status ucomuse_mouse_press(ucomuse_mouse_button button);
ucomuse_status ucomuse_mouse_release(ucomuse_mouse_button button);
ucomuse_status ucomuse_mouse_wheel(ucomuse_wheel_direction direction,
                                   double notches);

#ifdef __cplusplus
}
#endif

#endif
