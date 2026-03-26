
#ifndef BASE_INC_H
#define BASE_INC_H

#include "core/ctx_crack.h"

#if OS_LINUX
	#define _DEFAULT_SOURCE
#endif
#include "core/core.h"
#include "time/time.h"

#include "memory/memory.h"
#if OS_LINUX
	#include "time/linux/time_linux.h"
	#include "memory/linux/memory_linux.h"
#else
	# error "missing memory sublayer header"
#endif
#include "arena/arena.h"
#include "array/array.h"

#include "math/math.h"
#include "strings/strings.h"

#include "terminal/ansi_codes.h"
#include "log/log.h"
#include "terminal/console_logger.h"

#include "thread_ctx/thread_ctx.h"
#include "random/random.h"
#include "entry_point.h"

#endif // BASE_INC_H
