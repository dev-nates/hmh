
#include "base/base_inc.h"
#include "os/os_inc.h"

#include "base/base_inc.c"
#include "os/os_inc.c"

void
entry_point(Cmdline cmdline) {
	u8 test;
	test = 200;

	int x = 0;
	int y = (1 << 4);
	x = x | (1 << 4);
	x = x | (1 << 8);
	x = x & (1 << 4);
	x = x | (1 << 8);
	x = x ^ y;
}
