
//--------------------------------------------------------------------------------
// wayland lnx procs

//--------------------------------------------------------------------------------
// wl registry

proc void
_wayland_registry_establish_interface(
	rawptr data, struct wl_registry *registry,
	u32 name, const char *interface, u32 version)
{
	//--------------------------------------------------------------------------------
	//register all the interfaces that we want

	b32 registered_interface = 0;
	string interface_string = make_str((cstring)interface, cstring_length((cstring)interface));
	if (str_match(S("wl_compositor"), interface_string, 0)) {
		wayland_state.compositor = (struct wl_compositor*)wl_registry_bind(registry, name, &wl_compositor_interface, version);
		registered_interface = 1;
	} else if (str_match(S("wl_shm"), interface_string, 0)) {
		wayland_state.shm = (struct wl_shm*)wl_registry_bind(registry, name, &wl_shm_interface, version);
		registered_interface = 1;
	} else if (str_match(S("xdg_wm_base"), interface_string, 0)) {
		wayland_state.wm = cast(struct xdg_wm_base*)wl_registry_bind(registry, name, &xdg_wm_base_interface, version);
		xdg_wm_base_add_listener(wayland_state.wm, &_xdg_wm_base_listener, 0);
		registered_interface = 1;
	} else if (str_match(S("wl_seat"), interface_string, 0)) {
		wayland_state.seat = cast(struct wl_seat*)wl_registry_bind(registry, name, &wl_seat_interface, version);
		wl_seat_add_listener(wayland_state.seat, &wayland_seat_listener, 0);
		registered_interface = 1;
	}

	if (registered_interface) {
		printf("registered interface: %.*s [v: %u n: %u]\n", SVARG(interface_string), version, name);
	}
}

proc void
_wayland_registry_unestablish_interface(rawptr data, struct wl_registry *registry, u32 name)
{
	// left blank
}

//--------------------------------------------------------------------------------
// wl buffers

proc void
_wayland_buffer_release(rawptr data, struct wl_buffer *buffer) {
	Wayland_Buffer *buf = (Wayland_Buffer*)data;
	if (buf->buffer == buffer) {
		buf->in_use = 0;
	} else {
		wl_buffer_destroy(buffer);
	}
}

//--------------------------------------------------------------------------------
// wl seat
proc void
_wayland_seat_capabilities(rawptr data, struct wl_seat *wl_seat, u32 capabilities) {
	b32 has_keyboard_support = !!(capabilities & WL_SEAT_CAPABILITY_KEYBOARD);
	b32 has_pointer_support = !!(capabilities & WL_SEAT_CAPABILITY_POINTER);
	b32 has_touch_support = !!(capabilities & WL_SEAT_CAPABILITY_TOUCH);
	printf("keyboard support: [%s] -- pointer support: [%s] -- touch support: [%s]\n",
		(has_keyboard_support) ? "true" : "false",
		(has_pointer_support) ? "true" : "false",
		(has_touch_support) ? "true" : "false");

	if (has_keyboard_support) {
		wayland_state.keyboard = wl_seat_get_keyboard(wayland_state.seat);
		wl_keyboard_add_listener(wayland_state.keyboard, &wayland_keyboard_listener, 0);
	} else if (!has_keyboard_support && wayland_state.keyboard != 0) {
		wayland_state.keyboard = 0;
	}
	if (has_pointer_support) {
		wayland_state.pointer = wl_seat_get_pointer(wayland_state.seat);
		wl_pointer_add_listener(wayland_state.pointer, &wayland_pointer_listener, 0);
	} else if (!has_pointer_support && wayland_state.pointer != 0) {
		wayland_state.pointer = 0;
	}
	if (has_touch_support) {
		wayland_state.touch = wl_seat_get_touch(wayland_state.seat);
		wl_touch_add_listener(wayland_state.touch, &wayland_touch_listener, 0);
	} else if (!has_touch_support && wayland_state.touch != 0) {
		wayland_state.touch = 0;
	}
}

proc void
_wayland_seat_name(rawptr data, struct wl_seat *wl_seat, const char *name) {
	printf("seat name: [%s]\n", name);
}

