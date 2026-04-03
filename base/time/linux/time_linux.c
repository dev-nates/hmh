//--------------------------------------------------------------------------------_
// linux helpers

proc Date_Time
_date_time_from_tm(struct tm in, u32 msec)
{
	Date_Time dt = {0};
	dt.sec  = (u16)in.tm_sec;
	dt.min  = (u16)in.tm_min;
	dt.hour = (u16)in.tm_hour;
	dt.day  = (u16)(in.tm_mday-1);
	dt.mon  = (u32)in.tm_mon;
	dt.year = (u32)(in.tm_year+1900);
	dt.msec = (u16)msec;
	return dt;
}

proc struct tm
_tm_from_date_time(Date_Time dt)
{
	struct tm result = {0};
	result.tm_sec = (int)dt.sec;
	result.tm_min = (int)dt.min;
	result.tm_hour= (int)dt.hour;
	result.tm_mday= (int)dt.day+1;
	result.tm_mon = (int)dt.mon;
	result.tm_year= (int)dt.year-1900;
	return result;
}

proc struct timespec
_timespec_from_date_time(Date_Time dt)
{
	struct tm tm_val = _tm_from_date_time(dt);
	time_t seconds = timegm(&tm_val);
	struct timespec result = {0};
	result.tv_sec = seconds;
	return result;
}

proc Dense_Time
_dense_time_from_timespec(struct timespec in)
{
	Dense_Time result = 0;
	{
		struct tm tm_time = {0};
		gmtime_r(&in.tv_sec, &tm_time);
		Date_Time date_time = _date_time_from_tm(tm_time, (u32)(in.tv_nsec/millions(1)));
		result = dense_time_from_date_time(date_time);
	}
return result;
}

//--------------------------------------------------------------------------------
// time procs

proc u64
time_ns(void) {
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	u64 result = cast(u64)t.tv_sec*billions(1) + cast(u64)t.tv_nsec;
	return result;
}

proc u64
time_us(void) {
	return time_ns()/thousands(1);
}

proc u64
time_ms(void) {
	u64 result = time_us()/thousands(1);
	return result;
}

proc void
sleep_ns(u64 time) {
	usleep(cast(__useconds_t)(time/1000));
}

proc void
time_wait_ns(u64 start, u64 duration) {
	u64 now = time_ns();
	u64 target = start + duration;
	if (now > target) { return; }

	usleep(cast(__useconds_t)((target - now) / 1000));

	do {
		now = time_ns();
	} while (target > now);
}

