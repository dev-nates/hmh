

proc b8
input_layer_open(void) {
	b8 result = false;
	int flags = IN_NONBLOCK;
	device_change_notify = inotify_init1(flags);
	if (device_change_notify < 0) {
	}
	return result;
}




proc b8
input_gamepad_hotplugged(void) {
	u8 ignored_buf[4096];
	return true;
}