//--------------------------------------------------------------------------------
// wl keyboard

proc void
_wayland_keyboard_keymap(rawptr data, struct wl_keyboard *wl_keyboard, u32 format, int fd, u32 size) {
	assert(format == WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1);
	char *mapped = cast(char*)mmap(0, size, PROT_READ, MAP_SHARED, fd, 0);
	assert(mapped != MAP_FAILED);

	struct xkb_keymap *keymap = xkb_keymap_new_from_string(
		wayland_state.xkb_context, mapped,
		XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS
	);
	munmap(mapped, size);
	close(fd);

	struct xkb_state *state = xkb_state_new(keymap);
	xkb_keymap_unref(wayland_state.xkb_keymap);
	xkb_state_unref(wayland_state.xkb_state);
	wayland_state.xkb_keymap = keymap;
	wayland_state.xkb_state = state;
}

proc void
_wayland_keyboard_enter(
	rawptr data, struct wl_keyboard *wl_keyboard,
	u32 serial, struct wl_surface *surface,
	struct wl_array *keys)
{
	u32 *key;
	wl_array_for_each(key, keys) {
		u32 scancode = *key;
		// IButton *button = wayland_state.scancodes + scancode;
		// button->htc = 1;
		// button->down = 1;

		#if 0
		char buf[128];
		xkb_keysym_t sym = xkb_state_key_get_one_sym(wayland_state.xkb_state, *key+8);
		xkb_keysym_get_name(sym, buf, sizeof(buf));
		printf("sym: %-12s (%u), ", buf, sym);
		xkb_state_key_get_utf8(wayland_state.xkb_state, *key+8, buf, sizeof(buf));
		printf("utf8: '%s'\n", buf);
		#endif
	}
}

proc void
_wayland_keyboard_leave(rawptr data, struct wl_keyboard *wl_keyboard,
	u32 serial, struct wl_surface *surface)
{
}

proc void
_wayland_keyboard_key(rawptr data, struct wl_keyboard *wl_keyboard,
	    u32 serial, u32 time, u32 scancode, u32 state)
{
	b32 pressed = (state == WL_KEYBOARD_KEY_STATE_PRESSED);
	// IButton *button = wayland_state.scancodes + scancode;
	// button->htc += 1;
	// button->down = pressed;

#if 0
	char buf[128];
	u32 keycode = key+8;
	xkb_keysym_t sym = xkb_state_key_get_one_sym(wayland_state.xkb_state, keycode);
	xkb_keysym_get_name(sym, buf, sizeof(buf));
	cstring action = (state == WL_KEYBOARD_KEY_STATE_PRESSED) ? "pressed" : "released";
	printf("key: %s: keycode: %u sym: %-12s (%u), ", action, keycode, buf, sym);
	xkb_state_key_get_utf8(wayland_state.xkb_state, keycode, buf, sizeof(buf));
	printf("utf8: '%s'\n", buf);
#endif
}

proc void
_wayland_keyboard_modifiers(
	rawptr data, struct wl_keyboard *wl_keyboard,
	u32 serial, u32 mods_depressed,
	u32 mods_latched, u32 mods_locked,
	u32 group)
{
	xkb_state_update_mask(wayland_state.xkb_state, mods_depressed, mods_latched, mods_locked, 0, 0, group);
}

proc void
_wayland_keyboard_repeat_info(
	rawptr data, struct wl_keyboard *wl_keyboard,
	s32 rate, s32 delay)
{
	// TODO
}

//--------------------------------------------------------------------------------
// wl pointer

proc void
_wayland_pointer_enter(
	rawptr data, struct wl_pointer *wl_pointer,
	u32 serial, struct wl_surface *surface,
	wl_fixed_t surface_x, wl_fixed_t surface_y)
{
	Wayland_Pointer_Event *event = &wayland_state.pointer_event;
	event->flags |= Wayland_Pointer_Event_Enter;
	event->serial = serial;
	event->surface_x = surface_x;
	event->surface_y = surface_y;
}

