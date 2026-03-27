// -------------------------------------------------------------------------------------------------
// Helpers

proc IGamepad_Button_Kind
_gamepad_button_from_linux_button(Linux_Gamepad_Button_Kind kind) {
	IGamepad_Button_Kind arr[Linux_Gamepad_Button_COUNT] = {
		[Linux_Gamepad_Button_0]      = -1,
		[Linux_Gamepad_Button_1]      = -1,
		[Linux_Gamepad_Button_2]      = -1,
		[Linux_Gamepad_Button_3]      = -1,
		[Linux_Gamepad_Button_4]      = -1,
		[Linux_Gamepad_Button_5]      = -1,
		[Linux_Gamepad_Button_6]      = -1,
		[Linux_Gamepad_Button_7]      = -1,
		[Linux_Gamepad_Button_8]      = -1,
		[Linux_Gamepad_Button_9]      = -1,
		[Linux_Gamepad_Button_10]     = -1,
		[Linux_Gamepad_Button_11]     = -1,
		[Linux_Gamepad_Button_12]     = -1,
		[Linux_Gamepad_Button_13]     = -1,
		[Linux_Gamepad_Button_14]     = -1,
		[Linux_Gamepad_Button_15]     = -1,

		[Linux_Gamepad_Button_A]      = IGamepad_Button_A,
		[Linux_Gamepad_Button_B]      = IGamepad_Button_B,
		[Linux_Gamepad_Button_C]      = IGamepad_Button_C,
		[Linux_Gamepad_Button_X]      = IGamepad_Button_X,
		[Linux_Gamepad_Button_Y]      = IGamepad_Button_Y,
		[Linux_Gamepad_Button_Z]      = IGamepad_Button_Z,
		[Linux_Gamepad_Button_TL]     = IGamepad_Button_LTrigger,
		[Linux_Gamepad_Button_TR]     = IGamepad_Button_RTrigger,
		[Linux_Gamepad_Button_TL2]    = IGamepad_Button_LBumper,
		[Linux_Gamepad_Button_TR2]    = IGamepad_Button_RBumper,
		[Linux_Gamepad_Button_SELECT] = IGamepad_Button_Select,
		[Linux_Gamepad_Button_START]  = IGamepad_Button_Start,
		[Linux_Gamepad_Button_MODE]   = IGamepad_Button_Mode,
		[Linux_Gamepad_Button_THUMBL] = IGamepad_Button_LThumb,
		[Linux_Gamepad_Button_THUMBR] = IGamepad_Button_RThumb,
	};
	assert(kind < array_count(arr));
	return arr[kind];
}

