// -------------------------------------------------------------------------------------------------
// Helpers

proc Gamepad_Button_Kind
_gamepad_button_from_linux_button(Linux_Gamepad_Button_Kind kind) {
	Gamepad_Button_Kind arr[Linux_Gamepad_Button_COUNT] = {
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

		[Linux_Gamepad_Button_A]      = Gamepad_Button_A,
		[Linux_Gamepad_Button_B]      = Gamepad_Button_B,
		[Linux_Gamepad_Button_C]      = Gamepad_Button_C,
		[Linux_Gamepad_Button_X]      = Gamepad_Button_X,
		[Linux_Gamepad_Button_Y]      = Gamepad_Button_Y,
		[Linux_Gamepad_Button_Z]      = Gamepad_Button_Z,
		[Linux_Gamepad_Button_TL]     = Gamepad_Button_LTrigger,
		[Linux_Gamepad_Button_TR]     = Gamepad_Button_RTrigger,
		[Linux_Gamepad_Button_TL2]    = Gamepad_Button_LBumper,
		[Linux_Gamepad_Button_TR2]    = Gamepad_Button_RBumper,
		[Linux_Gamepad_Button_SELECT] = Gamepad_Button_Select,
		[Linux_Gamepad_Button_START]  = Gamepad_Button_Start,
		[Linux_Gamepad_Button_MODE]   = Gamepad_Button_Mode,
		[Linux_Gamepad_Button_THUMBL] = Gamepad_Button_LThumb,
		[Linux_Gamepad_Button_THUMBR] = Gamepad_Button_RThumb,
	};
	assert(kind < array_count(arr));
	return arr[kind];
}

proc Gamepad_Axis_Kind
_gamepad_axis_from_linux_axis(Linux_Gamepad_Axis_Kind kind) {
	Gamepad_Axis_Kind arr[Linux_Gamepad_Axis_COUNT] = {
		[Linux_Gamepad_Axis_X]          = Gamepad_Axis_X,
		[Linux_Gamepad_Axis_Y]          = Gamepad_Axis_Y,
		[Linux_Gamepad_Axis_Z]          = Gamepad_Axis_Z,
		[Linux_Gamepad_Axis_RX]         = Gamepad_Axis_RX,
		[Linux_Gamepad_Axis_RY]         = Gamepad_Axis_RY,
		[Linux_Gamepad_Axis_RZ]         = Gamepad_Axis_RZ,
		[Linux_Gamepad_Axis_THROTTLE]   = Gamepad_Axis_Throttle,
		[Linux_Gamepad_Axis_RUDDER]     = Gamepad_Axis_Rudder,
		[Linux_Gamepad_Axis_WHEEL]      = Gamepad_Axis_Wheel,
		[Linux_Gamepad_Axis_GAS]        = Gamepad_Axis_Gas,
		[Linux_Gamepad_Axis_BRAKE]      = Gamepad_Axis_Brake,

		[Linux_Gamepad_Axis_i0]         = -1,
		[Linux_Gamepad_Axis_i1]         = -1,
		[Linux_Gamepad_Axis_i2]         = -1,
		[Linux_Gamepad_Axis_i3]         = -1,
		[Linux_Gamepad_Axis_i4]         = -1,

		[Linux_Gamepad_Axis_Hat0X]      = Gamepad_Axis_Hat0x,
		[Linux_Gamepad_Axis_Hat0Y]      = Gamepad_Axis_Hat0y,
		[Linux_Gamepad_Axis_Hat1X]      = Gamepad_Axis_Hat1x,
		[Linux_Gamepad_Axis_Hat1Y]      = Gamepad_Axis_Hat1y,
		[Linux_Gamepad_Axis_Hat2X]      = Gamepad_Axis_Hat2x,
		[Linux_Gamepad_Axis_Hat2Y]      = Gamepad_Axis_Hat2y,
		[Linux_Gamepad_Axis_Hat3X]      = Gamepad_Axis_Hat3x,
		[Linux_Gamepad_Axis_Hat3Y]      = Gamepad_Axis_Hat3y,
		[Linux_Gamepad_Axis_PRESSURE]   = Gamepad_Axis_Pressure,
		[Linux_Gamepad_Axis_DISTANCE]   = Gamepad_Axis_Distance,
		[Linux_Gamepad_Axis_TILT_X]     = Gamepad_Axis_Tilt_X,
		[Linux_Gamepad_Axis_TILT_Y]     = Gamepad_Axis_Tilt_Y,
		[Linux_Gamepad_Axis_TOOL_WIDTH] = Gamepad_Axis_Tool_Width,
	};
	assert(kind < array_count(arr));
	return arr[kind];
}

