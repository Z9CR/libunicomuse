# libunicomuse

`libunicomuse` 是一个使用 C99 编写的 computer-use 库，目标平台为
Windows、Linux（X11 和 Wayland）、macOS 与 FreeBSD。它提供鼠标移动、
按键和滚轮控制，以及对当前桌面会话中按键状态的轮询。键盘轮询为非阻塞
操作；库不记录按键事件，也不注入键盘按键。

[English](../README.md)

## 构建

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build
```

## 在其他 CMake 项目中使用

不需要把头文件或库安装到系统目录。可以将 `libunicomuse` 作为 Git 子模块
或 vendored 源码目录放入自己的项目，再通过 CMake 添加：

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_app LANGUAGES C)

add_subdirectory(external/libunicomuse)

add_executable(my_app main.c)
target_link_libraries(my_app PRIVATE unicomuse::unicomuse)
```

将 `external/libunicomuse` 替换为你存放库源码的实际路径。
`unicomuse::unicomuse` target 会传递公共头文件目录和平台链接依赖，因此
源文件可以直接包含 `<unicomuse/mouse.h>` 和 `<unicomuse/key.h>`。使用项目
必须启用 C 语言。本项目在启用 `BUILD_TESTING` 时（默认启用）会添加 API
测试程序。只有在你希望关闭整个 CMake 构建中的 CTest 测试时，才设置
`-DBUILD_TESTING=OFF`。

调用 API 前先调用 `ucomuse_init()`，使用结束后调用 `ucomuse_shutdown()`。
完整的 C 语言调用示例参见 [`../example/basic.c`](../example/basic.c)。

## 单头文件用法

生成好的单头文件位于
[`../single_include/unicomuse.h`](../single_include/unicomuse.h)。在项目中
包含该文件，并在且仅在一个 C 翻译单元中，先定义 `UCOMUSE_IMPLEMENTATION`
再包含它：

```c
#define UCOMUSE_IMPLEMENTATION
#include "unicomuse.h"
```

其他翻译单元只包含头文件，不定义该宏。也可以通过
`python3 scripts/amalgamate.py` 从源码重新生成。GitHub Actions 会检查提交的
单头文件是否与源码一致，并将其作为 workflow artifact 发布；GitHub Release
发布时也会附加名为 `unicomuse.h` 的文件。Linux 单头文件默认使用原生
`/dev/uinput` 后端；如需使用 X11 后端，在实现所在的翻译单元中定义
`UCOMUSE_USE_X11` 并链接 X11/XTEST。手动编译时，Linux/FreeBSD 需要链接
`-lm`；macOS 还需要 CoreFoundation、CoreGraphics 和 ApplicationServices
framework。CMake target 会自动传递平台所需的链接依赖。

Linux 上，如果检测到 X11/XTEST，CMake 会启用 X11 后端。X11 后端需要可用
的 `DISPLAY`。Wayland 会话使用原生 `/dev/uinput` 后端，进程需要 root 权限
或具有相应设备访问权限。安装包含 XTEST 的 X11 开发文件可启用 X11 后端。
FreeBSD 需要 X11/XTEST 开发库以及正在运行的 X Server。

Linux 原生后端会尝试从 `/dev/fb0` 获取屏幕尺寸；无法读取时使用
1920x1080。该后端使用相对移动并在进程内追踪光标位置，因此无法感知其他
应用移动光标后的位置变化。

Windows 使用系统输入 API。

## 注意事项

配置时启用示例即可构建可运行的示例程序：

```sh
cmake -S . -B build -DUCOMUSE_BUILD_EXAMPLES=ON
cmake --build build --target unicomuse_example
```

示例运行时会移动鼠标并点击鼠标按键。macOS 上，如果尚未获得辅助功能权限，
初始化时会请求系统显示授权提示。请在 `系统设置 > 隐私与安全性 >
设备控制和数据访问` 中授权调用应用；如果从 Terminal 启动，Terminal
本身也可能需要授权。授予权限后请重新启动应用。

## API

详细说明请参阅[英文 API 文档](./api.md)或
[简体中文 API 文档](./api.zh-CN.md)。

鼠标接口请包含 `<unicomuse/mouse.h>`，键盘接口请包含
`<unicomuse/key.h>`。调用其他函数之前先调用一次 `ucomuse_init()`；
使用完成后调用 `ucomuse_shutdown()`，关闭库打开的操作系统句柄。库不要求
调用方释放自行分配的对象或资源。

鼠标坐标单位为屏幕像素，`(0, 0)` 位于主显示器左上角。角度以向右为起点，
顺时针为正；滚轮刻度数量为非负数，滚动方向单独指定。平滑移动的时长单位
为毫秒。

[`../example/basic.c`](../example/basic.c) 提供鼠标控制和键盘状态查询示例。
运行示例会实际操作鼠标。

所有返回状态的操作都会返回 `ucomuse_status`，请检查返回值以发现操作系统
或权限错误。键盘查询读取当前按键状态，而不是按键事件。多个键同时按下时，
`ucomuse_key_get_pressed()` 根据后端的扫描顺序返回其中一个键。

库的后端状态是进程级共享的。调用方应串行化并发操作，不要在其他线程仍在
调用库函数时执行 `ucomuse_shutdown()`。

## 多设备说明

当前 API 不提供选择具体物理键盘或鼠标的接口。按键状态查询针对当前桌面
会话，鼠标操作由平台当前使用的输入后端发送。