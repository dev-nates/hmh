
//--------------------------------------------------------------------------------
// third party
disable_warnings_begin;
#define STB_SPRINTF_DECORATE(name) base_string_##name
#include <third_party/stb/stb_sprintf.h>
disable_warnings_end;

//--------------------------------------------------------------------------------
// str types

typedef array string;
typedef array16 string16;
typedef array32 string32;

//--------------------------------------------------------------------------------
// str list && array types

typedef struct string_node string_node;
struct string_node {
	string_node *next;
	string str;
};

typedef struct string_meta_node string_meta_node;
struct string_meta_node
{
  string_meta_node *next;
  string_node *node;
};

typedef struct string_list string_list;
struct string_list {
	string_node *head;
	string_node *tail;
	s64 node_count;
	s64 total_size;
};

typedef struct string_array string_array;
struct string_array {
	string *v;
	s64 count;
};

//--------------------------------------------------------------------------------
// match flags

typedef u32 String_Match_Flags;
enum {
	String_Match_Flag_Case_Insensitive = bit1,
	String_Match_Flag_Right_Side_Sloppy = bit2,
	String_Match_Flag_Slash_Insensitive = bit3,
};

typedef u32 String_Split_Flags;
enum String_Split_Flags {
	String_Split_Flag_Keep_Empties = bit1,
};

typedef struct String_Join String_Join;
struct String_Join {
	string pre;
	string sep;
	string pst;
};

//--------------------------------------------------------------------------------
// path style

typedef enum Path_Style {
  Path_Style_Null,
  Path_Style_Relative,
  Path_Style_Windows_Absolute,
  Path_Style_Unix_Absolute,

#if OS_WINDOWS
  Path_Style_System_Absolute = Path_Style_Windows_Absolute,
#elif OS_LINUX
  Path_Style_System_Absolute = Path_Style_Unix_Absolute,
#else
# error "absolute path style is undefined for this OS"
#endif
} Path_Style;

//--------------------------------------------------------------------------------
// string pair

typedef struct String_Loc_Pair String_Loc_Pair;
struct String_Loc_Pair {
	string str;
	Loc loc;
};

//--------------------------------------------------------------------------------
// IndentParams

typedef u8 Indent_Kind;
enum {
	$indent_kind_spaces,
	$indent_kind_tabs,
	$indent_kind_count,
};

typedef struct $indent_params $indent_params;
struct $indent_params {
	Indent_Kind kind;
	u8           width;
};

//--------------------------------------------------------------------------------
// unicode decode

typedef struct Unicode_Decode Unicode_Decode;
struct Unicode_Decode {
	s32 inc;
	u32 codepoint;
};

//--------------------------------------------------------------------------------
// fuzzy match

typedef struct Fuzzy_Match_Range_Node Fuzzy_Match_Range_Node;
struct Fuzzy_Match_Range_Node {
	Fuzzy_Match_Range_Node *next;
	rng1s64 range;
};

typedef struct Fuzzy_Match_Range_List Fuzzy_Match_Range_List;
struct Fuzzy_Match_Range_List {
	Fuzzy_Match_Range_Node *head;
	Fuzzy_Match_Range_Node *tail;
	s64 count;
	s64 needle_part_count;
	s64 total_dim;
};

//--------------------------------------------------------------------------------
// char procs

proc b8
char_is_space(u8 c);

proc b8
char_is_upper(u8 c);

proc b8
char_is_lower(u8 c);

proc b8
char_is_alpha(u8 c);

proc b8
char_is_slash(u8 c);

proc b8
char_is_digit(u8 c, u32 base);

proc u8
char_to_lower(u8 c);

proc u8
char_to_upper(u8 c);

proc u8
char_to_correct_slash(u8 c);

proc u8
char_to_weak_ass_slash(u8 c);

//--------------------------------------------------------------------------------
// cstring lengths

proc s64
cstring_length(cstring c);

proc s64
cstring16_length(cstring16 c);

proc s64
cstring32_length(cstring32 c);

//--------------------------------------------------------------------------------
// str macros && constructors

#define S(s) make_str((u8*)(s), sizeof(s)-1)
#define SC(s) {(u8*)(s), sizeof(s)-1}
#define svarg(s) (int)((s).size), ((s).m)

