#ifndef WM_LINUX_H
#define WM_LINUX_H

//--------------------------------------------------------------------------------
// walyand headers
#pragma push_macro("global");
#undef global
#include "wayland-client.h"
#include "xdg-shell.h"
#include "xdg-shell.c"
#include "xkbcommon/xkbcommon.h"
#pragma pop_macro("global");

//--------------------------------------------------------------------------------
// pointer events

typedef enum Wayland_Pointer_Event_Flags Wayland_Pointer_Event_Flags;
enum Wayland_Pointer_Event_Flags {
	Wayland_Pointer_Event_Enter         = bit1,
	Wayland_Pointer_Event_Leave         = bit2,
	Wayland_Pointer_Event_Motion        = bit3,
	Wayland_Pointer_Event_Button        = bit4,
	Wayland_Pointer_Event_Axis          = bit5,
	Wayland_Pointer_Event_Axis_Source   = bit6,
	Wayland_Pointer_Event_Axis_Stop     = bit7,
	Wayland_Pointer_Event_Axis_Discrete = bit8,
};

typedef struct Wayland_Pointer_Event Wayland_Pointer_Event;
struct Wayland_Pointer_Event {
	Wayland_Pointer_Event_Flags flags;
	wl_fixed_t surface_x, surface_y;
	u32 button, state;
	u32 time;
	u32 serial;
	struct {
		b32 valid;
		wl_fixed_t value;
		s32 discrete;
	} axes[Axis2_COUNT];
	u32 axis_source;
};

//--------------------------------------------------------------------------------
// wl lnx state
typedef struct Wayland_state Wayland_state;
struct Wayland_state {
	struct wl_display *display;
	struct wl_registry *registry;
	struct wl_compositor *compositor;
	struct wl_shm *shm;
	struct xdg_wm_base *wm;

	struct wl_seat *seat;
	struct wl_keyboard *keyboard;
	struct wl_pointer *pointer;
	struct wl_touch *touch;

	struct xkb_state   *xkb_state;
	struct xkb_context *xkb_context;
	struct xkb_keymap  *xkb_keymap;

	// IButton scancodes[256];

	// $arena *text_input_arena;
	// $string_list text_input;

	// pointer state
	Wayland_Pointer_Event pointer_event;
};
global Wayland_state wayland_state = zero_struct;

//--------------------------------------------------------------------------------
// wl lnx buffer
typedef struct Wayland_Buffer Wayland_Buffer;
struct Wayland_Buffer {
	struct wl_buffer *buffer;
	b32 in_use;
	s32 offset;
	rawptr m;
};

//--------------------------------------------------------------------------------
// wl lnx window
#define WL_WINDOW_MAX_BUFFER_COUNT 8
#define PIXEL_SIZE 4
typedef struct Wayland_Window Wayland_Window;
struct Wayland_Window {
	struct wl_surface *surface;
	struct xdg_surface *xdg_surface;
	struct xdg_toplevel *toplevel;

	s32 pool_size;
	struct wl_shm_pool *pool;
	rawptr shared_memory;
	OS_Shared_Memory shared_memory_file;

	vec2s32 dim;
	b8 closed;
	string title;

	s32     buffer_count;
	vec2s32 max_dim;
	s32     max_stride;
	Wayland_Buffer buffers[WL_WINDOW_MAX_BUFFER_COUNT];
};

//--------------------------------------------------------------------------------
// wayland lnx procs

//--------------------------------------------------------------------------------
// wl registry

proc void _wayland_registry_establish_interface(rawptr data, struct wl_registry *registry, u32 name, const char *interface, u32 version);
proc void _wayland_registry_unestablish_interface( rawptr data, struct wl_registry *registry, u32 name);

#pragma push_macro("global");
#undef global
static const struct wl_registry_listener wayland_registry_listener = {
	.global        = _wayland_registry_establish_interface,
	.global_remove = _wayland_registry_unestablish_interface,
};
#pragma pop_macro("global");

//--------------------------------------------------------------------------------
// wl buffers
proc void _wayland_buffer_release(rawptr data, struct wl_buffer *buffer);

global const struct wl_buffer_listener wayland_buffer_listener = {
	.release = _wayland_buffer_release,
};

//--------------------------------------------------------------------------------
// wl seat

proc void _wayland_seat_capabilities(rawptr data, struct wl_seat *wl_seat, u32 capabilities);
proc void _wayland_seat_name(rawptr data, struct wl_seat *wl_seat, const char *name);

global const struct wl_seat_listener wayland_seat_listener = {
	.capabilities = _wayland_seat_capabilities,
	.name         = _wayland_seat_name,
};

//--------------------------------------------------------------------------------
// wl keyboard

proc void _wayland_keyboard_keymap(rawptr data, struct wl_keyboard *wl_keyboard, u32 format, int fd, u32 size);
proc void _wayland_keyboard_enter(rawptr data, struct wl_keyboard *wl_keyboard, u32 serial, struct wl_surface *surface, struct wl_array *keys);
proc void _wayland_keyboard_leave(rawptr data, struct wl_keyboard *wl_keyboard, u32 serial, struct wl_surface *surface);
proc void _wayland_keyboard_key(rawptr data, struct wl_keyboard *wl_keyboard, u32 serial, u32 time, u32 key, u32 state);
proc void _wayland_keyboard_modifiers(rawptr data, struct wl_keyboard *wl_keyboard, u32 serial, u32 mods_depressed, u32 mods_latched, u32 mods_locked, u32 group);
proc void _wayland_keyboard_repeat_info(rawptr data, struct wl_keyboard *wl_keyboard, s32 rate, s32 delay);

