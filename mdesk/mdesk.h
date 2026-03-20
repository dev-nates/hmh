// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

#ifndef MDESK_H
#define MDESK_H

//--------------------------------------------------------------------------------
// message types

typedef enum $md_msg_kind $md_msg_kind;
enum $md_msg_kind {
	$md_msg_kind_info,
	$md_msg_kind_note,
	$md_msg_kind_warning,
	$md_msg_kind_error,
	$md_msg_kind_fatal_error,
};

typedef struct $md_msg $md_msg;
struct $md_msg {
	$md_msg *next;
	struct $md_node *node;
	$md_msg_kind kind;
	$string string;
};

typedef struct $md_msg_list $md_msg_list;
struct $md_msg_list {
	$md_msg *first, *last;
	s64 count;
	$md_msg_kind worst_message_kind;
};

//--------------------------------------------------------------------------------
// token types

typedef u32 $md_token_flags;
enum {
	// base kind info
	$md_token_flag_identifier     = bit1,
	$md_token_flag_numeric        = bit2,
	$md_token_flag_string_literal = bit3,
	$md_token_flag_symbol         = bit4,
	$md_token_flag_reserved       = bit5,
	$md_token_flag_comment        = bit6,
	$md_token_flag_whitespace     = bit7,
	$md_token_flag_newline        = bit8,

	// decoration info
	$md_token_flag_string_single_quote = bit9,
	$md_token_flag_string_double_quote = bit10,
	$md_token_flag_string_tick         = bit11,
	$md_token_flag_string_triplet      = bit12,

	// error info
	$md_token_flag_broken_comment        = bit13,
	$md_token_flag_broken_string_literal = bit14,
	$md_token_flag_bad_character         = bit15,
};

typedef u32 $md_token_groups;
enum {
	$md_token_group_comment    = $md_token_flag_comment,
	$md_token_group_whitespace = ($md_token_flag_whitespace | $md_token_flag_newline),
	$md_token_group_irregular  = ($md_token_group_comment | $md_token_group_whitespace),
	$md_token_group_regular    = ~$md_token_group_irregular,
	$md_token_group_label      = ($md_token_flag_identifier | $md_token_flag_numeric | $md_token_flag_string_literal | $md_token_flag_symbol),
	$md_token_group_error      = ($md_token_flag_broken_comment | $md_token_flag_broken_string_literal | $md_token_flag_bad_character),
};

typedef struct $md_token $md_token;
struct $md_token {
	rng1s64 range;
	$md_token_flags flags;
};

typedef struct $md_token_chunk_node $md_token_chunk_node;
struct $md_token_chunk_node {
	$md_token_chunk_node *next;
	$md_token *v;
	s64 count;
	s64 cap;
};

typedef struct $md_token_chunk_list $md_token_chunk_list;
struct $md_token_chunk_list {
	$md_token_chunk_node *first, *last;
	s64 chunk_count;
	s64 total_token_count;
};

typedef struct $md_token_array $md_token_array;
struct $md_token_array {
	$md_token *v;
	s64 count;
};

//--------------------------------------------------------------------------------
// node types

typedef enum $md_node_kind $md_node_kind;
enum $md_node_kind {
	$md_node_kind_nil,
	$md_node_kind_file,
	$md_node_kind_error_marker,
	$md_node_kind_main,
	$md_node_kind_tag,
	$md_node_kind_list,
	$md_node_kind_reference,
	$md_node_kind_count,
};

typedef u32 $md_node_flags;
enum {
	//--------------------------------------------------------------------------------
	$md_node_flag_mask_set_delimiters = bitmask6,
	$md_node_flag_has_paren_left    = bit1,
	$md_node_flag_has_paren_right   = bit2,
	$md_node_flag_has_bracket_left  = bit3,
	$md_node_flag_has_bracket_right = bit4,
	$md_node_flag_has_brace_left    = bit5,
	$md_node_flag_has_brace_right   = bit6,

