

proc b8
input_layer_open(void) {
	b8 result = false;
	int flags = IN_NONBLOCK;
	gamepad_notify = inotify_init1(flags);
	if (device_change_notify < 0) {
		errorf("Failed to initialize inotify for /dev/input");
		return false
	}
	inotify_add_watch(gamepad_notify)

	return result;
}

proc b8
input_gamepad_hotplugged(void) {
	u8 ignored_buf[4096];
	return true;
}
