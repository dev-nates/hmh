
#include "base/base_inc.h"
#include "input/input_inc.h"
#include "os/os_inc.h"
#include "wm/wm_inc.h"

#include "base/base_inc.c"
#include "input/input_inc.c"
#include "os/os_inc.c"
#include "wm/wm_inc.c"

// #define MICROSECONDS_PER_FRAME 16666
// global f32 dt = ((f32)MICROSECONDS_PER_FRAME)/1000000.f;

proc void
entry_point(Cmdline cl) {
	if (!input_open()) { return; }
	Arena *gamepad_arena = arena_alloc();

	IGamepad_List gamepads = zero_struct;
	gamepads.count = -1;

	Timer hotplug_timer = timer_make(millions(1));

	volatile b8 is_running = true;
	for (;is_running;) {
		Temp scr = scratch_begin(0,0);


		if (input_check_for_hotplugged_gamepads()) {
			timer_begin(&hotplug_timer);
		}
		if (gamepads.count == -1 || timer_elapsed(hotplug_timer)) {
			timer_done(&hotplug_timer);

			input_disconnect_gamepads(gamepads);
			arena_clear(gamepad_arena);

			gamepads = input_connect_gamepads(gamepad_arena);
			infof("Gamepad count: " TC(255, 0, 0) "%ld" TC_END, gamepads.count);
			for (each_node(gamepad, gamepads.head, IGamepad)) {
				infof("Gamepad: " TC(255, 255, 0) "%.*s" TC_END, svarg(gamepad->name));
			}
		}

		s64 idx = 0;
		IGamepad *gamepad = gamepads.head;
		for (s64 idx = 0; idx < gamepads.count; idx += 1, gamepad = gamepad->next) {
			IGamepad_Events events = input_gamepad_read_events(gamepad, scr.arena);
			if (events.count <= 0) { continue; }

			infof("Gamepad #" TC(255, 128, 0) "%ld" TC_END, idx);
			for (s64 i = 0; i < events.count; i += 1) {
				IGamepad_Event *event = events.v + i;
				switch (event->kind) {
				case IGamepad_Event_Button: {
					IButton_Event *e = cast(IButton_Event*)event;
					infof(
						"Event: [" TC(255,255,0) "%s" TC_END "]: State: %s -- Time %lu",
						"Button",
						(e->state) ? "pressed" : "released",
						e->e.time
					);
				} break;
				case IGamepad_Event_Axis: {
					IAxis_Event *e = cast(IAxis_Event*)event;
					infof(
						"Event: [" TC(255,0,0) "%s" TC_END "]: State: %f -- Time %lu",
						"Axis",
						cast(f64)(e->value),
						e->e.time
					);
				} break;
				}
			}
		}

		scratch_end(scr);
	}
	return;
}

