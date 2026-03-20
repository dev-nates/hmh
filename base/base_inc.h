
#ifndef BASE_INC_H
#define BASE_INC_H

#include "core/base_ctx_crack.h"
#include "core/base_core.h"

#include "memory/base_memory.h"
#if OS_LINUX
	#include "memory/linux/base_memory_linux.h"
#else
	# error "missing memory sublayer header"
#endif
#include "arena/base_arena.h"
#include "array/base_array.h"

#include "math/base_math.h"
#include "strings/base_strings.h"
#include "log/base_log.h"
#include "thread_ctx/base_thread_ctx.h"
#include "random/base_random.h"

#include "base_entry_point.h"

#endif // BASE_INC_H
