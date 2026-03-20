

#if OS_LINUX
	#include "linux/wm_linux.c"
#else
	#error "window manager not implemented for this operating system."
#endif
