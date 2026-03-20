
#ifndef BASE_ARENA_H
#define BASE_ARENA_H

#define ARENA_HEADER_SIZE 64

//--------------------------------------------------------------------------------
/*
	Arena's don't reference the memory in the structure. Instead arena's a treated as pointers and the pointer
	point's to a (HEADER + MEMORY_BLOCK) here the header is the arena's struct fields. This prevents copy bugs
	as there can only be one canonical set of values.
	Arena's pos includes the ARENA_HEADER_SIZE, eg if you do arena_clear(arena); it will reset it to ARENA_HEADER_SIZE
*/
typedef struct Arena Arena;
struct Arena {
	s64 res_size; // NOTE ns: Unused till large pages is implemented
	s64 cmt_size;

	s64 cmt;
	s64 res;
	s64 pos;
};
static_assert(sizeof(Arena) <= ARENA_HEADER_SIZE, arena_header_size_check);

//--------------------------------------------------------------------------------
// temp memory
typedef struct Temp Temp;
struct Temp {
	Arena *arena;
	s64 pos;
};

//--------------------------------------------------------------------------------
// arena procs

global s64 arena_default_reserve_size = megabyte(64);
global s64 arena_default_commit_size = kilobyte(64);
typedef struct Arena_Params Arena_Params;
struct Arena_Params {
	s64 reserve_size;
	s64 commit_size;
	rawptr optional_backing_buffer;
};
#define arena_alloc(...) arena_alloc_(&(Arena_Params){.reserve_size = arena_default_reserve_size, .commit_size = arena_default_commit_size, __VA_ARGS__})
proc Arena*
arena_alloc_(Arena_Params *params);

proc void
arena_release(Arena *arena);

proc rawptr
arena_push(Arena *arena, s64 size, s64 align);

proc void
arena_pop_to(Arena *arena, s64 pos);

proc void
arena_clear(Arena *arena);

proc void
arena_pop(Arena *arena, s64 size);

proc s64
arena_pos(Arena *arena);

#define push_size_no_zero_aligned(a, s, align) arena_push(a, s, align)
#define push_size_aligned(a, s, align) memory_zero(push_size_no_zero_aligned(a, s, align), (s))
#define push_size_no_zero(a, s) push_size_no_zero_aligned(a, s, 16)
#define push_size(a, s) memory_zero(push_size_no_zero(a, s), s)

#define push_struct_no_zero_aligned(a, T, align) (T*)arena_push((a), size_of(T), (align))
#define push_struct_aligned(a, T, align) (T*)memory_zero(arena_push((a), size_of(T), (align)), size_of(T))
#define push_struct_no_zero(a, T) push_struct_no_zero_aligned(a, T, max(16, align_of(T)))
#define push_struct(a, T) push_struct_aligned(a, T, max(16, align_of(T)))

#define push_array_no_zero_aligned(a, T, c, align) (T*)arena_push((a), size_of(T)*(c), (align))
#define push_array_aligned(a, T, c, align) (T*)memory_zero(push_array_no_zero_aligned(a, T, c, align), size_of(T)*(c))
#define push_array_no_zero(a, T, c) push_array_no_zero_aligned(a, T, c, max(16, align_of(T)))
#define push_array(a, T, c) push_array_aligned(a, T, c, max(16, align_of(T)))

//--------------------------------------------------------------------------------
// temporary memory procs

proc Temp
temp_begin(Arena *arena);

proc void
temp_end(Temp temp);

#endif // BASE_ARENA_H