proc void
_wayland_pointer_leave(
	rawptr data, struct wl_pointer *wl_pointer,
	u32  serial, struct wl_surface *surface)
{
	Wayland_Pointer_Event *event = &wayland_state.pointer_event;
	event->flags |= Wayland_Pointer_Event_Leave;
	event->serial = serial;
}

proc void
_wayland_pointer_motion(
	rawptr data, struct wl_pointer *wl_pointer, u32 time,
	wl_fixed_t surface_x, wl_fixed_t surface_y)
{
	Wayland_Pointer_Event *event = &wayland_state.pointer_event;
	event->flags |= Wayland_Pointer_Event_Motion;
	event->time = time;
	event->surface_x = surface_x;
	event->surface_y = surface_y;
}

proc void
_wayland_pointer_button(
	rawptr data, struct wl_pointer *wl_pointer,
	u32 serial, u32 time, u32 button, u32 state)
{
	Wayland_Pointer_Event *event = &wayland_state.pointer_event;
	event->flags |= Wayland_Pointer_Event_Button;
	event->time = time;
	event->serial = serial;
	event->button = button;
	event->state = state;
}

proc void
_wayland_pointer_axis(
	rawptr data, struct wl_pointer *wl_pointer,
	u32 time, u32 axis, wl_fixed_t value)
{
	Wayland_Pointer_Event *event = &wayland_state.pointer_event;
	event->flags |= Wayland_Pointer_Event_Axis;
	event->time = time;
	event->axes[axis].valid = 1;
	event->axes[axis].value = value;
}

proc void
_wayland_pointer_axis_source(rawptr data, struct wl_pointer *wl_pointer, u32 axis_source) {
	Wayland_Pointer_Event *event = &wayland_state.pointer_event;
	event->flags |= Wayland_Pointer_Event_Axis_Source;
	event->axis_source = axis_source;
}

proc void
_wayland_pointer_axis_stop(rawptr data, struct wl_pointer *wl_pointer, u32 time, u32 axis) {
	Wayland_Pointer_Event *event = &wayland_state.pointer_event;
	event->flags |= Wayland_Pointer_Event_Axis_Stop;
	event->time = time;
	event->axes[axis].valid = 1;
}

proc void
_wayland_pointer_axis_discrete(rawptr data, struct wl_pointer *wl_pointer, u32 axis, s32 discrete) {
	Wayland_Pointer_Event *event = &wayland_state.pointer_event;
	event->flags |= Wayland_Pointer_Event_Axis_Discrete;
	event->axes[axis].valid = 1;
	event->axes[axis].discrete = discrete;
}

proc void
_wayland_pointer_axis_value120(rawptr data, struct wl_pointer *wl_pointer, u32 axis, s32 value120) {
	// TODO
}

proc void
_wayland_pointer_axis_relative_direction(rawptr data, struct wl_pointer *wl_pointer, u32 axis, u32 direction) {
	// TODO
}

