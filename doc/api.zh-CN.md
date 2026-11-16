# libunicomuse API 参考

本文档对应 `include/unicomuse/` 下的公共头文件。库使用 ISO C99 编写，
提供 C ABI；公共接口不要求调用方创建对象或分配库内存。

[English](./api.md).

## 头文件与链接

- `<unicomuse/common.h>`：状态类型和库生命周期函数。
- `<unicomuse/mouse.h>`：鼠标按钮、滚轮方向和鼠标控制函数。
- `<unicomuse/key.h>`：按键枚举和按键状态查询函数。

通常直接包含 `mouse.h` 和/或 `key.h` 即可，它们会包含 `common.h`。
所有公共声明使用 `extern "C"` 保护，可从 C++ 调用，但库本身按 C99 编译。

## 基本概念

### 初始化和资源生命周期

调用任何鼠标或键盘操作之前，必须先调用 `ucomuse_init()`。成功后，库会
保留后端所需的操作系统句柄或连接。使用结束后调用 `ucomuse_shutdown()`
关闭这些资源。库不返回需要调用方释放的对象、缓冲区或句柄。

重复初始化已初始化的库会成功且不会重复分配资源。`ucomuse_shutdown()`
可以重复调用。关闭后，下一次使用前必须重新初始化。

库的状态是进程级共享的，不支持同时管理多个独立实例。调用方应串行化
并发 API 调用；不得在其他线程仍调用库函数时执行 `ucomuse_shutdown()`。

### 状态返回值

除 `ucomuse_shutdown()` 外，所有公共操作返回 `ucomuse_status`：

| 值 | 含义 |
| --- | --- |
| `UCOMUSE_STATUS_OK` | 操作成功。 |
| `UCOMUSE_STATUS_ERROR` | 后端或操作系统操作失败，例如无法连接显示服务器、访问输入设备或发送输入。 |
| `UCOMUSE_STATUS_NOT_INITIALIZED` | 尚未成功调用 `ucomuse_init()`，或已经调用 `ucomuse_shutdown()`。 |
| `UCOMUSE_STATUS_INVALID_ARGUMENT` | 参数不合法，例如空输出指针、无效按钮或方向、非有限角度/滚轮量。 |
| `UCOMUSE_STATUS_NOT_SUPPORTED` | 当前后端无法查询指定按键或不支持该能力。 |

检查返回值；不要把失败当作操作成功。键盘查询中的 `pressed` 输出值只有在
返回 `UCOMUSE_STATUS_OK` 时才有效。

### 鼠标坐标和移动

绝对坐标以像素为单位，屏幕左上方向右为 X 正方向、向下为 Y 正方向。
`ucomuse_mouse_move_to()` 使用主显示器屏幕坐标。实际边界与坐标映射由
操作系统和后端决定；Linux Wayland 的 uinput 后端使用 `/dev/fb0` 的屏幕
尺寸（如果无法读取则回退到 1920x1080），并无法观察其他程序移动光标后
发生的位置变化。

相对移动角度以度为单位，从向右（0°）开始顺时针增加：90° 向下，
180° 向左，270° 向上。距离以像素计；负距离会反向移动。

平滑移动按大约 4 毫秒间隔分步发送。它是同步调用：函数返回前完成所有
步骤或在某一步报错。`time_ms == 0` 表示立即移动；非零时长小于一个步长
仍至少发送一步。

### 按键查询语义

键盘 API 轮询当前是否有键处于按下状态，不提供按下/释放事件流、事件历史、
键盘注入或指定设备选择。`ucomuse_key_get_pressed()` 只返回一个当前按下的
键；同时按下多个键时，返回哪个键由平台后端的扫描顺序决定。

`ucomuse_key` 使用与 GLFW 相近的数值约定，方便跨平台使用同一按键标识。
它表示逻辑按键，不是字符输入；键盘布局、修饰键和平台映射可能影响某些
标点或特殊键的检测。后端无法映射的按键可能返回
`UCOMUSE_STATUS_NOT_SUPPORTED`。

## 公共类型

公共头文件没有定义 `struct` 类型。数据类型均为枚举。

### `ucomuse_status`

由所有返回状态的函数使用，具体值及含义见“状态返回值”。

### `ucomuse_mouse_button`

传给鼠标按钮函数的按钮标识：

| 常量 | 含义 |
| --- | --- |
| `UCOMUSE_MOUSE_LEFT` | 左键。 |
| `UCOMUSE_MOUSE_RIGHT` | 右键。 |
| `UCOMUSE_MOUSE_MIDDLE` | 中键/滚轮按键。 |
| `UCOMUSE_MOUSE_X1` | 侧键 1（通常为后退键）。 |
| `UCOMUSE_MOUSE_X2` | 侧键 2（通常为前进键）。 |

