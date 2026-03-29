
#ifndef BASE_INC_C
#define BASE_INC_C

#include "core/core.c"
#include "time/time.c"
#include "array/array.c"

#if OS_LINUX
	#include "time/linux/time_linux.c"
	#include "memory/linux/memory_linux.c"
	#include "sync/linux/sync_linux.c"
#else
	#error missing memory sublayer implementation
#endif

#include "arena/arena.c"
#include "math/math.c"
#include "strings/strings.c"
#include "log/log.c"
#include "terminal/console_logger.c"
#include "thread_ctx/thread_ctx.c"
#include "random/random.c"

#include "entry_point.c"

#endif // BASE_INC_C