proc void
_wayland_pointer_frame(rawptr data, struct wl_pointer *wl_pointer) {
	Wayland_Pointer_Event *event = &wayland_state.pointer_event;
	#if 0
	printf("pointer frame: @ %u: ", event->time);
	if (event->flags & Wayland_Pointer_Event_Enter) {
		printf("entered: (%.2f, %.2f) ",
			wl_fixed_to_double(event->surface_x),
			wl_fixed_to_double(event->surface_y));
	}
	if (event->flags & Wayland_Pointer_Event_Leave) {
		printf("leave ");
	}
	if (event->flags & Wayland_Pointer_Event_Motion) {
		printf("motion: (%.2f, %.2f) ",
			wl_fixed_to_double(event->surface_x),
			wl_fixed_to_double(event->surface_y));
	}
	if (event->flags & Wayland_Pointer_Event_Button) {
		cstring state = (event->state == WL_POINTER_BUTTON_STATE_RELEASED) ?
			"released" : "pressed";
		printf("button: %u %s ", event->button, state);
	}

	$wayland_pointer_event_flags axis_events =
		Wayland_Pointer_Event_Axis | Wayland_Pointer_Event_Axis_Source |
		Wayland_Pointer_Event_Axis_Stop | Wayland_Pointer_Event_Axis_Discrete;
	cstring axis_names[$axis2_count] = {
		[WL_POINTER_AXIS_VERTICAL_SCROLL] = "vertical",
		[WL_POINTER_AXIS_HORIZONTAL_SCROLL] = "horizontal",
	};
	cstring axis_source[4] = {
		[WL_POINTER_AXIS_SOURCE_WHEEL] = "wheel",
		[WL_POINTER_AXIS_SOURCE_FINGER] = "finger",
		[WL_POINTER_AXIS_SOURCE_CONTINUOUS] = "continuous",
		[WL_POINTER_AXIS_SOURCE_WHEEL_TILT] = "wheel tilt",
	};

	if (event->flags & axis_events) {
		for each_enum_val($axis2, axis) {
			if (!event->axes[axis].valid) {
				continue;
			}

			printf("%s axis ", axis_names[axis]);
			if (event->flags & Wayland_Pointer_Event_Axis) {
				printf("value %.2f ", wl_fixed_to_double(event->axes[axis].value));
			}
			if (event->flags & Wayland_Pointer_Event_Axis_Discrete) {
				printf("discrete %i ", event->axes[axis].discrete);
			}
			if (event->flags & Wayland_Pointer_Event_Axis_Source) {
				printf("via %s ", axis_source[event->axis_source]);
			}
			if (event->flags & Wayland_Pointer_Event_Axis_Stop) {
				printf("(stopped) ");
			}
		}
	}
	printf("\n");
	#endif
	memory_zero_struct(event);
}

//--------------------------------------------------------------------------------
// wl touch

proc void
_wayland_touch_down(rawptr data, struct wl_touch *wl_touch, u32 serial, u32 time, struct wl_surface *surface, s32 id, wl_fixed_t x, wl_fixed_t y) {
	// TODO
}
proc void
_wayland_touch_up(rawptr data, struct wl_touch *wl_touch, u32 serial, u32 time, s32 id) {
	// TODO
}
proc void
_wayland_touch_motion(rawptr data, struct wl_touch *wl_touch, u32 time, s32 id, wl_fixed_t x, wl_fixed_t y) {
	// TODO
}
proc void
_wayland_touch_frame(rawptr data, struct wl_touch *wl_touch) {
	// TODO
}
proc void
_wayland_touch_cancel(rawptr data, struct wl_touch *wl_touch) {
	// TODO
}
proc void
_wayland_touch_shape(rawptr data, struct wl_touch *wl_touch, s32 id, wl_fixed_t major, wl_fixed_t minor) {
	// TODO
}
proc void
_wayland_touch_orientation(rawptr data, struct wl_touch *wl_touch, s32 id, wl_fixed_t orientation) {
	// TODO
}

//--------------------------------------------------------------------------------
// xdg wm base

proc void
_xdg_wm_base_ping(rawptr data, struct xdg_wm_base *wm, u32 serial) {
	xdg_wm_base_pong(wm, serial);
}

//--------------------------------------------------------------------------------
// xdg surface

proc void
_xdg_surface_configure(rawptr data, struct xdg_surface *surface, u32 serial) {
	xdg_surface_ack_configure(surface, serial);

	assert(data != 0);
	Wayland_Window *window = data;
	wl_surface_commit(window->surface);
}

//--------------------------------------------------------------------------------
// xdg toplevel

proc void
_xdg_toplevel_configure(
	rawptr data, struct xdg_toplevel *xdg_toplevel,
	s32 width, s32 height, struct wl_array *states)
{
	Wayland_Window *window = data;
	vec2s32 new_dim = (width && height) ? v2s32(width, height) : v2s32(1, 1);
	assert(new_dim.x <= window->max_dim.x && new_dim.y <= window->max_dim.y);
	window->dim = new_dim;
	printf("xdg toplevel configure: [%i %i]\n", new_dim.x, new_dim.y);

	for each_index(i, window->buffer_count) {
		Wayland_Buffer *buffer = window->buffers + i;
		if (!buffer->in_use && buffer->buffer) {
			wl_buffer_destroy(buffer->buffer);
		}

		buffer->buffer = wl_shm_pool_create_buffer(
			window->pool, buffer->offset,
			new_dim.x, new_dim.y, window->max_stride, WL_SHM_FORMAT_XRGB8888);
		wl_buffer_add_listener(buffer->buffer, &wayland_buffer_listener, buffer);
	}
}