### `ucomuse_wheel_direction`

传给滚轮函数的滚动方向：

| 常量 | 含义 |
| --- | --- |
| `UCOMUSE_WHEEL_UP` | 向上滚动。 |
| `UCOMUSE_WHEEL_DOWN` | 向下滚动。 |

滚轮量必须是有限的非负数；不足一个刻度的正数会被提升为一个刻度，
其他数值按最近整数取整。

### `ucomuse_key`

可用于查询的跨平台按键标识：

| 常量 | 含义 |
| --- | --- |
| `UCOMUSE_KEY_NONE` | 没有检测到按下的键；仅作为 `get_pressed` 输出值，不是有效查询目标。 |
| `UCOMUSE_KEY_SPACE` | 空格。 |
| `UCOMUSE_KEY_APOSTROPHE`、`COMMA`、`MINUS`、`PERIOD`、`SLASH` | 撇号、逗号、减号、句点、斜线标点。实际可检测性取决于键盘布局和后端映射。 |
| `UCOMUSE_KEY_0` … `UCOMUSE_KEY_9` | 主键区数字键。 |
| `UCOMUSE_KEY_SEMICOLON`、`EQUAL`、`LEFT_BRACKET`、`BACKSLASH`、`RIGHT_BRACKET`、`GRAVE` | 分号、等号、左右方括号、反斜线、重音符号。实际映射可能随布局而异。 |
| `UCOMUSE_KEY_A` … `UCOMUSE_KEY_Z` | 字母键。 |
| `UCOMUSE_KEY_ESCAPE`、`ENTER`、`TAB`、`BACKSPACE` | Esc、回车、Tab、退格。 |
| `UCOMUSE_KEY_INSERT`、`DELETE` | Insert、Delete。macOS 键盘上的 Delete 通常对应退格，前向删除映射为 `UCOMUSE_KEY_DELETE`。 |
| `UCOMUSE_KEY_RIGHT`、`LEFT`、`DOWN`、`UP` | 方向键。 |
| `UCOMUSE_KEY_PAGE_UP`、`PAGE_DOWN`、`HOME`、`END` | 页面和光标导航键。 |
| `UCOMUSE_KEY_CAPS_LOCK`、`SCROLL_LOCK`、`NUM_LOCK`、`PRINT_SCREEN`、`PAUSE` | 锁定及系统键。部分键在特定键盘上没有实体对应键。 |
| `UCOMUSE_KEY_F1` … `UCOMUSE_KEY_F12` | F1 至 F12。 |
| `UCOMUSE_KEY_KP_0` … `UCOMUSE_KEY_KP_9` | 数字小键盘 0 至 9。 |
| `UCOMUSE_KEY_KP_DECIMAL`、`KP_DIVIDE`、`KP_MULTIPLY`、`KP_SUBTRACT`、`KP_ADD` | 小键盘小数点、除、乘、减、加。 |
| `UCOMUSE_KEY_KP_ENTER`、`KP_EQUAL` | 小键盘回车和等号。平台可能将小键盘回车与普通回车映射到同一系统按键。 |
| `UCOMUSE_KEY_LEFT_SHIFT`、`LEFT_CONTROL`、`LEFT_ALT`、`LEFT_SUPER` | 左侧 Shift、Control、Alt/Option、Super/Command/Windows 键。 |
| `UCOMUSE_KEY_RIGHT_SHIFT`、`RIGHT_CONTROL`、`RIGHT_ALT`、`RIGHT_SUPER` | 右侧 Shift、Control、Alt/Option、Super/Command/Windows 键。 |
| `UCOMUSE_KEY_MENU` | 键盘菜单/应用程序键；不是所有平台都能映射。 |

枚举常量带有 `UCOMUSE_KEY_` 前缀；表格中省略此前缀的部分仅为排版简写。

## 生命周期函数

### `ucomuse_status ucomuse_init(void)`

初始化当前平台的鼠标和键盘后端。

- 成功：返回 `UCOMUSE_STATUS_OK`，之后可以使用鼠标和键盘函数。
- 失败：返回后端错误。失败后不会进入已初始化状态，可在解决权限或会话
  问题后再次尝试。
- macOS：如果进程尚无辅助功能权限，初始化会请求系统显示授权提示；用户
  授权后通常需要重新启动应用再初始化。

Linux Wayland 的原生输入后端需要访问 `/dev/uinput`，通常要求以 root 运行
或配置等效设备权限。Linux X11 后端需要可用的 `DISPLAY` 和 XTEST。FreeBSD
目前需要可用的 X11/XTEST 显示会话。

### `void ucomuse_shutdown(void)`

关闭初始化时打开的系统句柄和连接。可以在未初始化时调用，也可以重复调用。
不返回状态；调用方必须确保没有其他线程仍在执行本库的函数。

