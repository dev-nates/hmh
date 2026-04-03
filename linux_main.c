
#include "base/inc.h"
#include "input/input.h"

#include "game/game.h"
#include "game/game.c"

#include "os/linux/inc.h"

#include "base/inc.c"
#include "os/linux/inc.c"
#include "input/input.c"

#include "sys/inotify.h"
#include "linux/input.h"

#include "platform/linux/gamepad.h"

#define NANOSECONDS_PER_FRAME 16666666
global f32 dt = ((f32)NANOSECONDS_PER_FRAME)/1000000000.f;
global Input input_state = zero_struct;

#if WINDOW_BACKEND == WAYLAND

#define LOG_WAYLAND true
#define LOG_SCANCODES false
#define WAYLAND_COLOR TC_START(249,142,79)

#define MOUSE_LEAVE_DEFAULT_POSITION v2f32(0.f, 0.f)

#include "platform/linux/wayland.h"

// -------------------------------------------------------------------------------------------------
// Gamepad

typedef s64 Device_Notify_Bits;
enum Device_Notify_Bits {
	Devices_Were_Connected = bit1,
	Devices_Were_Disconnected = bit2,
};
proc Device_Notify_Bits
check_devices(int gamepad_notify);

proc void
try_connecting_gamepads(Linux_Gamepad gamepads[MAX_GAMEPADS], s64 *gamepad_count);

proc void
try_disconnecting_gamepads(Linux_Gamepad gamepads[MAX_GAMEPADS], s64 gamepad_count);

proc void
read_gamepad_events(Linux_Gamepad *gamepad);

// -------------------------------------------------------------------------------------------------
// Wayland Backend
proc b8
wayland_open(Wayland_Window *window, string8 window_title);

proc Wayland_Buffer*
get_current_window_backbuffer(Wayland_Window *window);

proc void
swap_backbuffer(Wayland_Window *window, Wayland_Buffer *backbuffer);

proc void
read_and_dispatch_wayland_events(void);