	//--------------------------------------------------------------------------------
	$md_node_flag_mask_seperators     = (bitmask4<<6),
	$md_node_flag_is_before_semicolon = bit7,
	$md_node_flag_is_after_semicolon  = bit8,
	$md_node_flag_is_before_comma     = bit9,
	$md_node_flag_is_after_comma      = bit10,

	//--------------------------------------------------------------------------------
	$md_node_flag_mask_string_delimiters = (bitmask4<<10),
	$md_node_flag_string_single_quote = bit11,
	$md_node_flag_string_double_quote = bit12,
	$md_node_flag_string_tick         = bit13,
	$md_node_flag_string_triplet      = bit14,

	$md_node_flag_mask_label_kind = (bitmask4<<14),
	$md_node_flag_numeric        = bit15,
	$md_node_flag_identifier     = bit16,
	$md_node_flag_string_literal = bit17,
	$md_node_flag_symbol         = bit18,
};
#define md_node_flag_after_from_before(f) ((f) << 1)

typedef struct $md_node $md_node;
struct $md_node {
	// tree links
	$md_node *next, *prev;
	$md_node *parent;
	$md_node *first, *last;

	// tags
	$md_node *first_tag, *last_tag;

	// info
	$md_node_kind kind;
	$md_node_flags flags;
	$string string;
	$string raw_string;

	s64 src_offset;

	u64 user_gen;

	u64 _unused_[2];
};

typedef struct $md_node_rec $md_node_rec;
struct $md_node_rec {
	$md_node *next;
	s32 push_count;
	s32 pop_count;
};

//--------------------------------------------------------------------------------
// text -> token types

typedef struct $md_tokenize_result $md_tokenize_result;
struct $md_tokenize_result {
	$md_token_array tokens;
	$md_msg_list msgs;
};

//--------------------------------------------------------------------------------
// tokens -> tree types

typedef struct $md_parse_result $md_parse_result;
struct $md_parse_result {
	$md_node *root;
	$md_msg_list msgs;
};

//--------------------------------------------------------------------------------
// globals

global read_only $md_node md_nil_node_struct = {
	&md_nil_node_struct,
	&md_nil_node_struct,
	&md_nil_node_struct,
	&md_nil_node_struct,
	&md_nil_node_struct,
	&md_nil_node_struct,
	&md_nil_node_struct,
};
global read_only $md_node *md_nil_node = &md_nil_node_struct;

//--------------------------------------------------------------------------------
// message procs

proc void
md_msg_list_push($arena *arena, $md_msg_list *list, $md_node *node, $md_msg_kind kind, $string string);

proc void
md_msg_list_pushf($arena *arena, $md_msg_list *list, $md_node *node, $md_msg_kind kind, cstring fmt, ...);

proc void
md_msg_list_concat_in_place($md_msg_list *dst, $md_msg_list *to_push);


//--------------------------------------------------------------------------------
// token procs

proc $md_token
md_mk_token(rng1s64 range, $md_token_flags flags);

proc b32
md_token_match($md_token a, $md_token b);

proc $string_list
md_string_list_from_token_flags($arena *arena, $md_token_flags flags);

proc void
md_token_chunk_list_push($arena *arena, $md_token_chunk_list *list, s64 cap, $md_token token);

proc $md_token_array
md_token_array_from_chunk_list($arena *arena, $md_token_chunk_list *list);

proc $string
md_content_string_from_token_flags_str($md_token_flags flags, $string string);

//--------------------------------------------------------------------------------
// node procs

//--------------------------------------------------------------------------------
// flag conversions
proc $md_node_flags
md_node_flags_from_token_flags($md_token_flags flags);

//--------------------------------------------------------------------------------
// nil
proc b32
md_node_is_nil($md_node *node);

//--------------------------------------------------------------------------------
// flags reflection
proc $string_list
md_string_list_from_node_flags($arena *arena, $md_node_flags flags);

