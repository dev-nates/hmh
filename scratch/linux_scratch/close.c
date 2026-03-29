

#include "base/inc.h"
#include "os/linux/inc.h"

#include "base/inc.c"
#include "os/linux/inc.c"

proc void
entry_point(Cmdline cl) {
	Arena *arena = arena_alloc();

	string8 cwd = linux_current_path(arena);

	string8 name = S("/shared-memory-for-testing");
	Linux_Shared_Memory memory = linux_shm_alloc(name, megabytes(1), O_RDWR, 0644);
	typedef struct Foo Foo;
	struct Foo {
		b8 exit;
	};

	Foo *foo = cast(Foo*)linux_shm_view_open(memory, r1s64(0, size_of(Foo)));
	foo->exit = true;

	linux_shm_view_close(memory, cast(rawptr)(foo), r1s64(0, size_of(Foo)));
	linux_shm_close(memory);
}