// -------------------------------------------------------------------------------------------------
// Entry Point
proc void
entry_point(Cmdline cl) {
	Thread_Context *tctx = tctx_get_context();
	Logger_Options options = 0;
	*tctx->logger = make_console_logger(Log_Debug, options);

	int device_notify = -1;
	Linux_Gamepad gamepads[MAX_GAMEPADS] = zero_struct;
	s64 gamepad_count = 0;
#define DEVICE_CONNETION_DELAY_NSEC millions(100)
	Timer connected_devices_timer = zero_struct;
	Timer disconnected_devices_timer = zero_struct;

	// -------------------------------------------------------------------------------------------------
	// Initialize wayland state && open window
	Wayland_Window *window = &main_window;
	string8 window_title = S("Fish");
	b8 ok = wayland_open(window, window_title);
	if (!ok) { goto end; }

	// -------------------------------------------------------------------------------------------------
	// Open up a notify watch for /dev/input
	{
		int flags = IN_NONBLOCK;
		device_notify = inotify_init1(flags);
		if (device_notify < 0) {
			errorf("Failed to initialize device inotify.");
			goto end;
		}
		u32 mask = IN_CREATE | IN_DELETE | IN_MASK_CREATE;
		cstring directory = "/dev/input";
		inotify_add_watch(device_notify, directory, mask);
	}

	// -------------------------------------------------------------------------------------------------
	// Connect initial gamepads

	for each_index(i, array_count(gamepads)) {
		Linux_Gamepad *gamepad = gamepads + i;
		Input_Gamepad *g = input_state.gamepads + i;
		gamepad->ptr = g;
		gamepad->file = -1;
	}
	try_connecting_gamepads(gamepads, &gamepad_count);
	
	for (;!window->closed;) {
		// -------------------------------------------------------------------------------------------------
		// Start Frame

		Temp scr = scratch_begin(0,0);
		u64 start = time_ns();

		// -------------------------------------------------------------------------------------------------
		// Dispatch wayland events for our main window
		read_and_dispatch_wayland_events();

		// -------------------------------------------------------------------------------------------------
		// Check for hotplugged devices (gamepads)

		// @Note:
		// This code assumes that it can read event files in `/dev/input` for gamepads.
		//
		// Depending on how permissions are set up in linux, the event files might not be readable without elevated
		// user privileges; in that case there's not much we can do here besides informing the user that they
		// should somehow make the event files for the gamepad that they are using, readable.
		// I'm pretty sure that if steam is installed, steam adds rules to make these files
		// readable without elevated privileges.

		Device_Notify_Bits bits = check_devices(device_notify);
		if (bits & Devices_Were_Connected) {
			timer_begin(&connected_devices_timer, DEVICE_CONNETION_DELAY_NSEC);
		}
		if (bits & Devices_Were_Disconnected) {
			timer_begin(&disconnected_devices_timer, DEVICE_CONNETION_DELAY_NSEC);
		}

		if (timer_end(connected_devices_timer)) {
			timer_done(&connected_devices_timer);
			infof(TC(255,0,255, "CONNECTED"));
			try_connecting_gamepads(gamepads, &gamepad_count);
		}
		if (timer_end(disconnected_devices_timer)) {
			timer_done(&disconnected_devices_timer);
			infof(TC(0,0,255, "DISCONNECTED"));
			try_disconnecting_gamepads(gamepads, gamepad_count);
		}

		// -------------------------------------------------------------------------------------------------
		// Loop through and read gamepad inputs 
		for each_element(i, gamepads) {
			Linux_Gamepad *gamepad = gamepads + i;
			read_gamepad_events(gamepad);
		}

		// -------------------------------------------------------------------------------------------------
		// Apply gamepad deadzones
		for each_element(gamepad_idx, gamepads) {
			Linux_Gamepad *gamepad = gamepads + gamepad_idx;
			Input_Gamepad *g = gamepad->ptr;

			// Single dimensional axes
			Gamepad_Axis_Kind single_dimensional_axes[Gamepad_Axis_Kind_COUNT] = {
				Gamepad_Axis_Throttle,
				Gamepad_Axis_Rudder,
				Gamepad_Axis_Wheel,
				Gamepad_Axis_Gas,
				Gamepad_Axis_Brake,
				Gamepad_Axis_Hat0x,
				Gamepad_Axis_Hat0y,
				Gamepad_Axis_Hat1x,
				Gamepad_Axis_Hat1y,
				Gamepad_Axis_Hat2x,
				Gamepad_Axis_Hat2y,
				Gamepad_Axis_Hat3x,
				Gamepad_Axis_Hat3y,
				Gamepad_Axis_Pressure,
				Gamepad_Axis_Distance,
				Gamepad_Axis_Tool_Width,
			};
			for each_element(axis_kind, single_dimensional_axes) {
				Gamepad_Deadzone deadzone = _linux_gamepad_axis_deadzones[axis_kind];
				Input_Axis *axis = g->axes + axis_kind;

				rng1f32 range = r1f32(deadzone.center - deadzone.dist, deadzone.center + deadzone.dist);
				assert(range.min <= range.max);
				if (range.min <= axis->value && axis->value <= range.max) {
					axis->value = deadzone.center;
				}
			}

			typedef struct Pair Pair;
			struct Pair {
				Gamepad_Axis_Kind a;
				Gamepad_Axis_Kind b;
			};
			// Multi dimensional axes
			Pair double_dimensional_axes[Gamepad_Axis_Kind_COUNT] = {
				{Gamepad_Axis_X, Gamepad_Axis_Y},
				{Gamepad_Axis_RX, Gamepad_Axis_RY},
				{Gamepad_Axis_Tilt_X, Gamepad_Axis_Tilt_Y},
			};

			for each_element(axis_kind, double_dimensional_axes) {
				Pair pair = double_dimensional_axes[axis_kind];
				Input_Axis *a = g->axes + pair.a;
				Input_Axis *b = g->axes + pair.b;
				Gamepad_Deadzone deadzone_a = _linux_gamepad_axis_deadzones[pair.a];
				Gamepad_Deadzone deadzone_b = _linux_gamepad_axis_deadzones[pair.b];

				vec2f32 v = v2f32(a->value, b->value);
				vec2f32 center = v2f32(deadzone_a.center, deadzone_b.center);
				f32 length = length_2f32(sub_2f32(v, center));
				f32 deadzone_dist = max(deadzone_a.dist, deadzone_b.dist);
				if (length <= deadzone_dist) {
					a->value = deadzone_a.center;
					b->value = deadzone_b.center;
				}
			}

		}


		// Input_Gamepad *g = gamepads[0].ptr;
		// for each_element(i, g->axes) {
		// 	Input_Axis *axis = g->axes + i;
		// 	if (i == Gamepad_Axis_X || i == Gamepad_Axis_Y) {
		// 		debugf("Axis[%.*s]: " TC(255,255,255, "{%.02f --> %.02f}[%.02f]"), svarg(gamepad_axis_names[i]), (f64)axis->range.min, (f64)axis->range.max, (f64)axis->value);
		// 	}
		// }

		// -------------------------------------------------------------------------------------------------
		// Update and render
		vec2s32 window_size = window->size;
		Wayland_Buffer *backbuffer = get_current_window_backbuffer(window);
		Backbuffer game_backbuffer = zero_struct;
		if (backbuffer != nil) {
			game_backbuffer.memory = backbuffer->memory;
			game_backbuffer.size = window_size;
			game_backbuffer.stride = window_size.x*BYTES_PER_PIXEL;
		}
		game_update_and_render(&input_state, game_backbuffer, dt);

		// -------------------------------------------------------------------------------------------------
		// End Frame
		input_clear_keyboard(&input_state.keyboard);
		input_clear_mouse(&input_state.mouse);
		for each_element(i, gamepads) {
			Linux_Gamepad *gamepad = gamepads + i;
			input_clear_gamepad(gamepad->ptr);
		}

		if (backbuffer != nil) { swap_backbuffer(window, backbuffer); }
		time_wait_ns(start, NANOSECONDS_PER_FRAME);

		scratch_end(scr);
	}

	// -------------------------------------------------------------------------------------------------
	// Close window shared memory
	{
		linux_shm_unlink(window->shared_memory_name);
		linux_shm_close(window->shared_memory_file);
	}

	// @Note: End tag here is for cleanup code, instead of early return
end:
	return;
}

// -------------------------------------------------------------------------------------------------
// Gamepad procedures

