
#include "base/inc.h"
#include "os/linux/inc.h"

#include "base/inc.c"
#include "os/linux/inc.c"

proc void
entry_point(Cmdline cl) {
	Linux_System_Info info = linux_system_info();
	infof("System Info:");
	infof("\tLogical Processors: %ld", info.logical_processor_count);
	infof("\tPage Size: %ld", info.page_size);
	infof("\tLarge Page Size: %ld", info.large_page_size);
	infof("\tMachine name: %.*s", svarg(info.machine_name));


}
