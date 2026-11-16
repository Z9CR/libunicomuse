#include <stdio.h>

#include <unicomuse/key.h>
#include <unicomuse/mouse.h>

static int report_error(const char *operation, ucomuse_status status)
{
    if (status == UCOMUSE_STATUS_OK) {
        return 0;
    }
    fprintf(stderr, "%s failed (status %d)\n", operation, (int)status);
    return 1;
}

int main(void)
{
    ucomuse_key key;
    int pressed;
    int result = 1;

    if (report_error("ucomuse_init", ucomuse_init())) {
        return result;
    }

    if (report_error("ucomuse_mouse_move_to",
                     ucomuse_mouse_move_to(500, 300, 250)) ||
        report_error("ucomuse_mouse_click",
                     ucomuse_mouse_click(UCOMUSE_MOUSE_LEFT)) ||
        report_error("ucomuse_mouse_wheel",
                     ucomuse_mouse_wheel(UCOMUSE_WHEEL_UP, 1.0))) {
        goto cleanup;
    }

    if (report_error("ucomuse_key_get_pressed",
                     ucomuse_key_get_pressed(&key)) ||
        report_error("ucomuse_key_is_pressed",
                     ucomuse_key_is_pressed(UCOMUSE_KEY_LEFT_CONTROL,
                                            &pressed))) {
        goto cleanup;
    }

    printf("A currently held key: %d; left Ctrl pressed: %s\n", (int)key,
           pressed ? "yes" : "no");
    result = 0;

cleanup:
    ucomuse_shutdown();
    return result;
}