proc Device_Notify_Bits
check_devices(int gamepad_notify) {
	Temp scr = scratch_begin(0,0);

	s64 buffer_size = kilobytes(4);
	u8 *buffer = push_size_aligned(scr.arena, buffer_size, size_of(struct inotify_event));

	ssize_t bytes_read = read(gamepad_notify, buffer, cast(size_t)buffer_size);
	assert(bytes_read < buffer_size);
	s64 count = cast(s64)bytes_read / size_of(struct inotify_event);

	Device_Notify_Bits bits = 0;
	u8 *at = buffer;
	for (;bytes_read > 0;) {
		struct inotify_event *event = cast(struct inotify_event*)(at);
		s64 event_size = size_of(struct inotify_event) + event->len;
		at += event_size;
		bytes_read -= event_size;

		string8 name = make_str(event->name, event->len);
		if (event->mask & IN_CREATE) {
			bits |= Devices_Were_Connected;
		}
		if (event->mask & IN_DELETE) {
			bits |= Devices_Were_Disconnected;
		}
	}
	scratch_end(scr);
	return bits;
}

proc void
try_connecting_gamepads(Linux_Gamepad gamepads[MAX_GAMEPADS], s64 *gamepad_count) {
	Temp scr = scratch_begin(0,0);
	if (*gamepad_count >= MAX_GAMEPADS) {
		warningf("Exceeded max gamepads [%d]", MAX_GAMEPADS);
		return;
	}

	// Loop over /dev/input and check for gamepads
	//
	// The order in which gamepads are connect must be preserved; in a scenario with two users, if both users connect
	// and then user 1 disconnects and then user 2 disconnects, if user 2 plugs back in, they should still be assign to
	// the second slot instead of taking over the first player. There probably needs to be some way to handle the fact that
	// if user 1 walked away and is done playing the game, then user 2 could take over as the main controller
	Linux_File_Iter it = zero_struct;
	Linux_File_Iter_Flags flags = Linux_File_Iter_Only_Normal_Files;
	string8 directory = S("/dev/input");
	b8 file_iter_begin = linux_file_iter_begin(directory, flags, _ret_ &it);

	for (;file_iter_begin;) {
		string8 path = str_zero();
		File_Properties props = zero_struct;
		b8 have_next_file = linux_file_iter_next(scr.arena, &it, _ret_ &path, &props);
		if (!have_next_file) { break; }

		// Only process event files
		string8 filename = str_skip_last_slash(path);
		string8 event_str = S("event");
		s64 pos = str_find_needle(0, event_str, filename, 0);
		if (pos == filename.size) { continue; }

		// Open the file (if this is readable)
		u32 mode = O_RDONLY;
		u32 flags = O_NONBLOCK;
		errno = 0;
		int file = open(path.m, O_NONBLOCK | O_RDWR);
		if (errno == EACCES) { continue; }
		// infof("File -- %.*s", svarg(filename));

		// Check to see if this is a gamepad by inspecting the available capabilities and types of keys / axes.
#define STORAGE_COUNT(cnt) (((cnt-1) / (size_of(int)*8)) + 1)
		int cap_bits[STORAGE_COUNT(EV_CNT)];
		int abs_bits[STORAGE_COUNT(ABS_CNT)];
		int btn_bits[STORAGE_COUNT(KEY_CNT)];
#undef STORAGE_COUNT
		memory_zero(cap_bits, size_of(cap_bits));
		memory_zero(btn_bits, size_of(btn_bits));
		memory_zero(abs_bits, size_of(abs_bits));
		b8 cap_bits_ok = (ioctl(file, EVIOCGBIT(EV_SYN, size_of(cap_bits)), cap_bits) >= 0);
		b8 abs_bits_ok = (ioctl(file, EVIOCGBIT(EV_ABS, size_of(abs_bits)), abs_bits) >= 0);
		b8 btn_bits_ok = (ioctl(file, EVIOCGBIT(EV_KEY, size_of(btn_bits)), btn_bits) >= 0);
		if (!cap_bits_ok || !abs_bits_ok || !btn_bits_ok) {
			close(file); continue;
		}

#define CHECK_BIT(bit, array) cast(b8)(((array)[(bit) / (size_of(int)*8)] >> ((bit) % (size_of(int)*8))) & 0x1)
		if (!CHECK_BIT(EV_KEY, cap_bits) || !CHECK_BIT(EV_ABS, cap_bits)) {
			debugf("Discarded because of capabilities");
			close(file); continue;
		}
		if (!CHECK_BIT(ABS_X, abs_bits) || !CHECK_BIT(ABS_Y, abs_bits)) {
			debugf("Discarded because of axes");
			close(file); continue;
		}
		if (!CHECK_BIT(BTN_A, btn_bits) || !CHECK_BIT(BTN_TL, btn_bits) || !CHECK_BIT(BTN_TR, btn_bits)) {
			debugf("Discarded because of buttons");
			close(file); continue;
		}
#undef CHECK_BIT

		// Get the unique identifier string for this device
		u8 buffer[LINUX_GAMEPAD_UNIQUE_MAX_SIZE];
		ioctl(file, EVIOCGUNIQ(size_of(buffer)), buffer);
		string8 unique_str = str_from_cstring(buffer);

		// Search through the array of gamepads and check to see if we already are connected
		b8 already_connected = false;
		Linux_Gamepad *gamepad = nil;
		for each_index(i, *gamepad_count) {
			Linux_Gamepad *g = gamepads + i;
			if (str_match(g->unique_str, unique_str, 0)) {
				gamepad = g;
				break;
			}
		}

		if (gamepad != nil) {
			// Reconnect gamepad
			assert(str_match(gamepad->unique_str, unique_str, 0));
			Input_Gamepad *g = gamepad->ptr;
			g->connected = true;
			gamepad->file = file;
			infof("Gamepad: " TC_START(255,255,255) "[%.*s]" TC_END TC_START(0,255,0)" reconnected"TC_END".", svarg(unique_str));
		} else {
			// Add new gamepad
			gamepad = gamepads + *gamepad_count;
			*gamepad_count += 1;
			memory_copy(gamepad->unique_str_buffer, buffer, unique_str.size);
			gamepad->unique_str = make_str(gamepad->unique_str_buffer, unique_str.size);
			gamepad->file = file;

			Input_Gamepad *g = gamepad->ptr;
			g->connected = true;

			// Initialize axis constraints
			for each_index(i, Linux_Gamepad_Axis_COUNT) {
				Linux_Gamepad_Axis_Kind kind = cast(Linux_Gamepad_Axis_Kind)(i);
				Gamepad_Axis_Kind gamepad_axis = _gamepad_axis_from_linux_axis(kind);
				if (gamepad_axis < 0) { continue; }

				struct input_absinfo info = zero_struct;
				int result = ioctl(file, EVIOCGABS(cast(u32)(kind)), &info);
				if (result < 0) { continue; }

				Input_Axis *axis = g->axes + gamepad_axis;
				axis->value = cast(f32)(info.value);
				axis->range = r1f32(cast(f32)info.minimum, cast(f32)(info.maximum + 1));
				axis->value = _default_linux_gamepad_axis_values[gamepad_axis];
			}
			infof("Gamepad: " TC_START(255,255,255) "[%.*s]" TC_END TC_START(0,255,0)" connected"TC_END".", svarg(unique_str));
		}
	}
	scratch_end(scr);
}