// -------------------------------------------------------------------------------------------------
// Implementation

proc b8
input_open(void) {
	int flags = IN_NONBLOCK;
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
#undef MAX_EVENTS
	ssize_t bytes_read = read(gamepad_notify, events, size_of(events));
	b8 result = (bytes_read > 0);
	return result;
}

proc Gamepad_List
input_connect_gamepads(Arena *arena) {
	Temp scr = scratch_begin(&arena, 1);

	Linux_File_Iter it = zero_struct;
	Linux_File_Iter_Flags flags = Linux_File_Iter_Only_Normal_Files;
	b8 file_iter_begin = linux_file_iter_begin(S("/dev/input"), flags, _ret_ &it);

	Gamepad_List list = zero_struct;
	if (!file_iter_begin) { return list; }


	// Loop through /dev/input and check for connected gamepads
	for (;;) {
		string8 path;
		File_Properties props;
		b8 result = linux_file_iter_next(scr.arena, &it, _ret_ &path, &props);
		if (!result) { break; }

		// Only process event files
		string8 filename = str_skip_last_slash(path);
		string8 event_str = S("event");
		s64 pos = str_find_needle(0, event_str, filename, 0);
		if (pos == filename.size) { continue; }

		// Device number
		assert(event_str.size < filename.size);
		rng1s64 device_num_range = r1s64(event_str.size, filename.size);
		string8 device_num_str = str_substr(filename, device_num_range);
		int device_num = cast(int)s64_from_str(device_num_str, 10);

		// Open the file
		u32 mode = O_RDONLY;
		u32 flags = O_NONBLOCK;
		errno = 0;
		int file = open(path.m, O_NONBLOCK | O_RDWR);
		if (errno == EACCES) { continue; }

		Gamepad *gamepad = push_struct(arena, Gamepad);
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
			Gamepad_Axis_Kind gamepad_axis_kind = _gamepad_axis_from_linux_axis(linux_axis_kind);
			if (gamepad_axis_kind < 0) { continue; }

			struct input_absinfo info = zero_struct;
			int result = ioctl(file, EVIOCGABS(cast(u32)(linux_axis_kind)), &info);
			if (result < 0) { continue; }

			Input_Axis *axis = gamepad->axes + gamepad_axis_kind;
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
		Gamepad **ptr = check_nil(list.head, nil) ? &list.head : &list.tail->next;
		*ptr = gamepad; list.tail = gamepad;
		gamepad->next = nil;
		list.count += 1;
	}

	linux_file_iter_end(&it);
	scratch_end(scr);

	return list;
}

proc void
input_disconnect_gamepads(Gamepad_List list) {
	for (each_node(gamepad, list.head, Gamepad)) {
		Linux_Gamepad_Data *data = cast(Linux_Gamepad_Data*)gamepad->opaque;
		assert(data->file >= 0);
		errno = 0;
		int result = close(data->file);
		if (result < 0 || errno != 0) {
			warningf("Failed to close gamepad device file.");
		}
	}
}