proc void
_xdg_toplevel_close(rawptr data, struct xdg_toplevel *xdg_toplevel) {
	// TODO Handle window closing
	Wayland_Window *window = data;
	window->closed = 1;
}

proc void
_xdg_toplevel_configure_bounds(
	rawptr data, struct xdg_toplevel *xdg_toplevel,
	s32 width, s32 height)
{
	// TODO handle configure bounds
}

proc void
_xdg_toplevel_wm_capabilities(rawptr data,
	struct xdg_toplevel *xdg_toplevel,
	struct wl_array *capabilities)
{
	// TODO handle wm capabilities
}

//--------------------------------------------------------------------------------
// LAYER IMPLEMENTATION

//--------------------------------------------------------------------------------
// layer initialization

proc void
wm_init(void) {
	//--------------------------------------------------------------------------------
	// get display
	wayland_state.display = wl_display_connect(0);
	if (!wayland_state.display) {
		printf("Failed to connect to wayland display.\n");
		return;
	}
	printf("Wayland display connection established.\n");

	{
		//--------------------------------------------------------------------------------
		// register objects
		wayland_state.registry = wl_display_get_registry(wayland_state.display);
		wl_registry_add_listener(wayland_state.registry, &wayland_registry_listener, 0);
		wl_display_roundtrip(wayland_state.display);
		assert(wayland_state.display && wayland_state.registry && wayland_state.compositor);

		//--------------------------------------------------------------------------------
		// xkb keyboard
		wayland_state.xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
	}
}

//--------------------------------------------------------------------------------
// events

proc void
wm_consume_events(void)
{
		while (wl_display_prepare_read(wayland_state.display) != 0) {
			wl_display_dispatch_pending(wayland_state.display);
		}
		wl_display_flush(wayland_state.display);
		wl_display_read_events(wayland_state.display);
		wl_display_dispatch_pending(wayland_state.display);
}

//--------------------------------------------------------------------------------
// input

// proc IButton_Array
// wm_consume_keyboard_scancodes(Arena *arena)
// {
// 	IButton_Array array;
// 	array.v = push_array(arena, IButton, array_count(wayland_state.scancodes));
// 	array.count = array_count(wayland_state.scancodes);
// 	memory_copy(array.v, wayland_state.scancodes, array_count(wayland_state.scancodes)*sizeof(IButton));
//
// 	//--------------------------------------------------------------------------------
// 	// consume input
// 	for each_element(i, wayland_state.scancodes) {
// 		IButton *btn = wayland_state.scancodes + i;
// 		btn->htc = 0;
// 	}
//
// 	return array;
// }

//--------------------------------------------------------------------------------
// window