proc void
try_disconnecting_gamepads(Linux_Gamepad gamepads[MAX_GAMEPADS], s64 gamepad_count) {
	Temp scr = scratch_begin(0,0);

	// -------------------------------------------------------------------------------------------------
	// Loop over /dev/input and get every unique identifier for each gamepad
	typedef struct Unique_Identifier Unique_Identifier;
	struct Unique_Identifier {
		Unique_Identifier *next;
		string8 str;
	};

	Unique_Identifier *head = nil, *tail = nil;
	s64 count = 0;

	Linux_File_Iter it = zero_struct;
	Linux_File_Iter_Flags flags = Linux_File_Iter_Only_Normal_Files;
	string8 directory = S("/dev/input");
	b8 file_iter_begin = linux_file_iter_begin(directory, flags, _ret_ &it);

	for (;file_iter_begin;) {
		string8 path = str_zero();
		File_Properties props = zero_struct;
		b8 have_next_file = linux_file_iter_next(scr.arena, &it, _ret_ &path, &props);
		if (!have_next_file) { break; }

		// Only process event files
		string8 filename = str_skip_last_slash(path);
		string8 event_str = S("event");
		s64 pos = str_find_needle(0, event_str, filename, 0);
		if (pos == filename.size) { continue; }

		// Open the file (if this is readable)
		u32 mode = O_RDONLY;
		u32 flags = O_NONBLOCK;
		errno = 0;
		int file = open(path.m, O_NONBLOCK | O_RDWR);
		if (errno == EACCES) { continue; }
		// infof("File -- %.*s", svarg(filename));

		// Get the unique identifier
		u8 buffer[LINUX_GAMEPAD_UNIQUE_MAX_SIZE];
		ioctl(file, EVIOCGUNIQ(size_of(buffer)), buffer);
		string8 str = str_from_cstring(buffer);

		Unique_Identifier *unique = push_struct(scr.arena, Unique_Identifier);
		unique->str = str;
		Unique_Identifier **ptr = check_nil(head, nil) ? &head : &tail->next;
		*ptr = unique; tail = unique;
		unique->next = nil;
		count += 1;

		close(file);
	}

	// Loop over already connected gamepads
	//
	// Loop over our list of unique strs and check to see if it exists, if it doesn't, our gamepad was disconnected.
	for each_index(i, gamepad_count) {
		Linux_Gamepad *gamepad = gamepads + i;
		assert(!str_match(gamepad->unique_str, str_zero(), 0));

		b8 exists = false;
		for each_node(unique, head, Unique_Identifier) {
			if (str_match(unique->str, gamepad->unique_str, 0)) {
				exists = true;
				break;
			}
		}
		if (!exists) {
			infof("Gamepad: " TC_START(255,255,255) "[%.*s]" TC_END TC_START(255,0,0)" disconnected"TC_END".", svarg(gamepad->unique_str));
			close(gamepad->file);
			gamepad->file = -1;
			Input_Gamepad *g = gamepad->ptr;
			g->connected = false;
		}
	}

	scratch_end(scr);
}