global const struct wl_keyboard_listener wayland_keyboard_listener = {
	.keymap      = _wayland_keyboard_keymap,
	.enter       = _wayland_keyboard_enter,
	.leave       = _wayland_keyboard_leave,
	.key         = _wayland_keyboard_key,
	.modifiers   = _wayland_keyboard_modifiers,
	.repeat_info = _wayland_keyboard_repeat_info,
};

//--------------------------------------------------------------------------------
// wl pointer

proc void _wayland_pointer_enter(rawptr data, struct wl_pointer *wl_pointer, u32 serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y);
proc void _wayland_pointer_leave(rawptr data, struct wl_pointer *wl_pointer, u32  serial, struct wl_surface *surface);
proc void _wayland_pointer_motion(rawptr data, struct wl_pointer *wl_pointer, u32 time, wl_fixed_t surface_x, wl_fixed_t surface_y);
proc void _wayland_pointer_button(rawptr data, struct wl_pointer *wl_pointer, u32 serial, u32 time, u32 button, u32 state);
proc void _wayland_pointer_axis(rawptr data, struct wl_pointer *wl_pointer, u32 time, u32 axis, wl_fixed_t value);
proc void _wayland_pointer_frame(rawptr data, struct wl_pointer *wl_pointer);
proc void _wayland_pointer_axis_source(rawptr data, struct wl_pointer *wl_pointer, u32 axis_source);
proc void _wayland_pointer_axis_stop(rawptr data, struct wl_pointer *wl_pointer, u32 time, u32 axis);
proc void _wayland_pointer_axis_discrete(rawptr data, struct wl_pointer *wl_pointer, u32 axis, s32 discrete);
proc void _wayland_pointer_axis_value120(rawptr data, struct wl_pointer *wl_pointer, u32 axis, s32 value120);
proc void _wayland_pointer_axis_relative_direction(rawptr data, struct wl_pointer *wl_pointer, u32 axis, u32 direction);

global const struct wl_pointer_listener wayland_pointer_listener = {
	.enter                   = _wayland_pointer_enter,
	.leave                   = _wayland_pointer_leave,
	.motion                  = _wayland_pointer_motion,
	.button                  = _wayland_pointer_button,
	.axis                    = _wayland_pointer_axis,
	.axis_source             = _wayland_pointer_axis_source,
	.axis_stop               = _wayland_pointer_axis_stop,
	.axis_discrete           = _wayland_pointer_axis_discrete,
	.axis_value120           = _wayland_pointer_axis_value120,
	.axis_relative_direction = _wayland_pointer_axis_relative_direction,
	.frame                   = _wayland_pointer_frame,
};

//--------------------------------------------------------------------------------
// wl touch

proc void _wayland_touch_down(rawptr data, struct wl_touch *wl_touch, u32 serial, u32 time, struct wl_surface *surface, s32 id, wl_fixed_t x, wl_fixed_t y);
proc void _wayland_touch_up(rawptr data, struct wl_touch *wl_touch, u32 serial, u32 time, s32 id);
proc void _wayland_touch_motion(rawptr data, struct wl_touch *wl_touch, u32 time, s32 id, wl_fixed_t x, wl_fixed_t y);
proc void _wayland_touch_frame(rawptr data, struct wl_touch *wl_touch);
proc void _wayland_touch_cancel(rawptr data, struct wl_touch *wl_touch);
proc void _wayland_touch_shape(rawptr data, struct wl_touch *wl_touch, s32 id, wl_fixed_t major, wl_fixed_t minor);
proc void _wayland_touch_orientation(rawptr data, struct wl_touch *wl_touch, s32 id, wl_fixed_t orientation);

global const struct wl_touch_listener wayland_touch_listener = {
 .down        = _wayland_touch_down,
 .up          = _wayland_touch_up,
 .motion      = _wayland_touch_motion,
 .frame       = _wayland_touch_frame,
 .cancel      = _wayland_touch_cancel,
 .shape       = _wayland_touch_shape,
 .orientation = _wayland_touch_orientation,
};

//--------------------------------------------------------------------------------
// xdg wm base

proc void _xdg_wm_base_ping(rawptr data, struct xdg_wm_base *wm, u32 serial);

global const struct xdg_wm_base_listener _xdg_wm_base_listener = {
	.ping = _xdg_wm_base_ping,
};

//--------------------------------------------------------------------------------
// xdg surface

proc void _xdg_surface_configure(rawptr data, struct xdg_surface *surface, u32 serial);

global const struct xdg_surface_listener _xdg_surface_listener = {
	.configure = _xdg_surface_configure,
};

//--------------------------------------------------------------------------------
// xdg toplevel

proc void _xdg_toplevel_configure(rawptr data, struct xdg_toplevel *xdg_toplevel, s32 width, s32 height, struct wl_array *states);
proc void _xdg_toplevel_close(rawptr data, struct xdg_toplevel *xdg_toplevel);
proc void _xdg_toplevel_configure_bounds(rawptr data, struct xdg_toplevel *xdg_toplevel, s32 width, s32 height);
proc void _xdg_toplevel_wm_capabilities(rawptr data, struct xdg_toplevel *xdg_toplevel, struct wl_array *capabilities);

global const struct xdg_toplevel_listener _xdg_toplevel_listener = {
	.configure = _xdg_toplevel_configure,
	.close = _xdg_toplevel_close,
	.configure_bounds = _xdg_toplevel_configure_bounds,
	.wm_capabilities = _xdg_toplevel_wm_capabilities,
};

#endif // WM_LINUX_H
