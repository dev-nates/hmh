
#ifndef OS_INC_H
#define OS_INC_H

#include "core/os_core.h"
#include "core/os_console_logger.h"

#if OS_LINUX
	#include "core/linux/os_core_linux.h"
#else
	#error "OS core layer not implemented for this operating system"
#endif

#endif // OS_INC_H
