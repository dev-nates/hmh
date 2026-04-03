
#include "input.h"

#if OS_LINUX
#include "linux/input_linux.h"
#else
#error "Platform not specified."
#endif
