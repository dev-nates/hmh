#ifndef WM_H
#define WM_H

typedef struct WM_Window WM_Window;
struct WM_Window {
	u64 v[1];
};

typedef struct WM_Buffer WM_Buffer;
struct WM_Buffer {
	u64 v[1];
};

#define WM_WINDOW_BACKBUFFER_COUNT 4
#define WM_WINDOW_BACKBUFFER_MAX_SIZE (1920*1080*size_of(u32))*4

//--------------------------------------------------------------------------------
// layer initialization

proc void
wm_init(void);

//--------------------------------------------------------------------------------
// events

proc void
wm_consume_events(void);

//--------------------------------------------------------------------------------
// input

proc IButton_Array
wm_consume_keyboard_scancodes(Arena *arena);

//--------------------------------------------------------------------------------
// window

proc WM_Window
wm_window_open(Arena *arena, string title);

proc void
wm_window_close(WM_Window window);


proc vec2s32
wm_window_get_backbuffer_dim(WM_Window window);
proc WM_Buffer
wm_window_get_unused_backbuffer(WM_Window window, /**/ rawptr *backbuffer, s64 *stride);

proc b32
wm_window_is_open(WM_Window window);

#endif // WM_H