proc IGamepad_Axis_Kind
_gamepad_axis_from_linux_axis(Linux_Gamepad_Axis_Kind kind) {
	IGamepad_Axis_Kind arr[Linux_Gamepad_Axis_COUNT] = {
		[Linux_Gamepad_Axis_X]          = IGamepad_Axis_X,
		[Linux_Gamepad_Axis_Y]          = IGamepad_Axis_Y,
		[Linux_Gamepad_Axis_Z]          = IGamepad_Axis_Z,
		[Linux_Gamepad_Axis_RX]         = IGamepad_Axis_RX,
		[Linux_Gamepad_Axis_RY]         = IGamepad_Axis_RY,
		[Linux_Gamepad_Axis_RZ]         = IGamepad_Axis_RZ,
		[Linux_Gamepad_Axis_THROTTLE]   = IGamepad_Axis_Throttle,
		[Linux_Gamepad_Axis_RUDDER]     = IGamepad_Axis_Rudder,
		[Linux_Gamepad_Axis_WHEEL]      = IGamepad_Axis_Wheel,
		[Linux_Gamepad_Axis_GAS]        = IGamepad_Axis_Gas,
		[Linux_Gamepad_Axis_BRAKE]      = IGamepad_Axis_Brake,

		[Linux_Gamepad_Axis_i0]         = -1,
		[Linux_Gamepad_Axis_i1]         = -1,
		[Linux_Gamepad_Axis_i2]         = -1,
		[Linux_Gamepad_Axis_i3]         = -1,
		[Linux_Gamepad_Axis_i4]         = -1,

		[Linux_Gamepad_Axis_Hat0X]      = IGamepad_Axis_Hat0x,
		[Linux_Gamepad_Axis_Hat0Y]      = IGamepad_Axis_Hat0y,
		[Linux_Gamepad_Axis_Hat1X]      = IGamepad_Axis_Hat1x,
		[Linux_Gamepad_Axis_Hat1Y]      = IGamepad_Axis_Hat1y,
		[Linux_Gamepad_Axis_Hat2X]      = IGamepad_Axis_Hat2x,
		[Linux_Gamepad_Axis_Hat2Y]      = IGamepad_Axis_Hat2y,
		[Linux_Gamepad_Axis_Hat3X]      = IGamepad_Axis_Hat3x,
		[Linux_Gamepad_Axis_Hat3Y]      = IGamepad_Axis_Hat3y,
		[Linux_Gamepad_Axis_PRESSURE]   = IGamepad_Axis_Pressure,
		[Linux_Gamepad_Axis_DISTANCE]   = IGamepad_Axis_Distance,
		[Linux_Gamepad_Axis_TILT_X]     = IGamepad_Axis_Tilt_X,
		[Linux_Gamepad_Axis_TILT_Y]     = IGamepad_Axis_Tilt_Y,
		[Linux_Gamepad_Axis_TOOL_WIDTH] = IGamepad_Axis_Tool_Width,
	};
	assert(kind < array_count(arr));
	return arr[kind];
}

// -------------------------------------------------------------------------------------------------
// Implementation

proc b8
input_open(void) {
	int flags;
	flags = 0;
	flags = IN_NONBLOCK;
	gamepad_notify = inotify_init1(flags);
	if (gamepad_notify < 0) {
		errorf("Failed to initialize inotify for /dev/input");
		return false;
	}
	u32 mask = IN_ATTRIB;
	cstring directory = "/dev/input";
	inotify_add_watch(gamepad_notify, directory, mask);
	return true;
}

proc b8
input_check_for_hotplugged_gamepads(void) {
	assert(gamepad_notify > 0);
	#define MAX_EVENTS 64
	struct inotify_event events[MAX_EVENTS];
	ssize_t bytes_read = read(gamepad_notify, events, size_of(events));
	b8 result = (bytes_read > 0);
	return result;
}

