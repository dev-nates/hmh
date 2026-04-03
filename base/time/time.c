
proc void
timer_begin(Timer *timer, u64 duration) {
	timer->active = true;
	timer->start = time_ns();
	timer->end = timer->start + duration;
}

proc b8
timer_end(Timer timer) {
	if (!timer.active) { return false; }
	b8 result = (time_ns() >= timer.end);
	return result;
}

proc void
timer_done(Timer *timer) {
	timer->active = false;
	timer->start = 0;
	timer->end = 0;
}

proc u64
rdtsc(void) {
	return __rdtsc();
}