/*
typedef struct Backbuffer Backbuffer;
struct Backbuffer {
	rawptr m;
	vec2s32 dim;
	s64 stride;
};

proc void
clear_backbuffer(Backbuffer buffer)
{
	for (s32 y = 0; y < buffer.dim.y; y += 1) {
		u8 *at = (u8 *)buffer.m + y*buffer.stride;
		memory_zero(at, size_of(u32)*buffer.dim.x);
	}
}

proc void
draw_rectangle(Backbuffer buffer, rng2s32 rect, vec4f32 color)
{
	rect.x0 = clamp_bot(0, rect.x0);
	rect.y0 = clamp_bot(0, rect.y0);
	rect.x1 = clamp_top(buffer.dim.x, rect.x1);
	rect.y1 = clamp_top(buffer.dim.y, rect.y1);

	// u32 rgba = u32_from_rgba(color);
	u8 *at = buffer.m;
	for (s32 y = rect.y0; y < rect.y1; y += 1)
	for (s32 x = rect.x0; x < rect.x1; x += 1) {
		u8 *at = (u8*)buffer.m + y*buffer.stride;
		u32 *pixel = (u32*)at + x;
		*pixel = 0x00ffffff;
	}
}

void
entry_point(Cmdline cl) {
	wm_init();

	Arena *arena = arena_alloc();
	WM_Window window = wm_window_open(arena, S("Fish"));

	// -------------------------------------------------------------------------------------------------
	// event loop
	vec2f32 offset = v2f32(0.f, 0.f);
	for (;wm_window_is_open(window);) {
		u64 start_time = os_time_us();

		Temp scr = scratch_begin(0,0);

		// typedef enum Action Action;
		// enum Action {
		// 	Action_Move_Left,
		// 	Action_Move_Right,
		// 	Action_Move_Up,
		// 	Action_Move_Down,
		// 	Action_Speedup,
		// };

		// typedef struct Keybind Keybind;
		// struct Keybind {
		// 	u32 scancode;
		// 	Action action;
		// 	IButton btn;
		// };

		// Keybind keybinds[] = {
		// 	{ .scancode = 17, .action = Action_Move_Up },
		// 	{ .scancode = 30, .action = Action_Move_Left },
		// 	{ .scancode = 31, .action = Action_Move_Down },
		// 	{ .scancode = 32, .action = Action_Move_Right },
		// 	{ .scancode = 57, .action = Action_Speedup },
		// };

		wm_consume_events();
		// IButton_Array scancodes = wm_consume_keyboard_scancodes(scr.arena);
		#if 0
		for each_index(i, scancodes.count) {
			IButton btn = scancodes.v[i];
			b32 pressed = ibutton_pressed(btn);
			b32 released = ibutton_released(btn);
			b32 down = ibutton_down(btn);
			if (pressed || released || down) {
				printf("button: [%li]: pressed: %i -- released: %i -- down: %i\n", i, pressed, released, down);
			}
		}
		#endif

		// for each_element(j, keybinds) {
		// 	Keybind *keybind = keybinds + j;
		// 	keybind->btn = scancodes.v[keybind->scancode];
		// }
		//
		// vec2f32 move_dir = v2f32(0.f, 0.f);
		// if (ibutton_down(keybinds[0].btn)) {
		// 	move_dir.y += 1.f;
		// }
		// if (ibutton_down(keybinds[1].btn)) {
		// 	move_dir.x -= 1.f;
		// }
		// if (ibutton_down(keybinds[2].btn)) {
		// 	move_dir.y -= 1.f;
		// }
		// if (ibutton_down(keybinds[3].btn)) {
		// 	move_dir.x += 1.f;
		// }
		// f32 speed = 100.f;
		// if (ibutton_down(keybinds[4].btn)) {
		// 	speed *= 3.f;
		// }
		// offset = add_2f32(offset, scale_2f32(move_dir, speed*dt));

		// -------------------------------------------------------------------------------------------------
		// rendering
		rawptr backbuffer_memory;
		s64 stride;
		WM_Buffer buffer_handle = wm_window_get_unused_backbuffer(window, &backbuffer_memory, &stride);
		vec2s32 dim = wm_window_get_backbuffer_dim(window);
		if (!handle_match(buffer_handle, zero_handle(WM_Buffer))) {
			Backbuffer backbuffer = {
				.m = backbuffer_memory,
				.dim = dim,
				.stride = stride,
			};

			clear_backbuffer(backbuffer);
			rng2s32 rect = r2s32p((s32)offset.x, (s32)offset.y, (s32)offset.x + 100, (s32)offset.y + 100);
			draw_rectangle(backbuffer, rect, v4f32(1.f, 0.f, 0.f, 0.f));

			wm_window_commit_buffer(window, buffer_handle);
		}

		u64 end_time = os_time_us();
		u64 frame_time = end_time-start_time;
		if (MICROSECONDS_PER_FRAME > frame_time) {
			u64 remaining_microseconds = MICROSECONDS_PER_FRAME - frame_time;
			u64 milliseconds = remaining_microseconds/1000;
			os_sleep_ms(milliseconds);
		}

		scratch_end(scr);
	}
}
*/