proc void
read_gamepad_events(Linux_Gamepad *gamepad) {
	Input_Gamepad *g = gamepad->ptr;
	if (!g->connected) { return; }
	assert(gamepad->file >= 0);

	for (;;) {
		struct input_event gamepad_event;
		errno = 0;
		ssize_t bytes_read = read(gamepad->file, &gamepad_event, size_of(gamepad_event));
		if (bytes_read <= 0 || errno != 0) {
			break;
		}

		int type = gamepad_event.type;
		int code = gamepad_event.code;
		struct timeval time = gamepad_event.time;

#define NANOSECONDS_PER_SECOND billions(1)
		if (type == EV_KEY && (BTN_GAMEPAD <= code && code <= BTN_THUMBR)) {
			Linux_Gamepad_Button_Kind linux_button_kind = code - BTN_GAMEPAD;
			Gamepad_Button_Kind gamepad_button_kind = _gamepad_button_from_linux_button(linux_button_kind);
			if (gamepad_button_kind < 0) { continue; }


			Input_Button *btn = g->btns + gamepad_button_kind;
			btn->down = cast(b8)(gamepad_event.value);
			btn->htc += 1;
			btn->time = cast(u64)((time.tv_sec*NANOSECONDS_PER_SECOND) + (time.tv_usec*1000));
		}
		if (type == EV_ABS && (ABS_X <= code && code <= ABS_TOOL_WIDTH)) {
			Linux_Gamepad_Axis_Kind linux_axis = code - ABS_X;
			Gamepad_Button_Kind gamepad_axis = _gamepad_axis_from_linux_axis(linux_axis);
			if (gamepad_axis < 0) { continue; }

			Input_Axis *axis = g->axes + gamepad_axis;
			rng1f32 range = axis->range;
			axis->value = cast(f32)(gamepad_event.value);
			axis->time = cast(u64)((time.tv_sec*NANOSECONDS_PER_SECOND) + (time.tv_usec*1000));
		}
	}
#undef NANOSECONDS_PER_SECOND
}

// -------------------------------------------------------------------------------------------------
// Wayland Backend
proc b8
wayland_open(Wayland_Window *window, string8 window_title) {
	b8 result = true;
	string8 title = str_zero();
	int fd = 0;

	// -------------------------------------------------------------------------------------------------
	// Connect to display
	wayland_state.display = wl_display_connect(nil);
	if (wayland_state.display == nil) {
		errorf("Failed to connect to wayland display.");
		result = false;
		goto end;
	}
#if LOG_WAYLAND
	infof("Connected to " WAYLAND_COLOR "WAYLAND" TC_END " display.");
#endif

	// -------------------------------------------------------------------------------------------------
	// Register objects
	wayland_state.registry = wl_display_get_registry(wayland_state.display);
	wl_registry_add_listener(wayland_state.registry, &registry_listener, nil);
	wl_display_roundtrip(wayland_state.display);
	assert(wayland_state.display != nil);
	assert(wayland_state.registry != nil);
	assert(wayland_state.compositor != nil);

	// -------------------------------------------------------------------------------------------------
	// XKB Keyboard
	wayland_state.xkb_context = xkb_context_new(cast(enum xkb_context_flags)(0));

	// -------------------------------------------------------------------------------------------------
	// Relative pointer
	
	
	// -------------------------------------------------------------------------------------------------
	// Main window
	
	window->arena = arena_from_fixed(window->arena_backing_storage);

	window->surface = wl_compositor_create_surface(wayland_state.compositor);
	window->xdg_surface = xdg_wm_base_get_xdg_surface(wayland_state.wm, window->surface);
	window->toplevel = xdg_surface_get_toplevel(window->xdg_surface);

	xdg_surface_add_listener(window->xdg_surface, &xdg_surface_listener, window);
	xdg_toplevel_add_listener(window->toplevel, &xdg_toplevel_listener, window);
	title = push_str_copy(window->arena, window_title);
	xdg_toplevel_set_title(window->toplevel, title.m);
	wl_surface_commit(window->surface);

	window->size = v2s32(0,0);
	window->title = title;
	window->pool_size = WAYLAND_WINDOW_BUFFER_COUNT*WAYLAND_WINDOW_MAX_BUFFER_SIZE;

	// -------------------------------------------------------------------------------------------------
	// Random name for shared memory
	{
		u64 time = time_us();
		Random_Series series = make_random_series(time);
		rng1s64 range = r1s64(0, 25);
		u8 buf[16];
		for each_element(i, buf) {
			buf[i] = 'A' + (u8)random_r1s64(&series, range);
		}
		string8 pre = S("/wl-lnx-shm-");
		string8 pst = str_array_fixed(buf);
		window->shared_memory_name = push_str_cat(window->arena, pre, pst);
	}


	window->shared_memory_file = linux_shm_alloc(window->shared_memory_name, window->pool_size, O_CREAT|O_RDWR, 0x644);
	window->shared_memory = linux_shm_view_open(window->shared_memory_file, r1s64(0, window->pool_size));
	fd = cast(int)(window->shared_memory_file.v[0]);
	window->pool = wl_shm_create_pool(wayland_state.shm, fd, cast(int)window->pool_size);

	for each_index(i, WAYLAND_WINDOW_BUFFER_COUNT) {
		Wayland_Buffer *buffer = window->buffers + i;
		s64 offset = i*WAYLAND_WINDOW_MAX_BUFFER_SIZE;
		buffer->offset = offset;
		buffer->in_use = false;
		buffer->memory = cast(rawptr)(cast(u8*)window->shared_memory + offset);
	}

	// Round up miscellaneous startup wayland events (like for seat name && seat capabilities)
	wl_display_roundtrip(wayland_state.display);
end:;
	return result;
}

