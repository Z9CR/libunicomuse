# libunicomuse API Reference

This document describes the public headers under `include/unicomuse/`.
The library is implemented in ISO C99 and exposes a C ABI. The public API
does not require callers to create objects or allocate library-owned memory.

[中文版](./api.zh-CN.md)

## Headers and Linking

- `<unicomuse/common.h>`: status type and library lifecycle functions.
- `<unicomuse/mouse.h>`: mouse button and wheel types, and mouse controls.
- `<unicomuse/key.h>`: key enumeration and key-state queries.

Including `mouse.h` and/or `key.h` is normally sufficient; both include
`common.h`. Public declarations use `extern "C"` guards and can be called from
C++, but the library itself is compiled as C99.

## Essential Concepts

### Initialization and Resource Lifetime

Call `ucomuse_init()` successfully before using any mouse or keyboard
operation. The library then retains the operating-system handles or
connections required by the selected backend. Call `ucomuse_shutdown()` when
finished to close those resources. The library does not return objects,
buffers, or handles that callers must free.

Calling `ucomuse_init()` again while already initialized succeeds without
acquiring duplicate resources. `ucomuse_shutdown()` may be called repeatedly.
After shutdown, initialize again before using the library.

The library state is shared process-wide; multiple independent instances are
not supported. Callers should serialize concurrent API calls and must not call
`ucomuse_shutdown()` while another thread is using the library.

### Status Values

Every public function except `ucomuse_shutdown()` returns `ucomuse_status`:

| Value | Meaning |
| --- | --- |
| `UCOMUSE_STATUS_OK` | The operation succeeded. |
| `UCOMUSE_STATUS_ERROR` | The backend or OS operation failed, for example because a display server, input device, or event injection operation was unavailable. |
| `UCOMUSE_STATUS_NOT_INITIALIZED` | `ucomuse_init()` has not succeeded, or `ucomuse_shutdown()` has already been called. |
| `UCOMUSE_STATUS_INVALID_ARGUMENT` | An argument is invalid, such as a null output pointer, invalid button, direction, or key, or a non-finite angle or wheel amount. |
| `UCOMUSE_STATUS_NOT_SUPPORTED` | The active backend cannot query the requested key or does not support the requested capability. |

Check return values; do not treat a failed operation as successful. The
`pressed` output of a key query is valid only when the function returns
`UCOMUSE_STATUS_OK`.

### Mouse Coordinates and Movement

Absolute coordinates are in pixels: the top-left of the screen is the origin,
X increases to the right, and Y increases downward. `ucomuse_mouse_move_to()`
uses primary-display screen coordinates. Actual bounds and coordinate mapping
depend on the OS and backend. The Linux Wayland uinput backend uses the screen
size read from `/dev/fb0` when available (otherwise it falls back to
1920x1080), and cannot observe cursor movement performed by other programs.

Relative movement angles are in degrees, measured clockwise from rightward:
0° is right, 90° is down, 180° is left, and 270° is up. Distance is in pixels;
a negative distance moves in the opposite direction.

Smoothed movement is sent in steps at approximately 4 ms intervals. It is
synchronous: the call completes after all steps have been sent or an error
occurs. `time_ms == 0` requests an immediate move. A nonzero duration shorter
than one step still sends at least one step.

### Key Query Semantics

The keyboard API polls whether keys are currently held. It does not provide
key-down/key-up event streams, event history, keyboard injection, or
per-device selection. `ucomuse_key_get_pressed()` returns only one currently
held key; when multiple keys are held, the selected key depends on the
backend's scan order.

`ucomuse_key` uses GLFW-like numeric values where practical so callers can use
the same key identifiers across platforms. It represents logical keys, not
text characters. Keyboard layout, modifiers, and platform mappings can affect
the detection of punctuation and special keys. A backend may return
`UCOMUSE_STATUS_NOT_SUPPORTED` when it cannot map a key.

## Public Types

The public headers define no `struct` types. Public data types are enumerations.

### `ucomuse_status`

