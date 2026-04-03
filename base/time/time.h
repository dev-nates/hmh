
// Nanosecond timer
typedef struct Timer Timer;
struct Timer {
	b64 active;
	u64 start;
	u64 end;
};

// -------------------------------------------------------------------------------------------------
// Time

proc u64
time_ns(void);

proc u64
time_us(void);

proc u64
time_ms(void);

proc void
time_sleep_ns(u64 time);

proc void
time_wait_ns(u64 start, u64 duration);

// -------------------------------------------------------------------------------------------------
// Timer

proc void
timer_begin(Timer *timer, u64 duration);

proc b8
timer_end(Timer timer);

proc void
timer_done(Timer *timer);

// -------------------------------------------------------------------------------------------------
// Rdtsc
proc u64
rdtsc(void);