proc Wayland_Buffer*
get_current_window_backbuffer(Wayland_Window *window) {

	Wayland_Buffer *backbuffer = nil;
	for each_index(i, WAYLAND_WINDOW_BUFFER_COUNT) {
		Wayland_Buffer *b = window->buffers + i;
		if (!b->in_use && b->handle != nil) {
			backbuffer = b;
			break;
		}
	}
	return backbuffer;
}

proc void
swap_backbuffer(Wayland_Window *window, Wayland_Buffer *backbuffer) {
	backbuffer->in_use = true;
	wl_surface_attach(window->surface, backbuffer->handle, 0, 0);
	wl_surface_damage(window->surface, 0, 0, max_s32, max_s32);
	wl_surface_commit(window->surface);
}

proc void
read_and_dispatch_wayland_events(void) {
	while (wl_display_prepare_read(wayland_state.display) != 0) {
		wl_display_dispatch_pending(wayland_state.display);
	}
	wl_display_flush(wayland_state.display);
	wl_display_read_events(wayland_state.display);
	wl_display_dispatch_pending(wayland_state.display);
}

// -------------------------------------------------------------------------------------------------
// Registry

proc void
register_interface(
	rawptr data, struct wl_registry *registry,
	u32 name, const char *interface, u32 version)
{
	//--------------------------------------------------------------------------------
	//register all the interfaces that we want

	b32 registered_interface = 0;
	string8 interface_string = make_str((cstring)interface, cstring_length((cstring)interface));
	if (str_match(S("wl_compositor"), interface_string, 0)) {
		wayland_state.compositor = (struct wl_compositor*)wl_registry_bind(registry, name, &wl_compositor_interface, version);
		registered_interface = true;
	} else if (str_match(S("wl_shm"), interface_string, 0)) {
		wayland_state.shm = (struct wl_shm*)wl_registry_bind(registry, name, &wl_shm_interface, version);
		registered_interface = true;
	} else if (str_match(S("xdg_wm_base"), interface_string, 0)) {
		wayland_state.wm = cast(struct xdg_wm_base*)wl_registry_bind(registry, name, &xdg_wm_base_interface, version);
		xdg_wm_base_add_listener(wayland_state.wm, &xdg_wm_base_listener, 0);
		registered_interface = true;
	} else if (str_match(S("wl_seat"), interface_string, 0)) {
		wayland_state.seat = cast(struct wl_seat*)wl_registry_bind(registry, name, &wl_seat_interface, version);
		wl_seat_add_listener(wayland_state.seat, &seat_listener, &main_window);
		registered_interface = true;
	} else if (str_match(S("zwp_relative_pointer_manager_v1"), interface_string, 0)) {
		wayland_state.relative_pointer_manager = cast(struct zwp_relative_pointer_manager_v1*)wl_registry_bind(registry, name, &zwp_relative_pointer_manager_v1_interface, version);
		registered_interface = true;
	}

#if LOG_WAYLAND
	if (registered_interface) {
		infof(WAYLAND_COLOR "Registered interface: " TC_END "%.*s [v: %u n: %u]", svarg(interface_string), version, name);
	}
#endif
}

proc void
unregister_interface(rawptr data, struct wl_registry *registry, u32 name) {
	// left blank
}

//--------------------------------------------------------------------------------
// Buffer
proc void
buffer_reuse(rawptr data, struct wl_buffer *buffer) {
	Wayland_Buffer *buf = (Wayland_Buffer*)data;

	buf->in_use = false;
	if (buf->handle == buffer) {
		// Reuse buffer
	} else {
		wl_buffer_destroy(buffer);
	}
}

//--------------------------------------------------------------------------------
// Seat

proc void
seat_capabilities(rawptr data, struct wl_seat *wl_seat, u32 capabilities) {
	b32 has_keyboard_support = !!(capabilities & WL_SEAT_CAPABILITY_KEYBOARD);
	b32 has_pointer_support = !!(capabilities & WL_SEAT_CAPABILITY_POINTER);
	b32 has_touch_support = !!(capabilities & WL_SEAT_CAPABILITY_TOUCH);

#if LOG_WAYLAND
	infof(WAYLAND_COLOR "Seat Capabilities" TC_END " -- keyboard support: [%s] -- pointer support: [%s] -- touch support: [%s]",
		(has_keyboard_support) ? "true" : "false",
		(has_pointer_support) ? "true" : "false",
		(has_touch_support) ? "true" : "false");
#endif
	Wayland_Window *window = cast(Wayland_Window*)(data);
	if (has_keyboard_support) {
		wayland_state.keyboard = wl_seat_get_keyboard(wayland_state.seat);
		wl_keyboard_add_listener(wayland_state.keyboard, &keyboard_listener, window);
	} else if (!has_keyboard_support && wayland_state.keyboard != 0) {
		wayland_state.keyboard = 0;
	}
	if (has_pointer_support) {
		wayland_state.pointer = wl_seat_get_pointer(wayland_state.seat);
		wl_pointer_add_listener(wayland_state.pointer, &pointer_listener, window);
		if (wayland_state.relative_pointer_manager != nil) {
			wayland_state.relative_pointer = zwp_relative_pointer_manager_v1_get_relative_pointer(wayland_state.relative_pointer_manager, wayland_state.pointer);
			zwp_relative_pointer_v1_add_listener(wayland_state.relative_pointer, &relative_pointer_listener, window);
		}
	} else if (!has_pointer_support && wayland_state.pointer != 0) {
		wayland_state.pointer = 0;
	}
	if (has_touch_support) {
		// @Todo: Touch support
		// wayland_state.touch = wl_seat_get_touch(wayland_state.seat);
		// wl_touch_add_listener(wayland_state.touch, &touch_listener, window);
	} else if (!has_touch_support && wayland_state.touch != 0) {
		wayland_state.touch = 0;
	}
}

