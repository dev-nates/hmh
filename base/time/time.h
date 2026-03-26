
typedef struct Timer Timer; // In Microseconds
struct Timer {
	u64 start;
	u64 duration;
};

// -------------------------------------------------------------------------------------------------
// Time

proc u64
time_us(void);

proc u64
time_ms(void);

proc void
time_sleep_us(u64 us);

proc void
time_wait_us(u64 start, u64 duration);

// -------------------------------------------------------------------------------------------------
// Timer

proc Timer
timer_make(u64 duration);

proc b8
timer_elapsed(Timer timer);

proc void
timer_begin(Timer *timer);

proc void
timer_done(Timer *timer);

// -------------------------------------------------------------------------------------------------
// Rdtsc
proc u64
rdtsc(void);


