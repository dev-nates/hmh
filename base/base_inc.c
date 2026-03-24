
#ifndef BASE_INC_C
#define BASE_INC_C

#include "core/base_core.c"
#include "array/base_array.c"

#if OS_LINUX
	#include "memory/linux/base_memory_linux.c"
#else
	#error missing memory sublayer implementation
#endif
#include "arena/base_arena.c"

#include "math/base_math.c"
#include "strings/base_strings.c"

#include "log/base_log.c"
#include "terminal/console_logger.c"

#include "thread_ctx/base_thread_ctx.c"
#include "random/base_random.c"
#include "base_entry_point.c"

#endif // BASE_INC_C
