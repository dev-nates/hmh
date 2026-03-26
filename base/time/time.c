
proc Timer
timer_make(u64 duration) {
	Timer timer = {
		.start = max_u64,
		.duration = duration,
	};
	return timer;
}

proc void
timer_begin(Timer *timer) {
	timer->start = time_us();
}

proc b8
timer_elapsed(Timer timer) {
	if (timer.start == max_u64) { return false; }

	u64 now = time_us();
	u64 target = timer.start + timer.duration;
	b8 result = (now >= target);
	return result;
}

proc void
timer_done(Timer *timer) {
	timer->start = max_u64;
}

proc u64
rdtsc(void) {
	return __rdtsc();
}