Returned by functions that report status. See [Status Values](#status-values)
for each constant's meaning.

### `ucomuse_mouse_button`

Button identifiers accepted by mouse-button functions:

| Constant | Meaning |
| --- | --- |
| `UCOMUSE_MOUSE_LEFT` | Left mouse button. |
| `UCOMUSE_MOUSE_RIGHT` | Right mouse button. |
| `UCOMUSE_MOUSE_MIDDLE` | Middle button / wheel button. |
| `UCOMUSE_MOUSE_X1` | Side button 1, commonly Back. |
| `UCOMUSE_MOUSE_X2` | Side button 2, commonly Forward. |

### `ucomuse_wheel_direction`

Scroll direction accepted by the wheel function:

| Constant | Meaning |
| --- | --- |
| `UCOMUSE_WHEEL_UP` | Scroll upward. |
| `UCOMUSE_WHEEL_DOWN` | Scroll downward. |

Wheel amounts must be finite and between zero and
`UCOMUSE_MAX_WHEEL_NOTCHES` (1000). A positive amount that rounds to zero is
promoted to one notch; other values are rounded to the nearest integer.

### `ucomuse_key`

Cross-platform logical key identifiers accepted by key-query functions:

| Constant(s) | Meaning |
| --- | --- |
| `UCOMUSE_KEY_NONE` | No held key was detected. Used only as the output of `get_pressed`; it is not a valid query target. |
| `UCOMUSE_KEY_SPACE` | Space bar. |
| `UCOMUSE_KEY_APOSTROPHE`, `COMMA`, `MINUS`, `PERIOD`, `SLASH` | Apostrophe, comma, minus, period, and slash punctuation. Detection depends on keyboard layout and backend mapping. |
| `UCOMUSE_KEY_0` … `UCOMUSE_KEY_9` | Top-row digit keys. |
| `UCOMUSE_KEY_SEMICOLON`, `EQUAL`, `LEFT_BRACKET`, `BACKSLASH`, `RIGHT_BRACKET`, `GRAVE` | Punctuation keys; their physical positions and mappings can vary by layout. |
| `UCOMUSE_KEY_A` … `UCOMUSE_KEY_Z` | Letter keys. |
| `UCOMUSE_KEY_ESCAPE`, `ENTER`, `TAB`, `BACKSPACE` | Escape, Enter, Tab, and Backspace. |
| `UCOMUSE_KEY_INSERT`, `DELETE` | Insert and Delete. On macOS, the key labeled Delete usually acts as Backspace; forward delete maps to `UCOMUSE_KEY_DELETE`. |
| `UCOMUSE_KEY_RIGHT`, `LEFT`, `DOWN`, `UP` | Arrow keys. |
| `UCOMUSE_KEY_PAGE_UP`, `PAGE_DOWN`, `HOME`, `END` | Page and cursor navigation keys. |
| `UCOMUSE_KEY_CAPS_LOCK`, `SCROLL_LOCK`, `NUM_LOCK`, `PRINT_SCREEN`, `PAUSE` | Lock and system keys. Some keyboards do not have a corresponding physical key. |
| `UCOMUSE_KEY_F1` … `UCOMUSE_KEY_F12` | Function keys F1 through F12. |
| `UCOMUSE_KEY_KP_0` … `UCOMUSE_KEY_KP_9` | Numeric keypad digits 0 through 9. |
| `UCOMUSE_KEY_KP_DECIMAL`, `KP_DIVIDE`, `KP_MULTIPLY`, `KP_SUBTRACT`, `KP_ADD` | Keypad decimal, divide, multiply, subtract, and add keys. |
| `UCOMUSE_KEY_KP_ENTER`, `KP_EQUAL` | Keypad Enter and equal keys. Some platforms map keypad Enter to the same system key as regular Enter. |
| `UCOMUSE_KEY_LEFT_SHIFT`, `LEFT_CONTROL`, `LEFT_ALT`, `LEFT_SUPER` | Left Shift, Control, Alt/Option, and Super/Command/Windows keys. |
| `UCOMUSE_KEY_RIGHT_SHIFT`, `RIGHT_CONTROL`, `RIGHT_ALT`, `RIGHT_SUPER` | Right Shift, Control, Alt/Option, and Super/Command/Windows keys. |
| `UCOMUSE_KEY_MENU` | Keyboard menu/application key; not every platform can map it. |

Constants in the C header all have the `UCOMUSE_KEY_` prefix. In the table,
the prefix is omitted from some grouped names for readability.

## Lifecycle Functions

### `ucomuse_status ucomuse_init(void)`

Initializes the current platform's mouse and keyboard backends.

- On success, returns `UCOMUSE_STATUS_OK`; mouse and keyboard functions can
  then be used.
- On failure, returns a backend error and leaves the library uninitialized.
  Retry after resolving the permission or session issue.
- On macOS, if Accessibility permission has not been granted, initialization
  asks macOS to show its permission prompt. After granting access, the
  application usually needs to be restarted.

The Linux Wayland native backend needs access to `/dev/uinput`, which
typically requires root or equivalent device permissions. The Linux X11
backend needs a usable `DISPLAY` and XTEST. FreeBSD requires a usable
X11/XTEST display session.

### `void ucomuse_shutdown(void)`

Closes OS handles and connections opened during initialization. It is safe to
call before initialization or more than once. It returns no status. Ensure no
other thread is using the library when calling it.

## Mouse Functions

### `ucomuse_status ucomuse_mouse_move_to(int32_t x, int32_t y, uint32_t time_ms)`

Moves the pointer to screen coordinate `(x, y)`.

- `x`, `y`: target screen coordinates in pixels.
- `time_ms`: duration in milliseconds. Zero requests an immediate move;
  nonzero values request smoothed movement.
- Returns `OK`, `NOT_INITIALIZED`, or a backend error.

Example:

```c
ucomuse_mouse_move_to(500, 300, 250);
```

### `ucomuse_status ucomuse_mouse_translate(double angle_degrees, int32_t distance, uint32_t time_ms)`

Moves the pointer relative to its current position.

- `angle_degrees`: finite angle in degrees; 0° is right, 90° is down, and
  positive angles rotate clockwise.
- `distance`: movement distance in pixels; negative values reverse direction.
- `time_ms`: movement duration, with the same meaning as in
  `ucomuse_mouse_move_to()`.
- Returns `INVALID_ARGUMENT` for a non-finite angle or if the calculated
  displacement is outside the `int32_t` range; otherwise returns the backend
  operation status.

### `ucomuse_status ucomuse_mouse_click(ucomuse_mouse_button button)`

Sends one press followed by one release for the specified button.

- `button`: a valid `ucomuse_mouse_button`.
- Returns an initialization, argument, or backend status.

If the backend reports that pressing failed, no release is sent. If pressing
succeeds but releasing fails, the function returns the release error.

### `ucomuse_status ucomuse_mouse_press(ucomuse_mouse_button button)`

Presses and holds the specified mouse button until
`ucomuse_mouse_release()` is called. Pair every press with a release; a button
left held can interfere with desktop interaction.

### `ucomuse_status ucomuse_mouse_release(ucomuse_mouse_button button)`

Releases the specified mouse button.

`ucomuse_mouse_press()` and `ucomuse_mouse_release()` accept
`UCOMUSE_MOUSE_LEFT`, `RIGHT`, `MIDDLE`, `X1`, and `X2`. Values outside this
enumeration return `UCOMUSE_STATUS_INVALID_ARGUMENT`.

### `ucomuse_status ucomuse_mouse_wheel(ucomuse_wheel_direction direction, double notches)`

Scrolls the mouse wheel.

- `direction`: `UCOMUSE_WHEEL_UP` or `UCOMUSE_WHEEL_DOWN`.
- `notches`: finite number of notches between zero and
  `UCOMUSE_MAX_WHEEL_NOTCHES` (1000). Zero sends no event. A positive value
  that rounds to zero sends one notch; other values are rounded to the nearest
  integer.
- Returns `INVALID_ARGUMENT` for an invalid direction or amount; otherwise
  returns the backend operation status.

## Keyboard Functions

### `ucomuse_status ucomuse_key_get_pressed(ucomuse_key *key)`

Queries one key that is currently held.

- `key`: required non-null output pointer.
- On success, writes `UCOMUSE_KEY_NONE` if no key is detected.
- If multiple keys are held, returns only one; the selection order is
  backend-defined.
- Returns an argument, initialization, or backend query status.

### `ucomuse_status ucomuse_key_is_pressed(ucomuse_key key, int *pressed)`

Queries whether the specified key is currently held.

- `key`: one of the defined `ucomuse_key` values, other than
  `UCOMUSE_KEY_NONE`.
- `pressed`: required non-null output pointer. On success, writes `1` when
  held or `0` when not held.
- Returns `INVALID_ARGUMENT` for invalid input. Returns `NOT_SUPPORTED` if
  the backend cannot map the key; otherwise returns the query status.

## Platform Notes

- **Windows:** Uses Windows system input APIs for mouse movement, buttons, and
  key-state polling.
- **Linux/X11:** When X11/XTEST is available, sends mouse input through the
  X server and queries key state from that server. A usable `DISPLAY` and X11
  permissions are required.
- **Linux/Wayland:** Sends mouse input through `/dev/uinput` and reads key
  state from `/dev/input/event*`. Root or appropriate device permissions are
  required. Relative cursor position is tracked locally and can become
  inaccurate if another application moves the pointer.
- **macOS:** Uses CoreGraphics for mouse input and key-state queries. The
  calling application needs Accessibility permission.
- **FreeBSD:** Uses X11/XTEST for mouse control and key-state queries. A
  usable X11 display session and XTEST are required.

Permissions and desktop-session restrictions are controlled by the OS.
Successful initialization does not guarantee that the session will remain
connected or that permissions will not later be revoked.