proc void
seat_name(rawptr data, struct wl_seat *wl_seat, const char *name) {
#if LOG_WAYLAND
	infof(WAYLAND_COLOR "Seat Name" TC_END ": [%s]", name);
#endif
}

// -------------------------------------------------------------------------------------------------
// Keyboard

proc string8
_utf8_from_scancode(u32 scancode, Arena *arena) {
	xkb_keycode_t xkb_keycode = cast(xkb_keycode_t)(scancode + 8);
	int size = xkb_state_key_get_utf8(wayland_state.xkb_state, xkb_keycode, nil, 0) + 1;
	if (size <= 1) { return str_zero(); };
	rawptr buffer = push_size_no_zero(arena, size);
	xkb_state_key_get_utf8(wayland_state.xkb_state, xkb_keycode, buffer, cast(size_t)size);
	if (size == 2) {
		u8 *characters = cast(u8*)(buffer);
		u8 c = characters[0];
		if (char_is_alpha_numeric_symbol(c)) {
			return make_str(buffer, size-1);
		}
		return str_zero();
	}
	return make_str(buffer, size-1);
}

proc void
keyboard_keymap(rawptr data, struct wl_keyboard *wl_keyboard, u32 format, int fd, u32 size) {
	char *mapped = cast(char*)mmap(0, size, PROT_READ, MAP_SHARED, fd, 0);
	if (mapped == MAP_FAILED) {
		errorf("Failed to map shared memory for xkb keymap.");
		return;
	}

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
keyboard_enter(
	rawptr data, struct wl_keyboard *wl_keyboard,
	u32 serial, struct wl_surface *surface,
	struct wl_array *keys)
{
	u32 *key;
	wl_array_for_each(key, keys) {
		u32 scancode = *key;
		Input_Button *btn = input_state.keyboard.btns + scancode;
		btn->htc = 1;
		btn->down = true;
	}
}

proc void
keyboard_leave(rawptr data, struct wl_keyboard *wl_keyboard,
	u32 serial, struct wl_surface *surface)
{
	// Do nothing
}

proc void
keyboard_key(rawptr data, struct wl_keyboard *wl_keyboard,
	    u32 serial, u32 time, u32 scancode, u32 state)
{
	Input_Button *btn = input_state.keyboard.btns + scancode;
	btn->htc += 1;
	btn->down = cast(b8)(state == WL_KEYBOARD_KEY_STATE_PRESSED);

#if LOG_SCANCODES
	infof("Scancode: %u", scancode);
#endif
}

proc void
keyboard_modifiers(
	rawptr data, struct wl_keyboard *wl_keyboard,
	u32 serial, u32 mods_depressed,
	u32 mods_latched, u32 mods_locked,
	u32 group)
{
	xkb_state_update_mask(wayland_state.xkb_state, mods_depressed, mods_latched, mods_locked, 0, 0, group);
}

proc void
keyboard_repeat_info(
	rawptr data, struct wl_keyboard *wl_keyboard,
	s32 rate, s32 delay)
{
	// TODO
}

//--------------------------------------------------------------------------------
// Pointer

proc void
pointer_enter(
	rawptr data, struct wl_pointer *wl_pointer,
	u32 serial, struct wl_surface *surface,
	wl_fixed_t surface_x, wl_fixed_t surface_y
) {
	Wayland_Window *window = cast(Wayland_Window*)(data);
	window->latest_pointer_enter_serial = serial;

	double x = wl_fixed_to_double(surface_x);
	double y = wl_fixed_to_double(surface_y);
	vec2f32 pos = v2f32(cast(f32)(x), cast(f32)(y));
	Input_Mouse *mouse = &input_state.mouse;
	mouse->abs_pos = pos;
}

proc void
pointer_leave(
	rawptr data, struct wl_pointer *wl_pointer,
	u32  serial, struct wl_surface *surface
) {
	Input_Mouse *mouse = &input_state.mouse;
	mouse->abs_pos = MOUSE_LEAVE_DEFAULT_POSITION;
}

proc void
pointer_motion(
	rawptr data, struct wl_pointer *wl_pointer, u32 time,
	wl_fixed_t surface_x, wl_fixed_t surface_y
) {
	double x = wl_fixed_to_double(surface_x);
	double y = wl_fixed_to_double(surface_y);
	vec2f32 pos = v2f32(cast(f32)(x), cast(f32)(y));
	Input_Mouse *mouse = &input_state.mouse;
	mouse->abs_pos_event_time = cast(u64)(time*millions(1));
	mouse->abs_pos = pos;
}

proc void
pointer_button(
	rawptr data, struct wl_pointer *wl_pointer,
	u32 serial, u32 time, u32 button, u32 state
) {
	Mouse_Button_Kind mouse_btn_from_linux_btn[16];
	for each_index(i, array_count(mouse_btn_from_linux_btn)) {
		mouse_btn_from_linux_btn[i] = -1;
	}

	mouse_btn_from_linux_btn[BTN_LEFT-BTN_MOUSE] = Mouse_Button_Left;
	mouse_btn_from_linux_btn[BTN_RIGHT-BTN_MOUSE] = Mouse_Button_Right;
	mouse_btn_from_linux_btn[BTN_MIDDLE-BTN_MOUSE] = Mouse_Button_Middle;
	mouse_btn_from_linux_btn[BTN_FORWARD-BTN_MOUSE] = Mouse_Button_Forward;
	mouse_btn_from_linux_btn[BTN_BACK-BTN_MOUSE] = Mouse_Button_Back;

	s64 index = button - BTN_MOUSE;
	assert(0 <= index && index <= array_count(mouse_btn_from_linux_btn));
	Mouse_Button_Kind kind = mouse_btn_from_linux_btn[index];
	if (kind < 0) { return; }

	Input_Button *btn = &input_state.mouse.btns[kind];
	btn->htc += 1;
	btn->down = cast(b8)(state == WL_KEYBOARD_KEY_STATE_PRESSED);
	btn->time = cast(u64)(time*millions(1));
}

proc void
pointer_axis_source(rawptr data, struct wl_pointer *wl_pointer, u32 axis_source) {
	Wayland_Window *window = cast(Wayland_Window*)(data);
	window->wheel.has_source = true;
	window->wheel.source = cast(enum wl_pointer_axis_source)axis_source;
}

proc void
pointer_axis(
	rawptr data, struct wl_pointer *wl_pointer,
	u32 time, u32 axis, wl_fixed_t value
) {
	Wayland_Window *window = cast(Wayland_Window*)(data);
	window->wheel.time = cast(u64)(time*millions(1));
}

proc void
pointer_axis_discrete(rawptr data, struct wl_pointer *wl_pointer, u32 axis, s32 discrete) {
	Wayland_Window *window = cast(Wayland_Window*)(data);
	window->wheel.value = discrete;
}

proc void
pointer_axis_value120(rawptr data, struct wl_pointer *wl_pointer, u32 axis, s32 value120) {
	Wayland_Window *window = cast(Wayland_Window*)(data);
	window->wheel.value120 = value120;
}

proc void
pointer_frame(rawptr data, struct wl_pointer *wl_pointer) {
	Wayland_Window *window = cast(Wayland_Window*)(data);

#define w window->wheel
	if (w.has_source && w.source == WL_POINTER_AXIS_SOURCE_WHEEL) {
		Input_Mouse *mouse = &input_state.mouse;
		if (w.value120 != 0) {
			mouse->wheel_event_time = w.time;
			mouse->wheel = cast(f32)(w.value120) / 120.f;
		} else if (w.value != 0) {
			mouse->wheel_event_time = w.time;
			mouse->wheel = cast(f32)(w.value);
		}
	}
#undef w
}

proc void
pointer_axis_stop(rawptr data, struct wl_pointer *wl_pointer, u32 time, u32 axis) {
	// Empty
}

proc void
pointer_axis_relative_direction(rawptr data, struct wl_pointer *wl_pointer, u32 axis, u32 direction) {
	// Empty
}

// -------------------------------------------------------------------------------------------------
// Relative pointer

proc void
relative_pointer_motion(
	rawptr data,
	struct zwp_relative_pointer_v1 *relative_pointer,
	u32 utime_hi, u32 utime_lo,
	wl_fixed_t dx, wl_fixed_t dy,
	wl_fixed_t dx_unaccel, wl_fixed_t dy_unaccel
) {
	Input_Mouse *mouse = &input_state.mouse;
	u64 time = ((cast(u64)(utime_hi)) << 32) | cast(u64)(utime_lo);
	mouse->delta = v2f32(cast(f32)(wl_fixed_to_double(dx_unaccel)), cast(f32)(wl_fixed_to_double(dy_unaccel)));
	mouse->delta_event_time = time;
}

// -------------------------------------------------------------------------------------------------
// XDG WM Base

proc void
xdg_wm_base_ping(rawptr data, struct xdg_wm_base *wm, u32 serial) {
	xdg_wm_base_pong(wm, serial);
}

//--------------------------------------------------------------------------------
// XDG Surface
proc void
xdg_surface_configure(rawptr data, struct xdg_surface *surface, u32 serial) {
	xdg_surface_ack_configure(surface, serial);
	Wayland_Window *window = cast(Wayland_Window*)(data);
	wl_surface_commit(window->surface);
}

// -------------------------------------------------------------------------------------------------
// XDG Toplevel

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
	for each_index(i, WAYLAND_WINDOW_BUFFER_COUNT) {
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
		wl_buffer_add_listener(buffer->handle, &buffer_listener, buffer);
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
#else
#error "Window backend not specified"
#endif




