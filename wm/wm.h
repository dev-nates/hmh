#ifndef WM_H
#define WM_H

typedef struct WM_Window WM_Window;
struct WM_Window {
	u64 v[1];
};
proc WM_Window wm_window_zero(void) { return cast(WM_Window)zero_struct; }
proc b8 wm_window_match(WM_Window a, WM_Window b) { return cast(b8)(a.v[0] == b.v[0]); }

typedef struct WM_Buffer WM_Buffer;
struct WM_Buffer {
	u64 v[1];
};
proc WM_Buffer wm_buffer_zero(void) { return cast(WM_Buffer)zero_struct; }
proc b8 wm_buffer_match(WM_Buffer a, WM_Buffer b) { return cast(b8)(a.v[0] == b.v[0]); }

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

// proc IButton_Array
// wm_consume_keyboard_scancodes(Arena *arena);

//--------------------------------------------------------------------------------
// window

proc WM_Window
wm_window_open(Arena *arena, string title);

proc void
wm_window_close(WM_Window window);


proc vec2s32
wm_window_get_backbuffer_dim(WM_Window window);
proc WM_Buffer
wm_window_get_unused_backbuffer(WM_Window window, _ret_ rawptr *backbuffer, s64 *stride);

proc b8
wm_window_is_open(WM_Window window);

#endif // WM_H