proc WM_Window
wm_window_open(Arena *arena, string title)
{
	Temp scr = scratch_begin(&arena, 1);
	Wayland_Window *window = push_array(arena, Wayland_Window, 1);

	//--------------------------------------------------------------------------------
	// create a window surface && pixel buffers
	window->surface = wl_compositor_create_surface(wayland_state.compositor);
	window->xdg_surface = xdg_wm_base_get_xdg_surface(wayland_state.wm, window->surface);
	window->toplevel = xdg_surface_get_toplevel(window->xdg_surface);

	xdg_surface_add_listener(window->xdg_surface, &_xdg_surface_listener, window);
	xdg_toplevel_add_listener(window->toplevel, &_xdg_toplevel_listener, window);
	string title_copy = push_str_copy(scr.arena, title);
	xdg_toplevel_set_title(window->toplevel, title_copy.m);
	wl_surface_commit(window->surface);

	vec2s32 max_dim = scale_2s32(v2s32(1920, 1080), 4);
	window->buffer_count = WM_WINDOW_BACKBUFFER_COUNT;
	window->dim = v2s32(0, 0);
	window->title = title_copy;
	window->max_dim = max_dim;
	window->max_stride = max_dim.x*PIXEL_SIZE;

	s32 max_buffer_size = max_dim.y*window->max_stride;
	window->pool_size = max_buffer_size*window->buffer_count;

	// create random shared memory file name
	string random_name;
	{
		u64 time = os_time_us();
		$random_series series = mk_random_series(time);
		rng1s64 range = r1s64(0, 25);
		u8 buf[16];
		for each_element(i, buf) {
			buf[i] = 'A' + (u8)random_r1s64(&series, range);
		}
		string pre = S("/wl-lnx-shm-");
		string pst = str_array_fixed(buf);
		random_name = push_str_cat(scr.arena, pre, pst);
	}

	window->shared_memory_file = os_shared_memory_alloc(random_name, window->pool_size, (OS_File_Access_Flags)(OS_File_Access_Flag_Read_Write | OS_File_Access_Flag_Create));
	window->shared_memory = os_shared_memory_view_open(window->shared_memory_file, r1s64(0, window->pool_size));
	int fd = (int)window->shared_memory_file.v[0];
	window->pool = wl_shm_create_pool(wayland_state.shm, fd, window->pool_size);

	for each_index(i, window->buffer_count) {
		window->buffers[i].offset = (s32)i*max_buffer_size;
		window->buffers[i].in_use = 0;
		window->buffers[i].m = (rawptr)((u8*)window->shared_memory + window->buffers[i].offset);
	}

	WM_Window handle = {.v[0] = cast(u64)(window) };
	scratch_end(scr);
	return handle;
}

proc void
wm_window_close(WM_Window window) {
	assert(!handle_match(window, zero_handle(WM_Window)));

	Wayland_Window *w = cast(Wayland_Window*)(window.v[0]);
	os_shared_memory_view_close(w->shared_memory_file, w->shared_memory, r1s64(0, w->pool_size));
	os_shared_memory_close(w->shared_memory_file);

	wl_shm_pool_destroy(w->pool);
	xdg_toplevel_destroy(w->toplevel);
	xdg_surface_destroy(w->xdg_surface);
	wl_surface_destroy(w->surface);
}

proc vec2s32
wm_window_get_backbuffer_dim(WM_Window window) {
	assert(!handle_match(window, zero_handle(WM_Window)));

	Wayland_Window *w = cast(Wayland_Window*)(window.v[0]);
	return w->dim;
}

proc WM_Buffer
wm_window_get_unused_backbuffer(WM_Window window, /**/ rawptr *backbuffer, s64 *stride) {
	assert(!handle_match(window, zero_handle(WM_Window)));

	assert(backbuffer && stride);
	Wayland_Window *w = cast(Wayland_Window*)(window.v[0]);

	Wayland_Buffer *lnx_buffer = 0;
	for each_index(i, w->buffer_count) {
		Wayland_Buffer *at = w->buffers + i;
		if (!at->in_use && at->buffer != 0) {
			lnx_buffer = at;
			break;
		}
	}

	if (lnx_buffer) {
		*backbuffer = lnx_buffer->m;
		*stride = w->max_stride;
	}

	WM_Buffer handle = { .v[0] = cast(u64)lnx_buffer };
	return handle;
}

proc b32
wm_window_is_open(WM_Window window) {
	assert(!handle_match(window, zero_handle(WM_Window)));

	Wayland_Window *w = cast(Wayland_Window*)(window.v[0]);
	return !w->closed;
}

proc void
wm_window_commit_buffer(WM_Window window, WM_Buffer buffer_handle)
{
	assert(!handle_match(window, zero_handle(WM_Window)));
	assert(!handle_match(buffer_handle, zero_handle(WM_Buffer)));

	Wayland_Window *w = cast(Wayland_Window*)(window.v[0]);
	Wayland_Buffer *b = cast(Wayland_Buffer*)(buffer_handle.v[0]);
	b->in_use = 1;

	wl_surface_attach(w->surface, b->buffer, 0, 0);
	wl_surface_damage(w->surface, 0, 0, max_s32, max_s32);
	wl_surface_commit(w->surface);
}
