
#include "core/os_core.c"

#if OS_LINUX
# include "os/core/linux/os_core_linux.c"
#else
# error "OS core layer not implemented for this operating system."
#endif
