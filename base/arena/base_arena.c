
//--------------------------------------------------------------------------------
// arena procs

proc Arena *
arena_alloc_(Arena_Params *params)
{
	assert(params->reserve_size >= 0 && params->commit_size >= 0);
	// align to page boundary
	s64 reserve_size = align_pow2(params->reserve_size, memory_page_size());
	s64 commit_size = align_pow2(params->commit_size, memory_page_size());

	rawptr base = params->optional_backing_buffer;
	if (base == 0) {
		base = memory_reserve(reserve_size, 0);
		memory_commit(base, commit_size);
	}

	Arena *arena = (Arena*)base;
	arena->cmt_size = commit_size;
	arena->res_size = reserve_size;
	arena->cmt =      commit_size;
	arena->res =      reserve_size;
	arena->pos =      ARENA_HEADER_SIZE;
	asan_poison(base, (u64)commit_size);
	asan_cure(base, ARENA_HEADER_SIZE);
	return arena;
}

proc void
arena_release(Arena *arena)
{
	rawptr base = (rawptr)arena;
	memory_release(base, arena->res_size);
}

proc rawptr
arena_push(Arena *arena, s64 size, s64 align) {
	assert(arena->pos >= ARENA_HEADER_SIZE && size >= 0 && is_pow2(align));
	assert(arena->cmt <= arena->res);
	u8 *base = (u8 *)arena;

	s64 pos_aligned = align_pow2(arena->pos, align);
	s64 new_pos = pos_aligned + size;

	if (new_pos > arena->cmt) {
		assert(arena->cmt_size > 0);
		s64 cmt_pos = new_pos + arena->cmt_size-1;
		cmt_pos -= cmt_pos % arena->cmt_size;
		cmt_pos = clamp_top(cmt_pos, arena->res_size);
		assert(arena->cmt%arena->cmt_size == 0);
		s64 cmt_size = cmt_pos - arena->cmt;
		rawptr commit_base = (rawptr)(base + arena->cmt);
		memory_commit(commit_base, cmt_size);
		arena->cmt = cmt_pos;
	}

	rawptr result = 0;
	if (arena->cmt >= new_pos) {
		result = (rawptr)(base + pos_aligned);
		asan_cure(result, (u64)size);
		arena->pos = new_pos;
	}
	assert(result != 0);
	return result;
}

proc void
arena_pop_to(Arena *arena, s64 pos)
{
	pos = clamp_bot(ARENA_HEADER_SIZE, pos);
	u8 *base = (u8 *)arena;
	assert(pos <= arena->pos);
	asan_poison(base + pos, (u64)(arena->pos - pos));
	arena->pos = pos;
}

proc void
arena_clear(Arena *arena)
{
	arena_pop_to(arena, 0);
}

proc void
arena_pop(Arena *arena, s64 size)
{
	size = clamp_bot(0, size);
	assert(arena->pos >= size); // ns: idk if this should be allowed?
	s64 new_pos = arena->pos - size;
	arena_pop_to(arena, new_pos);
}

proc s64
arena_pos(Arena *arena) {
	return arena->pos;
}

//--------------------------------------------------------------------------------
// temporary memory procs

proc Temp temp_begin(Arena *arena)
{
	Temp result = {arena, arena->pos};
	return result;
}
proc void temp_end(Temp temp)
{
	arena_pop_to(temp.arena, temp.pos);
}