proc IGamepad_List
input_connect_gamepads(Arena *arena) {
	Temp scr = scratch_begin(&arena, 1);

	OS_File_Iter it = zero_struct;
	string path = S("/dev/input");
	OS_File_Iter_Flags flags = OS_File_Iter_Flag_Normal;
	os_file_iter_begin(path, flags, _ret_ &it);

	IGamepad_List list = zero_struct;

	// Loop through /dev/input and check for connected gamepads
	for (;;) {
		string path;
		File_Properties props;
		b8 result = os_file_iter_next(scr.arena, &it, _ret_ &path, &props);
		if (!result) { break; }

		// Only process event files
		string filename = str_skip_last_slash(path);
		string event_str = S("event");
		s64 pos = str_find_needle(0, event_str, filename, 0);
		if (pos == filename.size) { continue; }

		// Device number
		assert(event_str.size < filename.size);
		rng1s64 device_num_range = r1s64(event_str.size, filename.size);
		string device_num_str = str_substr(filename, device_num_range);
		int device_num = cast(int)s64_from_str(device_num_str, 10);

		// Open the file
		u32 mode = O_RDONLY;
		u32 flags = O_NONBLOCK;
		errno = 0;
		int file = open(path.m, O_NONBLOCK | O_RDWR);
		if (errno == EACCES) { continue; }

		IGamepad *gamepad = push_struct(arena, IGamepad);
		gamepad->connected = true;

		Linux_Gamepad_Data *data = cast(Linux_Gamepad_Data*)gamepad->opaque;
		data->device_number = device_num;
		data->file = file;

		// Gaempad name
		ioctl(file, EVIOCGNAME(size_of(gamepad->name_buffer)), gamepad->name_buffer);
		gamepad->name = str_from_cstring(gamepad->name_buffer);

		// Axis constraints
		for (s64 i = 0; i < Linux_Gamepad_Axis_COUNT; i += 1) {
			Linux_Gamepad_Axis_Kind linux_axis_kind = cast(Linux_Gamepad_Axis_Kind)i;
			IGamepad_Axis_Kind gamepad_axis_kind = _gamepad_axis_from_linux_axis(linux_axis_kind);
			if (gamepad_axis_kind < 0) { continue; }

			struct input_absinfo info = zero_struct;
			int result = ioctl(file, EVIOCGABS(cast(u32)(linux_axis_kind)), &info);
			if (result < 0) { continue; }

			IAxis *axis = gamepad->axes + gamepad_axis_kind;
			axis->value = cast(f32)(info.value);
			axis->range = r1f32(cast(f32)info.minimum, cast(f32)info.maximum);
		}

		// Rumble
		struct ff_effect effect = zero_struct;
		effect.type = FF_RUMBLE;
		effect.id = -1;
		if (ioctl(file, EVIOCSFF, &effect) >= 0) {
			gamepad->has_rumble = true;
			data->rumble_effect_id = effect.id;
		}
		
		// Push onto list
		IGamepad **ptr = check_nil(list.head, nil) ? &list.head : &list.tail->next;
		*ptr = gamepad; list.tail = gamepad;
		gamepad->next = nil;
		list.count += 1;
	}

	os_file_iter_end(&it);
	scratch_end(scr);

	return list;
}

proc void
input_disconnect_gamepads(IGamepad_List list) {
	for (each_node(gamepad, list.head, IGamepad)) {
		Linux_Gamepad_Data *data = cast(Linux_Gamepad_Data*)gamepad->opaque;
		assert(data->file >= 0);
		errno = 0;
		int result = close(data->file);
		if (result < 0 || errno != 0) {
			warningf("Failed to close gamepad device file.");
		}
	}
}


