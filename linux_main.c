
#include "base/inc.h"

#include "game/game.h"
#include "game/game.c"

#include "os/linux/inc.h"

#include "base/inc.c"
#include "os/linux/inc.c"


// -------------------------------------------------------------------------------------------------
// Window backend
proc void
window_backend_open(string8 window_title);
proc void
window_backend_close(void);
proc b8
window_backend_not_closed(void);
proc void
window_backend_events(void);
proc Backbuffer
window_backend_get_current_backbuffer(void);
proc vec2s32
window_backend_get_window_size(void);

#define LOG_WINDOW_BACKEND true
#define WINDOW_BACKEND_LOG_COLOR TC(249,142,79)
#define BYTES_PER_PIXEL 4
#if WINDOW_BACKEND == WAYLAND
#include "window_backends/wayland/wayland_backend.h"
#include "window_backends/wayland/wayland_backend.c"
#elif
#error "Window backend not specified"
#endif

#define MICROSECONDS_PER_FRAME 16666
global f32 dt = ((f32)MICROSECONDS_PER_FRAME)/1000000.f;

proc void
entry_point(Cmdline cl) {
	window_backend_open(S("Handmade Hero"));

	// -------------------------------------------------------------------------------------------------
	// @Todo: Gamepad input

	f32 offset = 0;
	for (;window_backend_not_closed();) {
		offset += 256.f*dt;
		u64 start = time_us();

		window_backend_events();

		vec2s32 window_size = window_backend_get_window_size();
		Backbuffer backbuffer = window_backend_get_current_backbuffer();
		if (backbuffer.memory != nil) {
			u8 *row = cast(u8*)(backbuffer.memory);
			for (each_index(y, window_size.y)) {
				for (each_index(x, window_size.x)) {
					u32 *pixel = cast(u32*)(row) + x;

					u8 blue = cast(u8)(y);
					u8 green = cast(u8)(x + cast(s64)(offset));

					u32 color = (u32)(green<<8) | (u32)(blue<<0);
					*pixel = color;
				}
				row += backbuffer.stride;
			}
		}
		window_backend_swap_buffers(backbuffer);

		time_wait_us(start, MICROSECONDS_PER_FRAME);
	}

	window_backend_close();
}