## 鼠标函数

### `ucomuse_status ucomuse_mouse_move_to(int32_t x, int32_t y, uint32_t time_ms)`

把鼠标移动到屏幕坐标 `(x, y)`。

- `x`、`y`：目标屏幕像素坐标。
- `time_ms`：移动时长，毫秒。0 表示立即移动，非零值执行平滑移动。
- 返回：`OK`、`NOT_INITIALIZED` 或后端的错误状态。

调用示例：

```c
ucomuse_mouse_move_to(500, 300, 250);
```

### `ucomuse_status ucomuse_mouse_translate(double angle_degrees, int32_t distance, uint32_t time_ms)`

从当前鼠标位置相对移动。

- `angle_degrees`：角度（度），0° 向右、90° 向下，顺时针为正；必须为有限值。
- `distance`：移动距离（像素）；负数表示反方向。
- `time_ms`：移动时长，语义与 `ucomuse_mouse_move_to()` 相同。
- 返回：`INVALID_ARGUMENT` 表示角度无效或计算出的位移超出 `int32_t` 范围；
  否则返回后端执行状态。

### `ucomuse_status ucomuse_mouse_click(ucomuse_mouse_button button)`

对指定按钮执行一次按下再释放。

- `button`：有效的 `ucomuse_mouse_button` 值。
- 返回：初始化状态、参数错误或后端状态。

如果后端报告按下失败，则不会继续发送释放；如果成功按下而释放失败，函数
返回释放阶段的错误状态。

### `ucomuse_status ucomuse_mouse_press(ucomuse_mouse_button button)`

按下指定鼠标按钮并保持按下状态，直到调用 `ucomuse_mouse_release()`。

不要忘记配对释放；意外保持按钮按下可能影响桌面交互。

### `ucomuse_status ucomuse_mouse_release(ucomuse_mouse_button button)`

释放指定鼠标按钮。

`ucomuse_mouse_press()` 和 `ucomuse_mouse_release()` 接受
`UCOMUSE_MOUSE_LEFT`、`RIGHT`、`MIDDLE`、`X1`、`X2`。枚举范围以外的值返回
`UCOMUSE_STATUS_INVALID_ARGUMENT`。

### `ucomuse_status ucomuse_mouse_wheel(ucomuse_wheel_direction direction, double notches)`

滚动鼠标滚轮。

- `direction`：`UCOMUSE_WHEEL_UP` 或 `UCOMUSE_WHEEL_DOWN`。
- `notches`：非负滚轮刻度数，必须是有限值且不大于 `INT_MAX`。0 表示不发送
  滚动事件；大于 0 且小于 0.5 的值至少发送一个刻度，其余值按最近整数取整。
- 返回：方向或数值不合法时为 `INVALID_ARGUMENT`，否则为后端执行状态。

## 键盘函数

### `ucomuse_status ucomuse_key_get_pressed(ucomuse_key *key)`

查询一个当前处于按下状态的键。

- `key`：必需的非空输出指针。
- 成功且没有检测到按键时，写入 `UCOMUSE_KEY_NONE`。
- 同时按下多个键时，只返回一个；选择顺序由后端决定。
- 返回：参数错误、未初始化状态或后端查询状态。

### `ucomuse_status ucomuse_key_is_pressed(ucomuse_key key, int *pressed)`

查询指定按键当前是否被按住。

- `key`：有效的 `ucomuse_key`，不能为 `UCOMUSE_KEY_NONE`。
- `pressed`：必需的非空输出指针。成功时写入 `1`（按下）或 `0`（未按下）。
- 返回：无效输入时为 `INVALID_ARGUMENT`；后端无法映射该按键时可能返回
  `NOT_SUPPORTED`；否则返回实际查询状态。

## 平台注意事项

- **Windows**：通过 Windows 系统输入 API 移动鼠标、发送按钮和读取按键状态。
- **Linux/X11**：有可用 X11/XTEST 时通过 X Server 发送鼠标输入，并从当前
  X Server 查询按键状态。`DISPLAY` 和相应 X11 权限必须可用。
- **Linux/Wayland**：使用 `/dev/uinput` 发送鼠标输入，并从
  `/dev/input/event*` 读取按键状态。需要 root 或适当的设备访问权限。相对
  输入设备的光标位置是本进程本地跟踪的，外部鼠标移动可能导致跟踪位置偏离。
- **macOS**：通过 CoreGraphics 发送鼠标输入和查询按键；调用应用需要
  Accessibility/辅助功能授权。
- **FreeBSD**：通过 X11/XTEST 控制鼠标并读取按键状态，需要可用的 X11 显示
  会话及 XTEST。

调用权限和桌面环境限制由操作系统决定；成功初始化并不保证桌面环境之后
不会断开或撤销访问权限。
