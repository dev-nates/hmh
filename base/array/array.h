
//--------------------------------------------------------------------------------
// array8

typedef struct array8 array8;
struct array8 {
	u8 *m;
	s64 size;
};

typedef struct array_node array_node;
struct array_node {
	array_node *next;
	array8 arr;
};

typedef struct array_list array_list;
struct array_list {
	array_node *head, *tail;
	s64 count;
	s64 total_size;
};

typedef struct meta_array meta_array;
struct meta_array {
	array8 *v;
	s64 count;
};

//--------------------------------------------------------------------------------
// array 16

typedef struct array16 array16;
struct array16 {
	u16 *m;
	s64 size;
};

//--------------------------------------------------------------------------------
// array 32

typedef struct array32 array32;
struct array32 {
	u32 *m;
	s64 size;
};

//--------------------------------------------------------------------------------
// array 64

typedef struct array64 array64;
struct array64 {
	u64 *m;
	s64 size;
};

#define array_from_struct(s) make_array(s, sizeof(*s))
#define array_from_count(a, cnt) make_array(a, sizeof(*a)*(cnt))
#define array_from_fixed(a) make_array((u8*)a, sizeof(a))
proc array8 make_array(u8 *m, s64 size);

proc void array_list_push_node(array_list *list, array_node *node);
proc void array_list_push(Arena *arena, array_list *list, array8 arr);

proc meta_array reserve_meta_array(Arena *arena, s64 count);
proc meta_array meta_array_from_list(Arena *arena, array_list list);

#define array16_from_struct(s) make_array16(s, size_of(*s))
#define array16_from_count(a, cnt) make_array16(a, size_of(*a)*(cnt))
#define array16_from_fixed(a) make_array16((u8*)a, size_of(a))
proc array16 make_array16(u16 *m, s64 size);

#define array32_from_struct(s) make_array32(s, size_of(*s))
#define array32_from_count(a, cnt) make_array32(a, size_of(*a)*(cnt))
#define array32_from_fixed(a) make_array32((u8*)a, size_of(a))
proc array32 make_array32(u32 *m, s64 size);

#define array64_from_struct(s) make_array64(s, size_of(*s))
#define array64_from_count(a, cnt) make_array64(a, size_of(*a)*(cnt))
#define array64_from_fixed(a) make_array64((u8*)a, size_of(a))
proc array64 make_array64(u64 *m, s64 size);
