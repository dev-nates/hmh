

c_linkage global per_thread Thread_Context* tctx_thread_global = 0;

proc void
tctx_init_and_equip(Thread_Context *tctx)
{
	memory_zero_struct(tctx);
	for (s64 i = 0; i < array_count(tctx->arenas); i += 1) {
		tctx->arenas[i] = arena_alloc();
	}
	tctx_thread_global = tctx;
}

proc void
tctx_release(void)
{
	for (s64 i = 0; i < array_count(tctx_thread_global->arenas); i += 1) {
		arena_release(tctx_thread_global->arenas[i]);
	}
}

proc Thread_Context *
tctx_get_equipped(void)
{
	return tctx_thread_global;
}

proc Arena *
tctx_get_scratch(Arena **conflicts, s64 count) {
	Thread_Context *tctx = tctx_get_equipped();
	Arena *result = 0;
	for (s64 i = 0; i < array_count(tctx->arenas); i += 1) {
		Arena *a = tctx->arenas[i];
		result = a;
		for (s64 j = 0; j < count; j += 1) {
			Arena *b = conflicts[j];
			if (a == b) {
				result = 0;
				break;
			}
		}

		if (result != 0) {
			break;
		}
	}

	return result;
}

proc void
tctx_set_name(string name) {
	Thread_Context *tctx = tctx_get_equipped();
	s64 size = clamp_top(name.size, (s64)sizeof(tctx->thread_name));
	memory_copy(tctx->thread_name, name.m, size);
	tctx->thread_name_size = size;
}

proc string
tctx_get_name(void) {
	Thread_Context *tctx = tctx_get_equipped();
	return make_str(tctx->thread_name, tctx->thread_name_size);
}

proc void
tctx_write_src_loc(cstring filename, s64 line_number) {
	Thread_Context *tctx = tctx_get_equipped();
	tctx->filename = filename;
	tctx->line_number = line_number;
}

proc void tctx_read_src_loc(cstring *filename, s64 *line_number) {
	Thread_Context *tctx = tctx_get_equipped();
	*filename = tctx->filename;
	*line_number = tctx->line_number;
}