proc IGamepad_Events
input_gamepad_read_events(IGamepad *gamepad, Arena *arena) {
	if (!gamepad->connected) { return cast(IGamepad_Events)zero_struct; }

	Linux_Gamepad_Data *data = cast(Linux_Gamepad_Data*)gamepad->opaque;
	Temp scr = scratch_begin(&arena, 1);

	typedef struct IButton_Event_Chunk IButton_Event_Chunk;
	struct IButton_Event_Chunk {
		IButton_Event_Chunk *next;
		IButton_Event events[64];
		s64 count;
	};
	typedef struct IButton_Event_Chunk_List IButton_Event_Chunk_List;
	struct IButton_Event_Chunk_List {
		IButton_Event_Chunk *head, *tail;
		s64 total;
		s64 chunk_count;
	};

	typedef struct IAxis_Event_Chunk IAxis_Event_Chunk;
	struct IAxis_Event_Chunk {
		IAxis_Event_Chunk *next;
		IAxis_Event events[64];
		s64 count;
	};
	typedef struct IAxis_Event_Chunk_List IAxis_Event_Chunk_List;
	struct IAxis_Event_Chunk_List {
		IAxis_Event_Chunk *head, *tail;
		s64 total;
		s64 chunk_count;
	};


	IButton_Event_Chunk_List button_chunks = zero_struct;
	IAxis_Event_Chunk_List axis_chunks = zero_struct;
	for (;;) {
		struct input_event gamepad_event;
		errno = 0;
		ssize_t bytes_read = read(data->file, &gamepad_event, size_of(gamepad_event));
		if (bytes_read <= 0 || errno != 0) {
			break;
		}

		#define MICROSECONDS_PER_SECOND 1000000
		if (gamepad_event.type == EV_KEY && (BTN_JOYSTICK <= gamepad_event.code && gamepad_event.code <= BTN_THUMBR)) {
			Linux_Gamepad_Button_Kind linux_button_kind = gamepad_event.code - BTN_JOYSTICK;
			IGamepad_Button_Kind      gamepad_button_kind = _gamepad_button_from_linux_button(linux_button_kind);
			if (gamepad_button_kind == -1) { continue; }

			IButton_Event_Chunk_List *list = &button_chunks;
			IButton_Event_Chunk *chunk = list->tail;
			if (check_nil(chunk, nil) || chunk->count >= array_count(chunk->events)) {
				chunk = push_struct(scr.arena, IButton_Event_Chunk);

				IButton_Event_Chunk **ptr = check_nil(list->head, nil) ? &list->head : &list->tail->next;
				*ptr = chunk; list->tail = chunk;
				chunk->next = nil;
				list->chunk_count += 1;
			}
			assert(chunk->count < array_count(chunk->events));

			IButton_Event *event = chunk->events + chunk->count;
			chunk->count += 1;
			list->total += 1;

			event->state = cast(IButton_State)(gamepad_event.value);
			event->e.time = cast(u64)(gamepad_event.time.tv_sec)*MICROSECONDS_PER_SECOND + cast(u64)(gamepad_event.time.tv_usec);
		}
		if (gamepad_event.type == EV_ABS && (ABS_X <= gamepad_event.code && gamepad_event.code <= ABS_TOOL_WIDTH)) {
			Linux_Gamepad_Axis_Kind linux_axis_kind = gamepad_event.code - ABS_X;
			IGamepad_Axis_Kind gamepad_axis_kind = _gamepad_axis_from_linux_axis(linux_axis_kind);
			if (gamepad_axis_kind == -1) { continue; }

			IAxis_Event_Chunk_List *list = &axis_chunks;
			IAxis_Event_Chunk *chunk = list->tail;
			if (check_nil(chunk, nil) || chunk->count >= array_count(chunk->events)) {
				chunk = push_struct(scr.arena, IAxis_Event_Chunk);

				IAxis_Event_Chunk **ptr = check_nil(list->head, nil) ? &list->head : &list->tail->next;
				*ptr = chunk; list->tail = chunk;
				chunk->next = nil;
				list->chunk_count += 1;
			}
			assert(chunk->count < array_count(chunk->events));

			IAxis_Event *event = chunk->events + chunk->count;
			chunk->count += 1;
			list->total += 1;

			rng1f32 range = gamepad->axes[gamepad_axis_kind].range;
			f32 normalized = (cast(f32)(gamepad_event.value) - range.min) / (range.max - range.min)*2.f - 1.f;
			event->value = normalized;
			event->e.time = cast(u64)(gamepad_event.time.tv_sec)*MICROSECONDS_PER_SECOND + cast(u64)(gamepad_event.time.tv_usec);
		}
	}

	IGamepad_Events gamepad_events = zero_struct;
	gamepad_events.count = button_chunks.total + axis_chunks.total;
	gamepad_events.v = push_array(arena, IGamepad_Event, gamepad_events.count);

	s64 at = 0;
	for (each_node(chunk, button_chunks.head, IButton_Event_Chunk)) {
		assert(chunk->count > 0);
		for (s64 i = 0; i < chunk->count; i += 1) {

			assert(at < gamepad_events.count);
			IGamepad_Event *event = gamepad_events.v + at; at += 1;
			event->kind = IGamepad_Event_Button;

			IButton_Event *src = chunk->events + i;
			IButton_Event *dst = cast(IButton_Event*)(event);
			*dst = *src;
		}
	}
	for (each_node(chunk, axis_chunks.head, IAxis_Event_Chunk)) {
		assert(chunk->count > 0);
		for (s64 i = 0; i < chunk->count; i += 1) {

			assert(at < gamepad_events.count);
			IGamepad_Event *event = gamepad_events.v + at; at += 1;
			event->kind = IGamepad_Event_Axis;

			IAxis_Event *src = chunk->events + i;
			IAxis_Event *dst = cast(IAxis_Event*)(event);
			*dst = *src;
		}
	}

	scratch_end(scr);
	return gamepad_events;
}

