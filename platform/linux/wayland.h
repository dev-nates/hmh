#if WINDOW_BACKEND == WAYLAND

#pragma push_macro("global")
#undef global
#include "wayland-client.h"

#include "relative-pointer.h"
#include "relative-pointer.c"

#include "xdg-shell.h"
#include "xdg-shell.c"
#include "xkbcommon/xkbcommon.h"
#pragma pop_macro("global")

// -------------------------------------------------------------------------------------------------
// Window

#define BYTES_PER_PIXEL 4
typedef struct Wayland_Buffer Wayland_Buffer;
struct Wayland_Buffer {
	struct wl_buffer *handle;
	rawptr memory;
	s64 offset;
	b8 in_use;
};

#define WAYLAND_WINDOW_BUFFER_COUNT 3
#define WAYLAND_WINDOW_MAX_BUFFER_SIZE megabytes(16)
typedef struct Wayland_Window Wayland_Window;
struct Wayland_Window {
	u8 arena_backing_storage[128];
	Arena *arena;
	vec2s32 size;
	string8 title;
	b8 closed;

	struct wl_surface *surface;
	struct xdg_surface *xdg_surface;
	struct xdg_toplevel *toplevel;
	struct wl_shm_pool *pool;
	s64 pool_size;

	string8 shared_memory_name;
	Linux_Shared_Memory shared_memory_file;
	rawptr shared_memory;
	Wayland_Buffer buffers[WAYLAND_WINDOW_BUFFER_COUNT];


	u32 latest_pointer_enter_serial;
	struct {
		b8 has_source;
		enum wl_pointer_axis_source source;
		s32 value;
		s32 value120;
		u64 time;
	} wheel;
};

// -------------------------------------------------------------------------------------------------
// State

typedef struct Wayland_State Wayland_State;
struct Wayland_State {
	struct wl_display *display;
	struct wl_registry *registry;
	struct wl_compositor *compositor;
	struct wl_shm *shm;
	struct xdg_wm_base *wm;
	struct zwp_relative_pointer_manager_v1 *relative_pointer_manager;
	
	struct wl_seat *seat;
	struct wl_keyboard *keyboard;
	struct wl_pointer *pointer;
	struct wl_touch *touch;
	struct zwp_relative_pointer_v1 *relative_pointer;

	struct xkb_state   *xkb_state;
	struct xkb_context *xkb_context;
	struct xkb_keymap  *xkb_keymap;
};

global Wayland_Window main_window = zero_struct;
global Wayland_State  wayland_state = zero_struct;

// -------------------------------------------------------------------------------------------------
// Registry

proc void
register_interface(rawptr data, struct wl_registry *registry, u32 name, const char *interface, u32 version);

proc void
unregister_interface(rawptr data, struct wl_registry *registry, u32 name);

#pragma push_macro("global");
#undef global
static const struct wl_registry_listener registry_listener = {
	.global        = register_interface,
	.global_remove = unregister_interface,
};
#pragma pop_macro("global");

//--------------------------------------------------------------------------------
// Buffer

proc void
buffer_reuse(rawptr data, struct wl_buffer *buffer);

static const struct wl_buffer_listener buffer_listener = {
	.release = buffer_reuse,
};

//--------------------------------------------------------------------------------
// Seat

proc void
seat_capabilities(rawptr data, struct wl_seat *wl_seat, u32 capabilities);

proc void
seat_name(rawptr data, struct wl_seat *wl_seat, const char *name);

static const struct wl_seat_listener seat_listener = {
	.capabilities = seat_capabilities,
	.name         = seat_name,
};

// -------------------------------------------------------------------------------------------------
// Keyboard

proc void
keyboard_keymap(rawptr data, struct wl_keyboard *wl_keyboard, u32 format, int fd, u32 size);

proc void
keyboard_enter(rawptr data, struct wl_keyboard *wl_keyboard, u32 serial, struct wl_surface *surface, struct wl_array *keys);

proc void
keyboard_leave(rawptr data, struct wl_keyboard *wl_keyboard, u32 serial, struct wl_surface *surface);


proc void
keyboard_key(rawptr data, struct wl_keyboard *wl_keyboard, u32 serial, u32 time, u32 scancode, u32 state);

proc void
keyboard_modifiers(rawptr data, struct wl_keyboard *wl_keyboard, u32 serial, u32 mods_depressed, u32 mods_latched, u32 mods_locked, u32 group);

