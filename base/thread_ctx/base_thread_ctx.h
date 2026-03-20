

typedef struct Thread_Context Thread_Context;
struct Thread_Context {
	Arena *arenas[2];

	s64 thread_name_size;
	u8 thread_name[32];

	cstring filename;
	s64 line_number;
};

proc void tctx_init_and_equip(Thread_Context *tctx);
proc void tctx_release(void);
proc Thread_Context *tctx_get_equipped(void);

proc Arena* tctx_get_scratch(Arena **conflicts, s64 count);

proc void   tctx_set_name(string name);
proc string tctx_get_name(void);

proc void tctx_write_src_loc(cstring filename, s64 line_number);

proc void tctx_read_src_loc(cstring *filename, s64 *line_number);
#define tctx_write_this_src_loc() tctx_write_src_loc(__FILE__, __LINE__)

#define scratch_begin(conflicts, count) temp_begin(tctx_get_scratch((conflicts), (count)))
#define scratch_end(scratch) temp_end(scratch)
