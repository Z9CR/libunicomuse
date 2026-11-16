#ifndef UNICOMUSE_COMMON_H
#define UNICOMUSE_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ucomuse_status {
    UCOMUSE_STATUS_OK = 0,
    UCOMUSE_STATUS_ERROR = 1,
    UCOMUSE_STATUS_NOT_INITIALIZED = 2,
    UCOMUSE_STATUS_INVALID_ARGUMENT = 3,
    UCOMUSE_STATUS_NOT_SUPPORTED = 4
} ucomuse_status;

/* Initialize the OS input backend. Call before any mouse or key operation.
 * The call may be repeated after shutdown; repeated calls while initialized
 * succeed without acquiring extra resources.
 */
ucomuse_status ucomuse_init(void);

/* Close OS handles acquired by ucomuse_init(). Safe to call more than once.
 */
void ucomuse_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
