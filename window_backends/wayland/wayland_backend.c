

// -------------------------------------------------------------------------------------------------
// Registry

proc void
wayland_registry_register_interface(
	rawptr data, struct wl_registry *registry,
	u32 name, const char *interface, u32 version)
{
	//--------------------------------------------------------------------------------
	//register all the interfaces that we want

	b32 registered_interface = 0;
	string8 interface_string = make_str((cstring)interface, cstring_length((cstring)interface));
	if (str_match(S("wl_compositor"), interface_string, 0)) {
		wayland_state.compositor = (struct wl_compositor*)wl_registry_bind(registry, name, &wl_compositor_interface, version);
		registered_interface = 1;
	} else if (str_match(S("wl_shm"), interface_string, 0)) {
		wayland_state.shm = (struct wl_shm*)wl_registry_bind(registry, name, &wl_shm_interface, version);
		registered_interface = 1;
	} else if (str_match(S("xdg_wm_base"), interface_string, 0)) {
		wayland_state.wm = cast(struct xdg_wm_base*)wl_registry_bind(registry, name, &xdg_wm_base_interface, version);
		xdg_wm_base_add_listener(wayland_state.wm, &xdg_wm_base_listener, 0);
		registered_interface = 1;
	} else if (str_match(S("wl_seat"), interface_string, 0)) {
		wayland_state.seat = cast(struct wl_seat*)wl_registry_bind(registry, name, &wl_seat_interface, version);
		wl_seat_add_listener(wayland_state.seat, &wayland_seat_listener, 0);
		registered_interface = 1;
	}

#if LOG_WINDOW_BACKEND
	if (registered_interface) {
		infof(WINDOW_BACKEND_LOG_COLOR "Registered interface: " TC_END "%.*s [v: %u n: %u]", svarg(interface_string), version, name);
	}
#endif
}

proc void
wayland_registry_unregister_interface(rawptr data, struct wl_registry *registry, u32 name) {
	// left blank
}

//--------------------------------------------------------------------------------
// Buffers

proc void
wayland_buffer_release(rawptr data, struct wl_buffer *buffer) {
	Wayland_Buffer *buf = (Wayland_Buffer*)data;

	buf->in_use = false;
	if (buf->handle == buffer) {
		// Reuse buffer
#if 0
		debugf("Reuse buffer: [%p]", buf->handle);
#endif
	} else {
#if 0
		debugf("Release buffer: [%p]", buf->handle);
#endif
		wl_buffer_destroy(buffer);
	}
}

//--------------------------------------------------------------------------------
// wl seat
proc void
wayland_seat_capabilities(rawptr data, struct wl_seat *wl_seat, u32 capabilities) {
	b32 has_keyboard_support = !!(capabilities & WL_SEAT_CAPABILITY_KEYBOARD);
	b32 has_pointer_support = !!(capabilities & WL_SEAT_CAPABILITY_POINTER);
	b32 has_touch_support = !!(capabilities & WL_SEAT_CAPABILITY_TOUCH);

#if LOG_WINDOW_BACKEND
	infof(WINDOW_BACKEND_LOG_COLOR "Seat Capabilities" TC_END " -- keyboard support: [%s] -- pointer support: [%s] -- touch support: [%s]",
		(has_keyboard_support) ? "true" : "false",
		(has_pointer_support) ? "true" : "false",
		(has_touch_support) ? "true" : "false");
#endif

	Wayland_Window *window = &wayland_state.main_window;
	if (has_keyboard_support) {
		wayland_state.keyboard = wl_seat_get_keyboard(wayland_state.seat);
		wl_keyboard_add_listener(wayland_state.keyboard, &wayland_keyboard_listener, window);
	} else if (!has_keyboard_support && wayland_state.keyboard != 0) {
		wayland_state.keyboard = 0;
	}
	if (has_pointer_support) {
		wayland_state.pointer = wl_seat_get_pointer(wayland_state.seat);
		wl_pointer_add_listener(wayland_state.pointer, &wayland_pointer_listener, window);
	} else if (!has_pointer_support && wayland_state.pointer != 0) {
		wayland_state.pointer = 0;
	}
	if (has_touch_support) {
		wayland_state.touch = wl_seat_get_touch(wayland_state.seat);
		wl_touch_add_listener(wayland_state.touch, &wayland_touch_listener, window);
	} else if (!has_touch_support && wayland_state.touch != 0) {
		wayland_state.touch = 0;
	}
}