#define str_array(s, count) make_str((u8*)(s), sizeof(*(s))*(count))
#define str_array_fixed(s) make_str((u8*)(s), sizeof(s))
#define str_struct(s) make_str((u8*)(s), sizeof(*(s)))

// str8
proc string
make_str(u8 *m, s64 size);

proc string
str_range(u8 *first, u8 *opl);

proc string
str_zero(void);

proc string
str_from_cstring(cstring c);

proc string
str_cstring_scan_to_cap(rawptr start, rawptr cap);

proc string
str_cstring_scan_backwards_from_pos(rawptr start, rawptr pos);

// str16
proc string16
mkstr16(u16 *m, s64 size);

proc string16
str16_range(u16 *first, u16 *opl);

proc string16
str16_zero(void);

proc string16
str16_cstring(cstring16 c);

proc string16
str16_cstring_scan_to_cap(rawptr start, rawptr cap);

proc string16
str16_cstring_scan_backwards_from_pos(rawptr start, rawptr pos);


// str32
proc string32
mkstr32(u32 *m, s64 size);

proc string32
str32_range(u32 *first, u32 *opl);

proc string32
str32_zero(void);

proc string32
str32_cstring(cstring32 c);

proc string32
str32_cstring_scan_to_cap(rawptr start, rawptr cap);

proc string32
str32_cstring_scan_backwards_from_pos(rawptr start, rawptr pos);

//--------------------------------------------------------------------------------
// copy && format

proc string
push_str_cat(Arena *arena, string a, string b);

proc string
push_str_copy(Arena *arena, string str);

proc string
push_strfv(Arena *arena, cstring fmt, va_list args);


CLANG_FORMAT_WARN(2, 3)
proc string
push_strf(Arena *arena, cstring fmt, ...);

//--------------------------------------------------------------------------------
// str stylization

proc string
upper_from_str(Arena *arena, string str);

proc string
lower_from_str(Arena *arena, string str);

proc string
correct_slash_from_str(Arena *arena, string str);

proc string
weak_ass_slash_from_str(Arena *arena, string str);

//--------------------------------------------------------------------------------
// str matching

#define str_match_lit(a_lit, b, flags)   str_match(str_lit(a_lit), (b), (flags))
#define str_match_cstr(a_cstr, b, flags) str_match(str_cstring(a_cstr), (b), (flags))
proc b8
str_match(string a, string b, String_Match_Flags flags);

proc s64
str_find_needle(s64 start_pos, string needle, string haystack, String_Match_Flags flags);

proc s64
str_find_needle_reverse(s64 start_pos, string needle, string haystack, String_Match_Flags flags);

proc b8
str_ends_with(string str, string end, String_Match_Flags flags);
#define str_ends_with_lit(str, end_lit, flags) str_ends_with((str), str_lit(end_lit), (flags))

//--------------------------------------------------------------------------------
// str slicing

proc string
str_substr(string str, rng1s64 range);

proc string
str_prefix(string str, s64 size);

proc string
str_skip(string str, s64 amt);

proc string
str_postfix(string str, s64 size);

proc string
str_chop(string str, s64 amt);

proc string
str_skip_chop_whitespace(string str);

//--------------------------------------------------------------------------------
// str -> integer
proc s64
sign_from_str(string str, string *string_tail);

proc b8
str_is_integer(string str, u32 radix);

proc u64
u64_from_str(string str, u32 radix);

proc s64
s64_from_str(string str, u32 radix);

proc u32
u32_from_str(string str, u32 radix);

proc s32
s32_from_str(string str, u32 radix);

proc b8
try_u64_from_str_c_rules(string str, u64 *x);

proc b8
try_s64_from_str_c_rules(string str, s64 *x);

//--------------------------------------------------------------------------------
// integer -> str
proc string
str_from_memory_size(Arena *arena, s64 size);

proc string
str_from_count(Arena *arena, s64 count);

proc string
str_from_bits_u32(Arena *arena, u32 x);

proc string
str_from_bits_u64(Arena *arena, u64 x);

proc string
str_from_u64(Arena *arena, u64 val, u32 radix, s64 min_digits, u8 digit_group_separator);

proc string
str_from_s64(Arena *arena, s64 val, u32 radix, s64 min_digits, u8 digit_group_separator);

//--------------------------------------------------------------------------------
// float <- str
proc f64
f64_from_str8(string str);

//--------------------------------------------------------------------------------
// str list procs

