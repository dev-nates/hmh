
#include "base/base_inc.h"
#include "os/os_inc.h"

#include "base/base_inc.c"
#include "os/os_inc.c"

proc void
entry_point(Cmdline cmdline) {
	Source_Code_Location loc = code_location;
	printf("File: %s -- Procedure: %s -- Line: %ld\n", loc.filename, loc.procedure, loc.line);

	Thread_Context *ctx = tctx_get_context();
	Logger *logger = ctx->logger;

	errorf("This is an error: %d", 10);

	int bh = 5;
}
