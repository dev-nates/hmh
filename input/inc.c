
#include "input.c"

#if OS_LINUX
#include "linux/input_linux.c"
#else
#error "Platform not specified."
#endif
