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

## Use from another CMake project

You do not need to install the headers or library system-wide. Add
`libunicomuse` to your project as a Git submodule or a vendored source
directory, then add it with CMake:

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_app LANGUAGES C)

add_subdirectory(external/libunicomuse)

add_executable(my_app main.c)
target_link_libraries(my_app PRIVATE unicomuse::unicomuse)
```

Replace `external/libunicomuse` with the path where you keep the library.
The `unicomuse::unicomuse` target supplies the public include directory and
platform link dependencies, so source files can include
`<unicomuse/mouse.h>` and `<unicomuse/key.h>` directly. The consuming project
must enable the C language. This project adds its API test executable when
`BUILD_TESTING` is enabled (the default); set `-DBUILD_TESTING=OFF` only if you
intend to disable CTest-based tests for the entire CMake build.

Call `ucomuse_init()` before using the API and `ucomuse_shutdown()` when
finished. See [`example/basic.c`](./example/basic.c) for a complete C usage
example.

## Single-header use

The generated amalgamated header is available at
[`single_include/unicomuse.h`](./single_include/unicomuse.h). Include it in
your project and define `UCOMUSE_IMPLEMENTATION` in exactly one C/C++
translation unit before including it:

```c
#define UCOMUSE_IMPLEMENTATION
#include "unicomuse.h"
```

In all other translation units, include the header without defining the macro.
The header can also be generated from the source with
`python3 scripts/amalgamate.py`; GitHub Actions checks that the committed
header matches the implementation and publishes it as a workflow artifact and
as `unicomuse.h` on GitHub Releases. Every push to `main` creates a pre-release
tagged with that commit's SHA and attaches the single header after all
cross-platform checks pass. On Linux, the single-header build uses
the native `/dev/uinput` backend by default; define `UCOMUSE_USE_X11` in the
implementation translation unit and link X11/XTEST to enable the X11 backend.
When compiling manually, link `-lm` on Linux/FreeBSD; macOS also requires the
CoreFoundation, CoreGraphics, and ApplicationServices frameworks. The CMake
target handles these platform link dependencies automatically.

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