proc Gamepad_Events
input_gamepad_read_events(Gamepad *gamepad, Arena *arena) {
	if (!gamepad->connected) { return cast(Gamepad_Events)zero_struct; }

	Linux_Gamepad_Data *data = cast(Linux_Gamepad_Data*)gamepad->opaque;
	Temp scr = scratch_begin(&arena, 1);

	typedef struct Input_Button_Event_Chunk Input_Button_Event_Chunk;
	struct Input_Button_Event_Chunk {
		Input_Button_Event_Chunk *next;
		Input_Button_Event events[64];
		s64 count;
	};
	typedef struct Input_Button_Event_Chunk_List Input_Button_Event_Chunk_List;
	struct Input_Button_Event_Chunk_List {
		Input_Button_Event_Chunk *head, *tail;
		s64 total;
		s64 chunk_count;
	};

	typedef struct Input_Axis_Event_Chunk Input_Axis_Event_Chunk;
	struct Input_Axis_Event_Chunk {
		Input_Axis_Event_Chunk *next;
		Input_Axis_Event events[64];
		s64 count;
	};
	typedef struct Input_Axis_Event_Chunk_List Input_Axis_Event_Chunk_List;
	struct Input_Axis_Event_Chunk_List {
		Input_Axis_Event_Chunk *head, *tail;
		s64 total;
		s64 chunk_count;
	};


	Input_Button_Event_Chunk_List button_chunks = zero_struct;
	Input_Axis_Event_Chunk_List axis_chunks = zero_struct;
	for (;;) {
		struct input_event gamepad_event;
		errno = 0;
		ssize_t bytes_read = read(data->file, &gamepad_event, size_of(gamepad_event));
		if (bytes_read <= 0 || errno != 0) {
			break;
		}

		#define NANOSECONDS_PER_SECOND 1000000000
		if (gamepad_event.type == EV_KEY && (BTN_GAMMEPAD <= gamepad_event.code && gamepad_event.code <= BTN_THUMBR)) {
			Linux_Gamepad_Button_Kind linux_button_kind = gamepad_event.code - BTN_JOYSTICK;
			Gamepad_Button_Kind      gamepad_button_kind = _gamepad_button_from_linux_button(linux_button_kind);
			if (gamepad_button_kind == -1) { continue; }

			Input_Button_Event_Chunk_List *list = &button_chunks;
			Input_Button_Event_Chunk *chunk = list->tail;
			if (check_nil(chunk, nil) || chunk->count >= array_count(chunk->events)) {
				chunk = push_struct(scr.arena, Input_Button_Event_Chunk);

				Input_Button_Event_Chunk **ptr = check_nil(list->head, nil) ? &list->head : &list->tail->next;
				*ptr = chunk; list->tail = chunk;
				chunk->next = nil;
				list->chunk_count += 1;
			}
			assert(chunk->count < array_count(chunk->events));

			Input_Button_Event *event = chunk->events + chunk->count;
			chunk->count += 1;
			list->total += 1;

			event->state = cast(Input_Button_State)(gamepad_event.value);
			event->e.time = cast(u64)(gamepad_event.time.tv_sec)*NANOSECONDS_PER_SECOND + cast(u64)(gamepad_event.time.tv_usec*1000);
		}
		if (gamepad_event.type == EV_ABS && (ABS_X <= gamepad_event.code && gamepad_event.code <= ABS_TOOL_WIDTH)) {
			Linux_Gamepad_Axis_Kind linux_axis_kind = gamepad_event.code - ABS_X;
			Gamepad_Axis_Kind gamepad_axis_kind = _gamepad_axis_from_linux_axis(linux_axis_kind);
			if (gamepad_axis_kind == -1) { continue; }

			Input_Axis_Event_Chunk_List *list = &axis_chunks;
			Input_Axis_Event_Chunk *chunk = list->tail;
			if (check_nil(chunk, nil) || chunk->count >= array_count(chunk->events)) {
				chunk = push_struct(scr.arena, Input_Axis_Event_Chunk);

				Input_Axis_Event_Chunk **ptr = check_nil(list->head, nil) ? &list->head : &list->tail->next;
				*ptr = chunk; list->tail = chunk;
				chunk->next = nil;
				list->chunk_count += 1;
			}
			assert(chunk->count < array_count(chunk->events));

			Input_Axis_Event *event = chunk->events + chunk->count;
			chunk->count += 1;
			list->total += 1;

			rng1f32 range = gamepad->axes[gamepad_axis_kind].range;
			f32 normalized = (cast(f32)(gamepad_event.value) - range.min) / (range.max - range.min)*2.f - 1.f;
			event->value = normalized;
			event->e.time = cast(u64)(gamepad_event.time.tv_sec)*NANOSECONDS_PER_SECOND + cast(u64)(gamepad_event.time.tv_usec*1000);
		}
		#undef NANOSECONDS_PER_SECOND
	}

	Gamepad_Events gamepad_events = zero_struct;
	gamepad_events.count = button_chunks.total + axis_chunks.total;
	gamepad_events.v = push_array(arena, Gamepad_Event, gamepad_events.count);

	s64 at = 0;
	for (each_node(chunk, button_chunks.head, Input_Button_Event_Chunk)) {
		assert(chunk->count > 0);
		for (s64 i = 0; i < chunk->count; i += 1) {

			assert(at < gamepad_events.count);
			Gamepad_Event *event = gamepad_events.v + at; at += 1;
			event->kind = Gamepad_Event_Button;

			Input_Button_Event *src = chunk->events + i;
			Input_Button_Event *dst = cast(Input_Button_Event*)(event);
			*dst = *src;
		}
	}
	for (each_node(chunk, axis_chunks.head, Input_Axis_Event_Chunk)) {
		assert(chunk->count > 0);
		for (s64 i = 0; i < chunk->count; i += 1) {
			assert(at < gamepad_events.count);
			Gamepad_Event *event = gamepad_events.v + at; at += 1;
			event->kind = Gamepad_Event_Axis;

			Input_Axis_Event *src = chunk->events + i;
			Input_Axis_Event *dst = cast(Input_Axis_Event*)(event);
			*dst = *src;
		}
	}

	scratch_end(scr);
	return gamepad_events;
}

