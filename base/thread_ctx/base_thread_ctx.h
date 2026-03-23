

#define THREAD_CONTEXT_ARENA_RESERVE megabyte(8)
typedef struct Thread_Context Thread_Context;
struct Thread_Context {
	Arena *arenas[2];
	Logger *logger;

	s64 thread_name_size;
	u8 thread_name[32];

	Source_Code_Location loc;
};
c_linkage global per_thread Thread_Context* tctx_thread_global = 0;

proc void tctx_init_and_set(Thread_Context *tctx);
proc void tctx_release(void);
proc Thread_Context *tctx_get_context(void);

proc Arena* tctx_get_scratch(Arena **conflicts, s64 count);

proc void   tctx_set_name(string name);
proc string tctx_get_name(void);

proc void tctx_write_src_loc(Source_Code_Location loc);

proc void tctx_read_src_loc(/**/Source_Code_Location *loc);
#define tctx_write_this_src_loc() tctx_write_src_loc(code_location)

#define scratch_begin(conflicts, count) temp_begin(tctx_get_scratch((conflicts), (count)))
#define scratch_end(scratch) temp_end(scratch)