proc void
keyboard_repeat_info(rawptr data, struct wl_keyboard *wl_keyboard, s32 rate, s32 delay);

static const struct wl_keyboard_listener keyboard_listener = {
	.keymap      = keyboard_keymap,
	.enter       = keyboard_enter,
	.leave       = keyboard_leave,
	.key         = keyboard_key,
	.modifiers   = keyboard_modifiers,
	.repeat_info = keyboard_repeat_info,
};

//--------------------------------------------------------------------------------
// Pointer

proc void
pointer_enter(rawptr data, struct wl_pointer *wl_pointer, u32 serial, struct wl_surface *surface, wl_fixed_t surface_x, wl_fixed_t surface_y);

proc void
pointer_leave(rawptr data, struct wl_pointer *wl_pointer, u32  serial, struct wl_surface *surface);

proc void
pointer_motion(rawptr data, struct wl_pointer *wl_pointer, u32 time, wl_fixed_t surface_x, wl_fixed_t surface_y);

proc void
pointer_button(rawptr data, struct wl_pointer *wl_pointer, u32 serial, u32 time, u32 button, u32 state);

proc void
pointer_axis_source(rawptr data, struct wl_pointer *wl_pointer, u32 axis_source);

proc void
pointer_axis(rawptr data, struct wl_pointer *wl_pointer, u32 time, u32 axis, wl_fixed_t value);

proc void
pointer_axis_discrete(rawptr data, struct wl_pointer *wl_pointer, u32 axis, s32 discrete);

proc void
pointer_axis_value120(rawptr data, struct wl_pointer *wl_pointer, u32 axis, s32 value120);

proc void
pointer_frame(rawptr data, struct wl_pointer *wl_pointer);

proc void
pointer_axis_stop(rawptr data, struct wl_pointer *wl_pointer, u32 time, u32 axis);

proc void
pointer_axis_relative_direction(rawptr data, struct wl_pointer *wl_pointer, u32 axis, u32 direction);

static const struct wl_pointer_listener pointer_listener = {
	.enter                   = pointer_enter,
	.leave                   = pointer_leave,
	.motion                  = pointer_motion,
	.button                  = pointer_button,
	.axis_source             = pointer_axis_source,
	.axis                    = pointer_axis,
	.axis_stop               = pointer_axis_stop,
	.axis_discrete           = pointer_axis_discrete,
	.axis_value120           = pointer_axis_value120,
	.axis_relative_direction = pointer_axis_relative_direction,
	.frame                   = pointer_frame,
};

// -------------------------------------------------------------------------------------------------
// Relative pointer

proc void
relative_pointer_motion(rawptr data, struct zwp_relative_pointer_v1 *relative_pointer, u32 utime_hi, u32 utime_lo, wl_fixed_t dx, wl_fixed_t dy, wl_fixed_t dx_unaccel, wl_fixed_t dy_unaccel);

static const struct zwp_relative_pointer_v1_listener relative_pointer_listener = {
	.relative_motion =  relative_pointer_motion,
};

// -------------------------------------------------------------------------------------------------
// XDG WM Base

proc void
xdg_wm_base_ping(rawptr data, struct xdg_wm_base *wm, u32 serial);

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
	.ping = xdg_wm_base_ping,
};

//--------------------------------------------------------------------------------
// XDG Surface

proc void
xdg_surface_configure(rawptr data, struct xdg_surface *surface, u32 serial);

static const struct xdg_surface_listener xdg_surface_listener = {
	.configure = xdg_surface_configure,
};

// -------------------------------------------------------------------------------------------------
// XDG Toplevel

proc void
xdg_toplevel_configure(rawptr data, struct xdg_toplevel *xdg_toplevel, s32 width, s32 height, struct wl_array *states);

proc void
xdg_toplevel_close(rawptr data, struct xdg_toplevel *xdg_toplevel);

proc void
xdg_toplevel_configure_bounds(rawptr data, struct xdg_toplevel *xdg_toplevel, s32 width, s32 height);

proc void
xdg_toplevel_wm_capabilities(rawptr data, struct xdg_toplevel *xdg_toplevel, struct wl_array *capabilities);

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
	.configure = xdg_toplevel_configure,
	.close = xdg_toplevel_close,
	.configure_bounds = xdg_toplevel_configure_bounds,
	.wm_capabilities = xdg_toplevel_wm_capabilities,
};

#endif