//--------------------------------------------------------------------------------
// iteration
#define md_each_node(it, first) ($md_node *it = first; !md_node_is_nil(it); it = it->next)

//--------------------------------------------------------------------------------
// depth first iteration
proc $md_node_rec
md_node_rec_depth_first($md_node *node, $md_node *subtree_root, s64 child_off, s64 sib_off);
#define md_node_rec_depth_first_pre(node, subtree_root) md_node_rec_depth_first((node), (subtree_root), offset_of($md_node, first), offset_of($md_node, next))
#define md_node_rec_depth_first_pre_rev(node, subtree_root) md_node_rec_depth_first((node), (subtree_root), offset_of($md_node, last), offset_of($md_node, prev))

//--------------------------------------------------------------------------------
// tree building
proc $md_node*
md_push_node($arena *arena, $md_node_kind kind, $md_node_flags flags, $string string, $string raw_string, s64 src_offset);

proc void
md_node_insert_child($md_node *parent, $md_node *prev_child, $md_node *node);

proc void
md_node_insert_tag($md_node *parent, $md_node *prev_tag, $md_node *node);

proc void
md_node_push_child($md_node *parent, $md_node *node);

proc void
md_node_push_tag($md_node *parent, $md_node *node);

proc void
md_unhook($md_node *node);

//--------------------------------------------------------------------------------
// tree introspection

proc $md_node*
md_node_from_chain_string($md_node *first, $md_node *opl, $string string, $string_match_flags flags);

proc $md_node*
md_node_from_chain_index($md_node *first, $md_node *opl, s64 index);

proc $md_node*
md_node_from_chain_flags($md_node *first, $md_node *opl, $md_node_flags flags);


proc s64
md_index_from_node($md_node *node);

proc $md_node*
md_root_from_node($md_node *node);


proc $md_node*
md_child_from_string($md_node *node, $string child_string, $string_match_flags flags);

proc $md_node*
md_tag_from_string($md_node *node, $string tag_string, $string_match_flags flags);

proc $md_node*
md_child_from_index($md_node *node, s64 index);

proc $md_node*
md_tag_from_index($md_node *node, s64 index);


proc $md_node*
md_tag_arg_from_index($md_node *node, $string tag_string, $string_match_flags flags, s64 index);

proc $md_node*
md_tag_arg_from_string($md_node *node, $string tag_string, $string_match_flags tag_str_flags, $string arg_string, $string_match_flags arg_str_flags);


proc b32
md_node_has_child($md_node *node, $string string, $string_match_flags flags);

proc b32
md_node_has_tag($md_node *node, $string string, $string_match_flags flags);

proc s64
md_child_count_from_node($md_node *node);

proc s64
md_tag_count_from_node($md_node *node);


proc $string
md_string_from_children($arena *arena, $md_node *root);

//--------------------------------------------------------------------------------
// tree comparison

proc b32
md_node_match($md_node *a, $md_node *b, $string_match_flags flags);

proc b32
md_tree_match($md_node *a, $md_node *b, $string_match_flags flags);

//--------------------------------------------------------------------------------
// tree duplication

proc $md_node*
md_tree_copy($arena *arena, $md_node *src_root);

//--------------------------------------------------------------------------------
// text -> tokens procs

proc $md_tokenize_result
md_tokenize_from_text($arena *arena, $string text);

//--------------------------------------------------------------------------------
// tokens -> tree procs

proc $md_parse_result
md_parse_from_tokenize_text($arena *arena, $string filename, $string text, $md_token_array tokens);

proc $md_parse_result
md_parse_from_text($arena *arena, $string filename, $string text);
#define md_tree_from_string(arena, string) (md_parse_from_text(arena, str_zero(), (string)).root)

//--------------------------------------------------------------------------------
// tree -> text procs

proc $string_list
md_debug_string_list_from_tree($arena *arena, $md_node *root);

#endif // MDESK_H
