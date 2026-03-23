
#include "input.c"

#if OS_LINUX
	#include "linux/input_linux.c"
#else
	#error "Platform not defined for input layer."
#endif
