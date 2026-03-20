#ifndef WM_INC_H
#define WM_INC_H

#include "wm.h"

#if OS_LINUX
	#include "linux/wm_linux.h"
#else
	#error "window manager not implemented for this operating system."
#endif

#endif // WM_INC_H
