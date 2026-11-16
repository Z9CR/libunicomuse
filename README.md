# libunicomuse

`libunicomuse` is a C99 computer-use library for Windows, Linux (X11 and
Wayland), macOS, and FreeBSD. It provides mouse movement, buttons and wheel
control, and polls whether keys are currently held in the active desktop
session. Keyboard polling is non-blocking; it does not record key events or
inject key presses.

[中文版](./doc/README.ZH-CN.md)

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build
```

On Linux, CMake uses X11/XTEST when available. X11 calls need a valid
`DISPLAY`; under Wayland, the native `/dev/uinput` backend is used and the
process needs root privileges (or equivalent device permissions). Install
X11 development files including XTEST to enable the X11 backend. On FreeBSD,
the X11/XTEST development libraries and a running X server are required.
The Linux native backend queries screen size from `/dev/fb0` when possible
(otherwise it uses 1920x1080), and its relative movement tracking cannot
observe cursor movement performed by other applications.

Windows uses the system input APIs.

## Notes

Build the runnable example by enabling it at configure time:

```sh
cmake -S . -B build -DUCOMUSE_BUILD_EXAMPLES=ON
cmake --build build --target unicomuse_example
```

The example moves and clicks the mouse when run. On macOS, initialization
requests Accessibility permission if needed. Grant permission to the calling
application in `System Settings > Privacy & Security > Device Control and Data Access`; if
launched from Terminal, Terminal may also need permission. Relaunch the
application after granting access.

## API

See the detailed API documentation: [English](./doc/api.md) /
[简体中文](./doc/api.zh-CN.md).

Include `<unicomuse/mouse.h>` for mouse APIs and
`<unicomuse/key.h>` for keyboard APIs. Call `ucomuse_init()` once before using
the other functions. Call `ucomuse_shutdown()` when finished; it closes
library-owned OS handles. There are no caller-allocated objects or resources
to free.

Mouse coordinates are screen pixels, with `(0, 0)` at the primary display's
top-left. Positive angles rotate clockwise from rightward, and wheel notches
are positive magnitudes whose direction is specified separately. Smooth
movement duration is in milliseconds.

See [`example/basic.c`](./example/basic.c) for a runnable example of mouse
control and keyboard-state queries.

Every operation returns a `ucomuse_status`; check it to detect OS or permission
errors. Key polling is non-blocking and reports current key state, not key
events. When multiple keys are held, `ucomuse_key_get_pressed()` returns one
key in backend-defined scan order.
The API uses process-wide backend state; callers should serialize concurrent
operations and must not call `ucomuse_shutdown()` while another operation is
running.

## Multi-device behavior

The API does not let callers select a physical keyboard or mouse. Key-state
queries report held keys across the active session, while mouse actions are
sent through the platform's active input backend.