proc string_node*
str_list_push_node(string_list *list, string_node *node);

proc string_node*
str_list_push_node_set_string(string_list *list, string_node *node, string str);

proc string_node*
str_list_push_node_front(string_list *list, string_node *node);

proc string_node*
str_list_push_node_front_set_string(string_list *list, string_node *node, string str);

proc string_node*
str_list_push(Arena *arena, string_list *list, string str);

proc string_node*
str_list_push_front(Arena *arena, string_list *list, string str);


proc string_node*
str_list_push_copy(Arena *arena, string_list *list, string str);

proc string_node*
str_list_push_front_copy(Arena *arena, string_list *list, string str);


proc void
str_list_concat_in_place(string_list *list, string_list *to_push);

proc string_node*
str_list_push_aligner(Arena *arena, string_list *list, s64 min, s64 align);

proc string_node*
str_list_pushf(Arena *arena, string_list *list, cstring fmt, ...);

proc string_node*
str_list_push_frontf(Arena *arena, string_list *list, cstring fmt, ...);

proc string_list
str_list_copy(Arena *arena, string_list *list);

#define str_list_first(list) ((list)->first ? (list)->first->str : str_zero())

//--------------------------------------------------------------------------------
// splitting & joining

proc string_list
str_split(Arena *arena, string str, u8 *split_chars, s32 split_char_count, String_Split_Flags flags);

proc string_list
str_split_by_string_chars(Arena *arena, string str, string split_chars, String_Split_Flags flags);

proc string_list
str_list_split_by_string_chars(Arena *arena, string_list list, string split_chars, String_Split_Flags flags);

proc string
str_list_join(Arena *arena, string_list *list, String_Join *optional_params);

proc string_list
str_list_from_flags(Arena *arena, u32 flags, string *flag_string_table, s32 flag_string_count);

//--------------------------------------------------------------------------------
// str arrays

proc string_array
str_array_from_list(Arena *arena, string_list *list);

proc string_array
str_array_reserve(Arena *arena, s64 count);

//--------------------------------------------------------------------------------
// path helpers

proc string
str_chop_last_slash(string str);

proc string
str_skip_last_slash(string str);

proc string
str_chop_last_dot(string str);

proc string
str_skip_last_dot(string str);


proc Path_Style
path_style_from_str(string str);

proc string_list
str_split_path(Arena *arena, string str);

proc void
str_path_list_resolve_dots_in_place(string_list *path, Path_Style style);

proc string
str_path_list_join_by_style(Arena *arena, string_list *path, Path_Style style);

proc string
str_extension_from_path(string path);

proc string
str_remove_extension_from_path(string path);


proc String_Loc_Pair
str_loc_pair_from_string(string str);

//--------------------------------------------------------------------------------
// utf-8 / utf-16 encoding / decoding

proc Unicode_Decode
utf8_decode(u8 *str, s64 max);

proc Unicode_Decode
utf16_decode(u16 *str, s64 max);

proc s32
utf8_encode(u8 *str, u32 codepoint);

proc s32
utf16_encode(u16 *str, u32 codepoint);

proc s32
utf8_from_utf32_single(u8 *buffer, u32 character);

//--------------------------------------------------------------------------------
// unicode str conversions

proc string
str_from_str16(Arena *arena, string16 in);

proc string16
str16_from_str(Arena *arena, string in);

proc string
str_from_str32(Arena *arena, string32 in);

proc string32
str32_from_str(Arena *arena, string in);


//--------------------------------------------------------------------------------
// text indenting

proc string
indented_from_string(Arena *arena, string str, $indent_params params);

//--------------------------------------------------------------------------------
// text escaping

proc string
escaped_from_raw_str(Arena *arena, string str);

proc string
raw_from_escaped_str(Arena *arena, string str);

//--------------------------------------------------------------------------------
// str <-> color

proc string
hex_string_from_rgba_4f32(Arena *arena, vec4f32 rgba);

proc vec4f32
rgba_from_hex_string_4f32(string hex_string);

//--------------------------------------------------------------------------------
// str fuzzy matching
proc Fuzzy_Match_Range_List
fuzzy_match_find(Arena *arena, string needle, string haystack);

proc Fuzzy_Match_Range_List
fuzzy_match_range_list_copy(Arena *arena, Fuzzy_Match_Range_List *src);