proc void
wayland_seat_name(rawptr data, struct wl_seat *wl_seat, const char *name) {
#if LOG_WINDOW_BACKEND
	infof(WINDOW_BACKEND_LOG_COLOR "Seat Name" TC_END ": [%s]", name);
#endif
}

//--------------------------------------------------------------------------------
// wl keyboard

proc void
wayland_keyboard_keymap(rawptr data, struct wl_keyboard *wl_keyboard, u32 format, int fd, u32 size) {
#if 0
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
#endif
}

proc void
wayland_keyboard_enter(
	rawptr data, struct wl_keyboard *wl_keyboard,
	u32 serial, struct wl_surface *surface,
	struct wl_array *keys)
{
#if 0
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
#endif
}

proc void
wayland_keyboard_leave(rawptr data, struct wl_keyboard *wl_keyboard,
	u32 serial, struct wl_surface *surface)
{
}

proc void
wayland_keyboard_key(rawptr data, struct wl_keyboard *wl_keyboard,
	    u32 serial, u32 time, u32 scancode, u32 state)
{
#if 0
	b32 pressed = (state == WL_KEYBOARD_KEY_STATE_PRESSED);
	// IButton *button = wayland_state.scancodes + scancode;
	// button->htc += 1;
	// button->down = pressed;

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
wayland_keyboard_modifiers(
	rawptr data, struct wl_keyboard *wl_keyboard,
	u32 serial, u32 mods_depressed,
	u32 mods_latched, u32 mods_locked,
	u32 group)
{
#if 0
	xkb_state_update_mask(wayland_state.xkb_state, mods_depressed, mods_latched, mods_locked, 0, 0, group);
#endif
}

proc void
wayland_keyboard_repeat_info(
	rawptr data, struct wl_keyboard *wl_keyboard,
	s32 rate, s32 delay)
{
	// TODO
}

//--------------------------------------------------------------------------------
// wl pointer

proc void
wayland_pointer_enter(
	rawptr data, struct wl_pointer *wl_pointer,
	u32 serial, struct wl_surface *surface,
	wl_fixed_t surface_x, wl_fixed_t surface_y)
{
	Wayland_Window *window = cast(Wayland_Window*)(data);
	Wayland_Pointer_State *state = &window->pointer_state;
	state->flags |= Wayland_Pointer_Event_Enter;
	state->serial = serial;
	state->surface_x = surface_x;
	state->surface_y = surface_y;
}

proc void
wayland_pointer_leave(
	rawptr data, struct wl_pointer *wl_pointer,
	u32  serial, struct wl_surface *surface)
{
	Wayland_Window *window = cast(Wayland_Window*)(data);
	Wayland_Pointer_State *state = &window->pointer_state;
	state->flags |= Wayland_Pointer_Event_Leave;
	state->serial = serial;
}

proc void
wayland_pointer_motion(
	rawptr data, struct wl_pointer *wl_pointer, u32 time,
	wl_fixed_t surface_x, wl_fixed_t surface_y
) {
	Wayland_Window *window = cast(Wayland_Window*)(data);
	Wayland_Pointer_State *state = &window->pointer_state;
	state->flags |= Wayland_Pointer_Event_Motion;
	state->time = time;
	state->surface_x = surface_x;
	state->surface_y = surface_y;
}

proc void
wayland_pointer_button(
	rawptr data, struct wl_pointer *wl_pointer,
	u32 serial, u32 time, u32 button, u32 state
) {
	Wayland_Window *window = cast(Wayland_Window*)(data);
	Wayland_Pointer_State *pointer_state = &window->pointer_state;
	pointer_state->flags |= Wayland_Pointer_Event_Button;
	pointer_state->time = time;
	pointer_state->serial = serial;
	pointer_state->button = button;
	pointer_state->state = state;
}

proc void
wayland_pointer_axis(
	rawptr data, struct wl_pointer *wl_pointer,
	u32 time, u32 axis, wl_fixed_t value
) {
	Wayland_Window *window = cast(Wayland_Window*)(data);
	Wayland_Pointer_State *state = &window->pointer_state;
	state->flags |= Wayland_Pointer_Event_Axis;
	state->time = time;
	state->axes[axis].valid = 1;
	state->axes[axis].value = value;
}

proc void
wayland_pointer_axis_source(rawptr data, struct wl_pointer *wl_pointer, u32 axis_source) {
	Wayland_Window *window = cast(Wayland_Window*)(data);
	Wayland_Pointer_State *state = &window->pointer_state;
	state->flags |= Wayland_Pointer_Event_Axis_Source;
	state->axis_source = axis_source;
}

proc void
wayland_pointer_axis_stop(rawptr data, struct wl_pointer *wl_pointer, u32 time, u32 axis) {
	Wayland_Window *window = cast(Wayland_Window*)(data);
	Wayland_Pointer_State *state = &window->pointer_state;
	state->flags |= Wayland_Pointer_Event_Axis_Stop;
	state->time = time;
	state->axes[axis].valid = 1;
}

proc void
wayland_pointer_axis_discrete(rawptr data, struct wl_pointer *wl_pointer, u32 axis, s32 discrete) {
	Wayland_Window *window = cast(Wayland_Window*)(data);
	Wayland_Pointer_State *state = &window->pointer_state;
	state->flags |= Wayland_Pointer_Event_Axis_Discrete;
	state->axes[axis].valid = 1;
	state->axes[axis].discrete = discrete;
}

proc void
wayland_pointer_axis_value120(rawptr data, struct wl_pointer *wl_pointer, u32 axis, s32 value120) {
	// TODO
}

proc void
wayland_pointer_axis_relative_direction(rawptr data, struct wl_pointer *wl_pointer, u32 axis, u32 direction) {
	// TODO
}

proc void
wayland_pointer_frame(rawptr data, struct wl_pointer *wl_pointer) {
	Wayland_Window *window = cast(Wayland_Window*)(data);
	Wayland_Pointer_State *state = &window->pointer_state;
	#if 0
	printf("pointer frame: @ %u: ", state->time);
	if (state->flags & Wayland_Pointer_Event_Enter) {
		printf("entered: (%.2f, %.2f) ",
			wl_fixed_to_double(state->surface_x),
			wl_fixed_to_double(state->surface_y));
	}
	if (state->flags & Wayland_Pointer_Event_Leave) {
		printf("leave ");
	}
	if (state->flags & Wayland_Pointer_Event_Motion) {
		printf("motion: (%.2f, %.2f) ",
			wl_fixed_to_double(state->surface_x),
			wl_fixed_to_double(state->surface_y));
	}
	if (state->flags & Wayland_Pointer_Event_Button) {
		cstring state = (state->state == WL_POINTER_BUTTON_STATE_RELEASED) ?
			"released" : "pressed";
		printf("button: %u %s ", state->button, state);
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

	if (state->flags & axis_events) {
		for each_enum_val($axis2, axis) {
			if (!state->axes[axis].valid) {
				continue;
			}

			printf("%s axis ", axis_names[axis]);
			if (state->flags & Wayland_Pointer_Event_Axis) {
				printf("value %.2f ", wl_fixed_to_double(state->axes[axis].value));
			}
			if (state->flags & Wayland_Pointer_Event_Axis_Discrete) {
				printf("discrete %i ", state->axes[axis].discrete);
			}
			if (state->flags & Wayland_Pointer_Event_Axis_Source) {
				printf("via %s ", axis_source[state->axis_source]);
			}
			if (state->flags & Wayland_Pointer_Event_Axis_Stop) {
				printf("(stopped) ");
			}
		}
	}
	printf("\n");
	#endif
	memory_zero_struct(state);
}

//--------------------------------------------------------------------------------
// wl touch

proc void
wayland_touch_down(rawptr data, struct wl_touch *wl_touch, u32 serial, u32 time, struct wl_surface *surface, s32 id, wl_fixed_t x, wl_fixed_t y) {
	// TODO
}
proc void
wayland_touch_up(rawptr data, struct wl_touch *wl_touch, u32 serial, u32 time, s32 id) {
	// TODO
}
proc void
wayland_touch_motion(rawptr data, struct wl_touch *wl_touch, u32 time, s32 id, wl_fixed_t x, wl_fixed_t y) {
	// TODO
}
proc void
wayland_touch_frame(rawptr data, struct wl_touch *wl_touch) {
	// TODO
}
proc void
wayland_touch_cancel(rawptr data, struct wl_touch *wl_touch) {
	// TODO
}
proc void
wayland_touch_shape(rawptr data, struct wl_touch *wl_touch, s32 id, wl_fixed_t major, wl_fixed_t minor) {
	// TODO
}
proc void
wayland_touch_orientation(rawptr data, struct wl_touch *wl_touch, s32 id, wl_fixed_t orientation) {
	// TODO
}

//--------------------------------------------------------------------------------
// xdg wm base

proc void
xdg_wm_base_ping(rawptr data, struct xdg_wm_base *wm, u32 serial) {
	xdg_wm_base_pong(wm, serial);
}

//--------------------------------------------------------------------------------
// xdg surface

proc void
xdg_surface_configure(rawptr data, struct xdg_surface *surface, u32 serial) {
	xdg_surface_ack_configure(surface, serial);

	assert(data != 0);
	Wayland_Window *window = data;
	wl_surface_commit(window->surface);
}

//--------------------------------------------------------------------------------
// xdg toplevel

proc void
xdg_toplevel_configure(
	rawptr data, struct xdg_toplevel *xdg_toplevel,
	s32 width, s32 height, struct wl_array *states)
{
	Wayland_Window *window = data;

	vec2s32 new_size = v2s32(width, height);
	if (new_size.x == 0 || new_size.y == 0) { return; }
	if (window->size.x == new_size.x && window->size.y == new_size.y) { return; }

#if LOG_WINDOW_BACKEND
	infof(WINDOW_BACKEND_LOG_COLOR "Window resize: " TC_END TC_WHITE "[%d %d]" TC_END, new_size.x, new_size.y);
#endif
	window->size = new_size;
	for (each_index(i, WAYLAND_WINDOW_BUFFER_COUNT)) {
		Wayland_Buffer *buffer = window->buffers + i;
		if (buffer->in_use == false && buffer->handle != nil) {
			wl_buffer_destroy(buffer->handle);
		}

		s32 stride = new_size.x*BYTES_PER_PIXEL;
		buffer->handle = wl_shm_pool_create_buffer(
			window->pool, cast(int)buffer->offset,
			new_size.x, new_size.y, stride,
			WL_SHM_FORMAT_XRGB8888
		);
		wl_buffer_add_listener(buffer->handle, &wayland_buffer_listener, buffer);
	}
}

proc void
xdg_toplevel_close(rawptr data, struct xdg_toplevel *xdg_toplevel) {
	// TODO Handle window closing
	Wayland_Window *window = data;
	window->closed = 1;
}

proc void
xdg_toplevel_configure_bounds(
	rawptr data, struct xdg_toplevel *xdg_toplevel,
	s32 width, s32 height)
{
	// TODO handle configure bounds
}

proc void
xdg_toplevel_wm_capabilities(rawptr data,
	struct xdg_toplevel *xdg_toplevel,
	struct wl_array *capabilities)
{
	// TODO handle wm capabilities
}

//--------------------------------------------------------------------------------
// Implementation

proc void
window_backend_open(string8 window_title) {
	Temp scr = scratch_begin(0, 0);

	// -------------------------------------------------------------------------------------------------
	// Connect to display
	wayland_state.display = wl_display_connect(nil);
	if (wayland_state.display == nil) {
		errorf("Failed to connect to wayland display server.");
		return;
	}
	infof("Connected to " WINDOW_BACKEND_LOG_COLOR "wayland" TC_END " display server.");

	// -------------------------------------------------------------------------------------------------
	// Register objects
	wayland_state.registry = wl_display_get_registry(wayland_state.display);
	wl_registry_add_listener(wayland_state.registry, &wayland_registry_listener, 0);
	wl_display_roundtrip(wayland_state.display);
	assert(wayland_state.display != nil);
	assert(wayland_state.registry != nil);
	assert(wayland_state.compositor != nil);
	
	// -------------------------------------------------------------------------------------------------
	// @Todo: XKB Keyboard
	
	// -------------------------------------------------------------------------------------------------
	// Main Window
	Wayland_Window *window = &wayland_state.main_window;
	window->arena = arena_from_fixed(window->arena_backing_storage);

	window->surface = wl_compositor_create_surface(wayland_state.compositor);
	window->xdg_surface = xdg_wm_base_get_xdg_surface(wayland_state.wm, window->surface);
	window->toplevel = xdg_surface_get_toplevel(window->xdg_surface);

	xdg_surface_add_listener(window->xdg_surface, &xdg_surface_listener, window);
	xdg_toplevel_add_listener(window->toplevel, &xdg_toplevel_listener, window);
	string8 title = push_str_copy(window->arena, window_title);
	xdg_toplevel_set_title(window->toplevel, title.m);
	wl_surface_commit(window->surface);

	window->size = v2s32(0,0);
	window->title = title;

	window->pool_size = WAYLAND_WINDOW_BUFFER_COUNT*WAYLAND_WINDOW_MAX_BUFFER_SIZE;

	// Make random name for shared memory file
	{
		u64 time = time_us();
		$random_series series = mk_random_series(time);
		rng1s64 range = r1s64(0, 25);
		u8 buf[16];
		for (each_element(i, buf)) {
			buf[i] = 'A' + (u8)random_r1s64(&series, range);
		}
		string8 pre = S("/wl-lnx-shm-");
		string8 pst = str_array_fixed(buf);
		window->shared_memory_name = push_str_cat(window->arena, pre, pst);
	}

	window->shared_memory_file = linux_shm_alloc(window->shared_memory_name, window->pool_size, O_CREAT|O_RDWR, 0x644);
	window->shared_memory = linux_shm_view_open(window->shared_memory_file, r1s64(0, window->pool_size));
	int fd = cast(int)(window->shared_memory_file.v[0]);
	window->pool = wl_shm_create_pool(wayland_state.shm, fd, cast(int)window->pool_size);

	for (each_index(i, WAYLAND_WINDOW_BUFFER_COUNT)) {
		Wayland_Buffer *buffer = window->buffers + i;
		s64 offset = i*WAYLAND_WINDOW_MAX_BUFFER_SIZE;
		buffer->offset = offset;
		buffer->in_use = false;
		buffer->memory = cast(rawptr)(cast(u8*)window->shared_memory + offset);
	}

	scratch_end(scr);
}

proc void
window_backend_close(void) {
	// -------------------------------------------------------------------------------------------------
	// Close window shared memory
	Wayland_Window *window = &wayland_state.main_window;
	linux_shm_unlink(window->shared_memory_name);
	linux_shm_close(window->shared_memory_file);
}

proc b8
window_backend_not_closed(void) {
	Wayland_Window *window = &wayland_state.main_window;
	return !window->closed;
}

proc void
window_backend_events(void) {
		while (wl_display_prepare_read(wayland_state.display) != 0) {
			wl_display_dispatch_pending(wayland_state.display);
		}
		wl_display_flush(wayland_state.display);
		wl_display_read_events(wayland_state.display);
		wl_display_dispatch_pending(wayland_state.display);
}

proc void
window_backend_swap_buffers(Backbuffer backbuffer) {
	Wayland_Window *window = &wayland_state.main_window;

	Wayland_Buffer *current = nil;
	for (each_index(i, WAYLAND_WINDOW_BUFFER_COUNT)) {
		Wayland_Buffer *b = window->buffers + i;
		if (b->memory == backbuffer.memory) {
			current = b;
			break;
		}
	}
	if (current == nil) { return; }

	current->in_use = true;
	wl_surface_attach(window->surface, current->handle, 0, 0);
	wl_surface_damage(window->surface, 0, 0, max_s32, max_s32);
	wl_surface_commit(window->surface);
}

proc Backbuffer
window_backend_get_current_backbuffer(void) {
	Wayland_Window *window = &wayland_state.main_window;

	Wayland_Buffer *current = nil;
	for (each_index(i, WAYLAND_WINDOW_BUFFER_COUNT)) {
		Wayland_Buffer *b = window->buffers + i;
		if (!b->in_use && b->handle != nil) {
			current = b;
			break;
		}
	}

	Backbuffer backbuffer = zero_struct;
	if (current != nil) {
		backbuffer.stride = window->size.x*BYTES_PER_PIXEL;
		backbuffer.memory = current->memory;
	}
	return backbuffer;
}

proc vec2s32
window_backend_get_window_size(void) {
	Wayland_Window *window = &wayland_state.main_window;
	return window->size;
}
