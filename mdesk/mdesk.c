// Copyright (c) 2024 Epic Games Tools
// Licensed under the MIT license (https://opensource.org/license/mit/)

//--------------------------------------------------------------------------------
// message procs

proc void
md_msg_list_push($arena *arena, $md_msg_list *list,
	$md_node *node, $md_msg_kind kind, $string string)
{
	$md_msg *msg = push_array(arena, $md_msg, 1);
	msg->node = node;
	msg->kind = kind;
	msg->string = string;
	sll_queue_push(list->first, list->last, msg);
	list->count += 1;
	list->worst_message_kind = max(kind, list->worst_message_kind);
}

proc void
md_msg_list_pushf($arena *arena, $md_msg_list *list,
	$md_node *node, $md_msg_kind kind, cstring fmt, ...)
{
	$md_msg *msg = push_array(arena, $md_msg, 1);
	msg->node = node;
	msg->kind = kind;

	va_list args;
	va_start(args, fmt);
	$string string = push_strfv(arena, fmt, args);
	va_end(args);

	msg->string = string;
	sll_queue_push(list->first, list->last, msg);
	list->count += 1;
	list->worst_message_kind = max(kind, list->worst_message_kind);
}

proc void
md_msg_list_concat_in_place($md_msg_list *dst, $md_msg_list *to_push) {
	if (to_push->first != 0) {
		if (dst->last) {
			dst->last->next = to_push->first;
			dst->last = to_push->first;
			dst->count += to_push->count;
			dst->worst_message_kind = max(dst->worst_message_kind, to_push->worst_message_kind);
		} else {
			memory_copy_struct(dst, to_push);
		}
	}
	memory_zero_struct(to_push);
}


//--------------------------------------------------------------------------------
// token procs

proc $md_token
md_mk_token(rng1s64 range, $md_token_flags flags) {
	assert(range.min >= 0 && range.min <= range.max);
	$md_token token = {range, flags};
	return token;
}

proc b8
md_token_match($md_token a, $md_token b) {
	return (a.range.min == b.range.min &&
		a.range.max == b.range.max &&
		a.flags == b.flags);
}

proc $string_list
md_string_list_from_token_flags($arena *arena, $md_token_flags flags) {
	$string_list list = zero_struct;
	if (flags & $md_token_flag_identifier)            {str_list_push(arena, &list, strlit("Identifier"));}
	if (flags & $md_token_flag_numeric)               {str_list_push(arena, &list, strlit("Numeric"));}
	if (flags & $md_token_flag_string_literal)        {str_list_push(arena, &list, strlit("StringLiteral"));}
	if (flags & $md_token_flag_symbol)                {str_list_push(arena, &list, strlit("Symbol"));}
	if (flags & $md_token_flag_reserved)              {str_list_push(arena, &list, strlit("Reserved"));}
	if (flags & $md_token_flag_comment)               {str_list_push(arena, &list, strlit("Comment"));}
	if (flags & $md_token_flag_whitespace)            {str_list_push(arena, &list, strlit("Whitespace"));}
	if (flags & $md_token_flag_newline)               {str_list_push(arena, &list, strlit("Newline"));}
	if (flags & $md_token_flag_broken_comment)        {str_list_push(arena, &list, strlit("BrokenComment"));}
	if (flags & $md_token_flag_broken_string_literal) {str_list_push(arena, &list, strlit("BrokenStringLiteral"));}
	if (flags & $md_token_flag_bad_character)         {str_list_push(arena, &list, strlit("BrokenStringLiteral"));}
	return list;
}

proc void
md_token_chunk_list_push($arena *arena, $md_token_chunk_list *list, s64 cap, $md_token token) {
	assert(cap > 0);
	$md_token_chunk_node *node = list->last;
	if (node == 0 || node->count >= node->cap) {
		node = push_array(arena, $md_token_chunk_node, 1);
		node->cap = cap;
		node->v = push_array(arena, $md_token, cap);
		sll_queue_push(list->first, list->last, node);
		list->chunk_count += 1;
	}
	memory_copy_struct(&node->v[node->count], &token);
	node->count += 1;
	list->total_token_count += 1;
}

proc $md_token_array
md_token_array_from_chunk_list($arena *arena, $md_token_chunk_list *list) {
	$md_token_array result;
	result.count = list->total_token_count;
	result.v = push_array(arena, $md_token, list->total_token_count);
	s64 at = 0;
	for each_node($md_token_chunk_node, node, list->first) {
		$md_token *dst = result.v + at;
		$md_token *src = node->v;
		memory_copy_typed(dst, src, node->count);
		at += node->count;
		assert(at <= result.count);
	}
	return result;
}

proc $string
md_content_string_from_token_flags_str($md_token_flags flags, $string string) {
	s64 num_chop = 0;
	s64 num_skip = 0;
	{
		num_skip += 3*!!(flags & $md_token_flag_string_triplet);
		num_chop += 3*!!(flags & $md_token_flag_string_triplet);
    num_skip += 1*(!(flags & $md_token_flag_string_triplet) && flags & $md_token_flag_string_literal);
    num_chop += 1*(!(flags & $md_token_flag_string_triplet) && flags & $md_token_flag_string_literal);
	}
	$string result = string;
	result = str_chop(result, num_chop);
	result = str_skip(result, num_skip);
	return result;
}

//--------------------------------------------------------------------------------
// node procs

//--------------------------------------------------------------------------------
// flag conversions
proc $md_node_flags
md_node_flags_from_token_flags($md_token_flags flags) {
	$md_node_flags result = zero_struct;
	result |= $md_node_flag_identifier     * !!(flags & $md_token_flag_identifier);
	result |= $md_node_flag_numeric        * !!(flags & $md_token_flag_numeric);
	result |= $md_node_flag_string_literal * !!(flags & $md_token_flag_string_literal);
	result |= $md_node_flag_symbol         * !!(flags & $md_token_flag_symbol);
	result |= $md_node_flag_string_single_quote   * !!(flags & $md_token_flag_string_single_quote);
	result |= $md_node_flag_string_double_quote   * !!(flags & $md_token_flag_string_double_quote);
	result |= $md_node_flag_string_tick    * !!(flags & $md_token_flag_string_tick);
	result |= $md_node_flag_string_triplet * !!(flags & $md_token_flag_string_triplet);
	return result;
}

//--------------------------------------------------------------------------------
// nil

proc b8
md_node_is_nil($md_node *node) {
	return check_nil(node, md_nil_node) || node->kind == $md_node_kind_nil;
}

//--------------------------------------------------------------------------------
// flags reflection
proc $string_list
md_string_list_from_node_flags($arena *arena, $md_node_flags flags) {
	$string_list list = zero_struct;
	if (flags & $md_node_flag_has_paren_left) { str_list_push(arena, &list, strlit("Has Paren Left")); }
	if (flags & $md_node_flag_has_paren_right) { str_list_push(arena, &list, strlit("Has Paren Right")); }
	if (flags & $md_node_flag_has_bracket_left) { str_list_push(arena, &list, strlit("Has Bracket Left")); }
	if (flags & $md_node_flag_has_bracket_right) { str_list_push(arena, &list, strlit("Has Bracket Right")); }

	if (flags & $md_node_flag_is_before_semicolon) { str_list_push(arena, &list, strlit("Is Before Semicolon")); }
	if (flags & $md_node_flag_is_after_semicolon) { str_list_push(arena, &list, strlit("Is After Semicolon")); }
	if (flags & $md_node_flag_is_before_comma) { str_list_push(arena, &list, strlit("Is Before Comma")); }
	if (flags & $md_node_flag_is_after_comma) { str_list_push(arena, &list, strlit("Is After Comma")); }

	if (flags & $md_node_flag_string_single_quote) { str_list_push(arena, &list, strlit("String Single Quote")); }
	if (flags & $md_node_flag_string_double_quote) { str_list_push(arena, &list, strlit("String Double Quote")); }
	if (flags & $md_node_flag_string_tick) { str_list_push(arena, &list, strlit("String Tick")); }
	if (flags & $md_node_flag_string_triplet) { str_list_push(arena, &list, strlit("String Triplet")); }

	if (flags & $md_node_flag_numeric) { str_list_push(arena, &list, strlit("String Numeric")); }
	if (flags & $md_node_flag_identifier) { str_list_push(arena, &list, strlit("String Identifier")); }
	if (flags & $md_node_flag_string_literal) { str_list_push(arena, &list, strlit("String Literal")); }
	if (flags & $md_node_flag_symbol) { str_list_push(arena, &list, strlit("Symbol")); }

	return list;
}

//--------------------------------------------------------------------------------
// depth first iteration

proc $md_node_rec
md_node_rec_depth_first($md_node *node, $md_node *subtree_root,
	s64 child_off, s64 sib_off)
{
	$md_node_rec rec = zero_struct;
	rec.next = md_nil_node;
	$md_node *child_link = *member_from_offset($md_node **, node, child_off);
	if (!md_node_is_nil(child_link)) {
		rec.next = child_link;
		rec.push_count = 1;
	}
	else for ($md_node *p = node;
		!md_node_is_nil(p) && p != subtree_root;
		p = p->parent, rec.pop_count += 1)
	{
		if (!md_node_is_nil(*member_from_offset($md_node **, p, sib_off))) {
			rec.next = *member_from_offset($md_node **, p, sib_off);
			break;
		}	
	}
	return rec;
}

//--------------------------------------------------------------------------------
// tree building

proc $md_node*
md_push_node($arena *arena, $md_node_kind kind, $md_node_flags flags,
	$string string, $string raw_string, s64 src_offset)
{
	$md_node *node = push_array(arena, $md_node, 1);
	node->first = node->last = node->parent = node->next = node->prev =
		node->first_tag = node->last_tag = md_nil_node;
	node->kind = kind;
	node->flags = flags;
	node->string = string;
	node->raw_string = raw_string;
	node->src_offset = src_offset;
	return node;
}

proc void
md_node_insert_child($md_node *parent,
	$md_node *prev_child, $md_node *node)
{
	node->parent = parent;
	dll_insert_npz(md_nil_node, parent->first, parent->last, prev_child, node, next, prev);
}

proc void
md_node_insert_tag($md_node *parent,
	$md_node *prev_tag, $md_node *node)
{
	node->kind = $md_node_kind_tag;
	node->parent = parent;
	dll_insert_npz(md_nil_node, parent->first_tag, parent->last_tag, prev_tag, node, next, prev);
}

proc void
md_node_push_child($md_node *parent, $md_node *node) {
	node->parent = parent;
	dll_push_back_npz(md_nil_node, parent->first, parent->last, node, next, prev);
}

proc void
md_node_push_tag($md_node *parent, $md_node *node) {
	node->kind = $md_node_kind_tag;
	node->parent = parent;
	dll_push_back_npz(md_nil_node, parent->first_tag, parent->last_tag, node, next, prev);
}

proc void
md_unhook($md_node *node) {
	$md_node *parent = node->parent;
	if (!md_node_is_nil(parent)) {
		if (node->kind == $md_node_kind_tag) {
			dll_remove_npz(md_nil_node, parent->first_tag, parent->last_tag, node, next, prev);
		} else {
			dll_remove_npz(md_nil_node, parent->first, parent->last, node, next, prev);
		}
		node->parent = md_nil_node;
	}
}

//--------------------------------------------------------------------------------
// tree introspection

proc $md_node*
md_node_from_chain_string($md_node *first, $md_node *opl,
	$string string, $string_match_flags flags)
{
	$md_node *result = md_nil_node;
	for ($md_node *n = first; !md_node_is_nil(n) && n != opl; n = n->next) {
		if (str_match(n->string, string, flags)) {
			result = n;
			break;
		}
	}
	return result;
}

proc $md_node*
md_node_from_chain_index($md_node *first,
	$md_node *opl, s64 index)
{
	$md_node *result = md_nil_node;
	s64 idx = 0;
	for ($md_node *n = first;
		!md_node_is_nil(n) && n != opl;
		n = n->next, idx += 1)
	{
		if (index == idx) {
			result = n;
			break;
		}
	}
	return result;
}

proc $md_node*
md_node_from_chain_flags($md_node *first, $md_node *opl, $md_node_flags flags)
{
	$md_node *result = md_nil_node;
	for ($md_node *n = first; !md_node_is_nil(n) && n != opl; n = n->next) {
		if (n->flags & flags) {
			result = n;
			break;
		}
	}
	return result;
}

proc s64
md_index_from_node($md_node *node) {
	s64 index = 0;
	for ($md_node *n = node->prev; !md_node_is_nil(n); n = n->prev) {
		index += 1;
	} 
	return index;
}

proc $md_node*
md_root_from_node($md_node *node) {
	$md_node *result = node;
	for ($md_node *p = node->parent;
		(p->kind == $md_node_kind_main || p->kind == $md_node_kind_tag) && !md_node_is_nil(p);
		p = p->parent)
	{
			result = p;
	}
	return result;
}

proc $md_node*
md_child_from_string($md_node *node, $string child_string, $string_match_flags flags) {
	return md_node_from_chain_string(node->first, md_nil_node, child_string, flags);
}

proc $md_node*
md_tag_from_string($md_node *node, $string tag_string, $string_match_flags flags) {
	return md_node_from_chain_string(node->first_tag, md_nil_node, tag_string, flags);
}

proc $md_node*
md_child_from_index($md_node *node, s64 index) {
	return md_node_from_chain_index(node->first, md_nil_node, index);
}

proc $md_node*
md_tag_from_index($md_node *node, s64 index) {
	return md_node_from_chain_index(node->first_tag, md_nil_node, index);
}

proc $md_node*
md_tag_arg_from_index($md_node *node, $string tag_string, $string_match_flags flags, s64 index) {
	$md_node *tag = md_tag_from_string(node, tag_string, flags);
	return md_child_from_index(tag, index);
}

proc $md_node*
md_tag_arg_from_string($md_node *node,
	$string tag_string, $string_match_flags tag_str_flags,
	$string arg_string, $string_match_flags arg_str_flags)
{
	$md_node *tag = md_tag_from_string(node, tag_string, tag_str_flags);
	return md_child_from_string(tag, arg_string, arg_str_flags);
}

proc b8
md_node_has_child($md_node *node,
	$string string, $string_match_flags flags)
{
	return !md_node_is_nil(md_child_from_string(node, string, flags));
}

proc b8
md_node_has_tag($md_node *node, $string string, $string_match_flags flags) {
	return !md_node_is_nil(md_tag_from_string(node, string, flags));
}

proc s64
md_child_count_from_node($md_node *node) {
	s64 result = 0;
	for ($md_node *n = node->first; !md_node_is_nil(n); n = n->next) {
		result += 1;
	}
	return result;
}

proc s64
md_tag_count_from_node($md_node *node) {
	s64 result = 0;
	for ($md_node *n = node->first_tag; !md_node_is_nil(n); n = n->next) {
		result += 1;
	}
	return result;
}

proc $string
md_string_from_children($arena *arena, $md_node *root) {
	$temp scratch = scratch_begin(&arena, 1);
	$string_list list = zero_struct;

	for md_each_node(child, root->first) {
		if (child->flags == child->prev->flags) {
			str_list_push(scratch.arena, &list, strlit(" "));
		}
		str_list_push(scratch.arena, &list, child->string);
	}
	$string result = str_list_join(arena, &list, 0);
	scratch_end(scratch);
	return result;
}

//--------------------------------------------------------------------------------
// tree comparison

proc b8
md_node_match($md_node *a, $md_node *b, $string_match_flags flags) {
	b32 result = 0;
	if (a->kind == b->kind && str_match(a->string, b->string, flags)) {
		result = 1;
		if (result) {
			result = result && a->flags == b->flags;
		}
		if (result && a->kind != $md_node_kind_tag) {
			for ($md_node *a_tag = a->first_tag, *b_tag = b->first_tag;
				!md_node_is_nil(a_tag) && !md_node_is_nil(b_tag);
				a_tag = a_tag->next, b_tag = b_tag->next)
			{
				if (md_node_match(a_tag, b_tag, flags)) {
					for ($md_node *a_tag_arg = a_tag->first, *b_tag_arg = b_tag->first;
						!md_node_is_nil(a_tag_arg) && !md_node_is_nil(b_tag_arg);
						a_tag_arg = a_tag_arg->next, b_tag_arg = b_tag_arg->next)
					{
						if (!md_tree_match(a_tag_arg, b_tag_arg, flags)) {
							result = 0;
							goto end;
						}
					}
				} else {
					result = 0;
					goto end;
				}
			}
		}
	}
	end:;
	return result;
}

proc b8
md_tree_match($md_node *a, $md_node *b, $string_match_flags flags) {
	b32 result = md_node_match(a, b, flags);
	if (result) {
		for ($md_node *a_child = a->first, *b_child = b->first;
			!md_node_is_nil(a_child) && !md_node_is_nil(b_child);
			a_child = a_child->next, b_child = b_child->next)
		{
			if (!md_tree_match(a_child, b_child, flags)) {
				result = 0;
				goto end;
			}
		}
	}
	end:;
	return result;
}

//--------------------------------------------------------------------------------
// tree duplication

proc $md_node*
md_tree_copy($arena *arena, $md_node *src_root) {
	$md_node *dst_root = md_nil_node;
	$md_node *dst_parent = dst_root;
	{
		$md_node_rec rec = zero_struct;
		// depth first copy
		for ($md_node *src = src_root; !md_node_is_nil(src); src = rec.next) {
			$md_node *dst = push_array(arena, $md_node, 1);
			dst->first = dst->last = dst->parent = dst->next = dst->prev = md_nil_node;
			dst->first_tag = dst->last_tag = md_nil_node;
			dst->kind = src->kind;
			dst->flags = src->flags;
			dst->string = push_str_copy(arena, src->string);
			dst->raw_string = push_str_copy(arena, src->raw_string);
			dst->src_offset = src->src_offset;
			dst->parent = dst_parent;

			if (dst_parent != md_nil_node) {
				dll_push_back_npz(md_nil_node, dst_parent->first, dst_parent->last, dst, next, prev);
			} else {
				dst_root = dst_parent = dst;
			}

			rec = md_node_rec_depth_first_pre(src, src_root);
			if (rec.push_count != 0) {
				dst_parent = dst;
			} else for (s64 idx = 0; idx < rec.pop_count; idx += 1) {
				dst_parent = dst_parent->parent;
			}
		}
	}
	return dst_root;
}

//--------------------------------------------------------------------------------
// text -> tokens procs

proc $md_tokenize_result
md_tokenize_from_text($arena *arena, $string text) {
	$temp scratch = scratch_begin(&arena, 1);
	$md_token_chunk_list tokens = {0};
	$md_msg_list msgs = {0};
	u8 *byte_first = text.m;
	u8 *byte_opl = byte_first + text.size;
	u8 *byte = byte_first;
	
	//- rjf: scan string & produce tokens
	for(;byte < byte_opl;)
	{
		$md_token_flags token_flags = 0;
		u8 *token_start = 0;
		u8 *token_opl = 0;
		
		//- rjf: whitespace
		if(token_flags == 0 && (*byte == ' ' || *byte == '\t' || *byte == '\v' || *byte == '\r'))
		{
			token_flags = $md_token_flag_whitespace;
			token_start = byte;
			token_opl = byte;
			byte += 1;
			for(;byte <= byte_opl; byte += 1) {
				token_opl += 1;
				if(byte == byte_opl || (*byte != ' ' && *byte != '\t' && *byte != '\v' && *byte != '\r'))
				{
					break;
				}
			}
		}
		
		//- rjf: newlines
		if(token_flags == 0 && *byte == '\n')
		{
			token_flags = $md_token_flag_newline;
			token_start = byte;
			token_opl = byte+1;
			byte += 1;
		}
		
		//- rjf: single-line comments
		if(token_flags == 0 && (byte+1 < byte_opl && *byte == '/' && byte[1] == '/'))
		{
			token_flags = $md_token_flag_comment;
			token_start = byte;
			token_opl = byte+2;
			byte += 2;
			b32 escaped = 0;
			for(;byte <= byte_opl; byte += 1)
			{
				token_opl += 1;
				if(byte == byte_opl) {
					break;
				}

				if(escaped) {
					escaped = 0;
				} else {
					if(*byte == '\n') {
						break;
					} else if(*byte == '\\') {
						escaped = 1;
					}
				}
			}
		}
		
		//- rjf: multi-line comments
		if(token_flags == 0 && (byte+1 < byte_opl && *byte == '/' && byte[1] == '*'))
		{
			token_flags = $md_token_flag_comment;
			token_start = byte;
			token_opl = byte+2;
			byte += 2;
			for(;byte <= byte_opl; byte += 1)
			{
				token_opl += 1;
				if(byte == byte_opl) {
					token_flags |= $md_token_flag_broken_comment;
					break;
				}

				if(byte+1 < byte_opl && byte[0] == '*' && byte[1] == '/') {
					token_opl += 2;
					break;
				}
			}
		}
		
		//- rjf: identifiers
		if(token_flags == 0 && (('A' <= *byte && *byte <= 'Z') ||
			('a' <= *byte && *byte <= 'z') ||
			*byte == '_' ||
			utf8_class[*byte>>3] >= 2 ))
		{
			token_flags = $md_token_flag_identifier;
			token_start = byte;
			token_opl = byte;
			byte += 1;
			for(;byte <= byte_opl; byte += 1) {
				token_opl += 1;
				if(byte == byte_opl ||
					(!('A' <= *byte && *byte <= 'Z') &&
					!('a' <= *byte && *byte <= 'z') &&
					!('0' <= *byte && *byte <= '9') &&
					*byte != '_' &&
					utf8_class[*byte>>3] < 2))
				{
					break;
				}
			}
		}
		
		//- rjf: numerics
		if(token_flags == 0 && (('0' <= *byte && *byte <= '9') ||
			(*byte == '.' && byte+1 < byte_opl && '0' <= byte[1] && byte[1] <= '9') ||
			(*byte == '-' && byte+1 < byte_opl && '0' <= byte[1] && byte[1] <= '9') ||
			*byte == '_'))
		{
			token_flags = $md_token_flag_numeric;
			token_start = byte;
			token_opl = byte;
			byte += 1;
			for(;byte <= byte_opl; byte += 1) {
				token_opl += 1;
				if(byte == byte_opl ||
					(!('A' <= *byte && *byte <= 'Z') &&
					!('a' <= *byte && *byte <= 'z') &&
					!('0' <= *byte && *byte <= '9') &&
					*byte != '_' &&
					*byte != '.'))
				{
					break;
				}
			}
		}
		
		//- rjf: triplet string literals
		if(token_flags == 0 && byte+2 < byte_opl &&
			((byte[0] == '"' && byte[1] == '"' && byte[2] == '"') ||
			(byte[0] == '\''&& byte[1] == '\''&& byte[2] == '\'') ||
			(byte[0] == '`' && byte[1] == '`' && byte[2] == '`')))
		{
			u8 literal_style = byte[0];
			token_flags = $md_token_flag_string_literal|$md_token_flag_string_triplet;
			token_flags |= (literal_style == '\'')*$md_token_flag_string_single_quote;
			token_flags |= (literal_style ==  '"')*$md_token_flag_string_double_quote;
			token_flags |= (literal_style ==  '`')*$md_token_flag_string_tick;
			token_start = byte;
			token_opl = byte+3;
			byte += 3;
			for(;byte <= byte_opl; byte += 1) {
				if(byte == byte_opl) {
					token_flags |= $md_token_flag_broken_string_literal;
					token_opl = byte;
					break;
				}

				if(byte+2 < byte_opl && (byte[0] == literal_style && byte[1] == literal_style && byte[2] == literal_style)) {
					byte += 3;
					token_opl = byte;
					break;
				}
			}
		}
		
		//- rjf: singlet string literals
		if(token_flags == 0 && (byte[0] == '"' || byte[0] == '\'' || byte[0] == '`'))
		{
			u8 literal_style = byte[0];
			token_flags = $md_token_flag_string_literal;
			token_flags |= (literal_style == '\'')*$md_token_flag_string_single_quote;
			token_flags |= (literal_style ==  '"')*$md_token_flag_string_double_quote;
			token_flags |= (literal_style ==  '`')*$md_token_flag_string_tick;
			token_start = byte;
			token_opl = byte+1;
			byte += 1;
			b32 escaped = 0;
			for(;byte <= byte_opl; byte += 1) {
				if(byte == byte_opl || *byte == '\n') {
					token_opl = byte;
					token_flags |= $md_token_flag_broken_string_literal;
					break;
				}

				if(!escaped && byte[0] == '\\') {
					escaped = 1;
				} else if(!escaped && byte[0] == literal_style) {
					token_opl = byte+1;
					byte += 1;
					break;
				} else if(escaped) {
					escaped = 0;
				}
			}
		}
		
		//- rjf: non-reserved symbols
		if(token_flags == 0 && (*byte == '~' || *byte == '!' || *byte == '$' || *byte == '%' || *byte == '^' ||
			*byte == '&' || *byte == '*' || *byte == '-' || *byte == '=' || *byte == '+' ||
			*byte == '<' || *byte == '.' || *byte == '>' || *byte == '/' || *byte == '?' ||
			*byte == '|'))
		{
			token_flags = $md_token_flag_symbol;
			token_start = byte;
			token_opl = byte;
			byte += 1;
			for(;byte <= byte_opl; byte += 1)
			{
				token_opl += 1;
				if(byte == byte_opl ||
					(*byte != '~' && *byte != '!' && *byte != '$' && *byte != '%' && *byte != '^' &&
					*byte != '&' && *byte != '*' && *byte != '-' && *byte != '=' && *byte != '+' &&
					*byte != '<' && *byte != '.' && *byte != '>' && *byte != '/' && *byte != '?' &&
					*byte != '|'))
				{
					break;
				}
			}
		}
		
		//- rjf: reserved symbols
		if(token_flags == 0 && (*byte == '{' || *byte == '}' || *byte == '(' || *byte == ')' ||
			*byte == '[' || *byte == ']' || *byte == '#' || *byte == ',' ||
			*byte == '\\'|| *byte == ':' || *byte == ';' || *byte == '@'))
		{
			token_flags = $md_token_flag_reserved;
			token_start = byte;
			token_opl = byte+1;
			byte += 1;
		}
		
		//- rjf: bad characters in all other cases
		if (token_flags == 0) {
			token_flags = $md_token_flag_bad_character;
			token_start = byte;
			token_opl = byte+1;
			byte += 1;
		}
		
		//- rjf; push token if formed
		if (token_flags != 0 && token_start != 0 && token_opl > token_start) {
			$md_token token = {{(token_start - byte_first), (token_opl - byte_first)}, token_flags};
			md_token_chunk_list_push(scratch.arena, &tokens, 4096, token);
		}
		
		//- rjf: push errors on unterminated comments
		if (token_flags & $md_token_flag_broken_comment) {
			$md_node *error = md_push_node(arena, $md_node_kind_error_marker, 0, strlit(""), strlit(""), token_start - byte_first);
			$string error_string = strlit("Unterminated comment.");
			md_msg_list_push(arena, &msgs, error, $md_msg_kind_error, error_string);
		}
		
		//- rjf: push errors on unterminated strings
		if(token_flags & $md_token_flag_broken_string_literal) {
			$md_node *error = md_push_node(arena, $md_node_kind_error_marker, 0, strlit(""), strlit(""), token_start - byte_first);
			$string error_string = strlit("Unterminated string literal.");
			md_msg_list_push(arena, &msgs, error, $md_msg_kind_error, error_string);
		}
	}
	
	//- rjf: bake, fill & return
	$md_tokenize_result result = zero_struct;
	{
		result.tokens = md_token_array_from_chunk_list(arena, &tokens);
		result.msgs = msgs;
	}
	scratch_end(scratch);
	return result;
}

//--------------------------------------------------------------------------------
// tokens -> tree procs

proc $md_parse_result
md_parse_from_tokenize_text($arena *arena, $string filename, $string text, $md_token_array tokens) {
	$temp scratch = scratch_begin(&arena, 1);
	
	//- rjf: set up outputs
	$md_msg_list msgs = {0};
	$md_node *root = md_push_node(arena, $md_node_kind_file, 0, filename, text, 0);
	
	//- rjf: set up parse rule stack
	typedef enum $md_parse_work_kind {
		$md_parse_work_kind_main,
		$md_parse_work_kind_main_implicit,
		$md_parse_work_kind_node_optional_follow_up,
		$md_parse_work_kind_node_children_style_scan,
	} $md_parse_work_kind;

	typedef struct $md_parse_work_node $md_parse_work_node;
	struct $md_parse_work_node
	{
		$md_parse_work_node *next;
		$md_parse_work_kind kind;
		$md_node *parent;
		$md_node *first_gathered_tag;
		$md_node *last_gathered_tag;
		$md_node_flags gathered_node_flags;
		s32 counted_newlines;
	};
	$md_parse_work_node first_work =
	{
		0,
		$md_parse_work_kind_main,
		root,
	};
	$md_parse_work_node broken_work = { 0, $md_parse_work_kind_main, root,};
	$md_parse_work_node *work_top = &first_work;
	$md_parse_work_node *work_free = 0;
#define md_parse_work_push(work_kind, work_parent) do\
{\
$md_parse_work_node *work_node = work_free;\
if(work_node == 0) {work_node = push_array(scratch.arena, $md_parse_work_node, 1);}\
else { sll_stack_pop(work_free); }\
work_node->kind = (work_kind);\
work_node->parent = (work_parent);\
sll_stack_push(work_top, work_node);\
}while(0)
#define md_parse_work_pop() do\
{\
sll_stack_pop(work_top);\
if(work_top == 0) {work_top = &broken_work;}\
}while(0)
	
	//- rjf: parse
	$md_token *tokens_first = tokens.v;
	$md_token *tokens_opl = tokens_first + tokens.count;
	$md_token *token = tokens_first;
	for(;token < tokens_opl;) {
		//- rjf: unpack token
		$string token_string = str_substr(text, token[0].range);
		
		//- rjf: whitespace -> always no-op & inc
		if(token->flags & $md_token_flag_whitespace) {
			token += 1;
			goto end_consume;
		}
		
		//- rjf: comments -> always no-op & inc
		if(token->flags & $md_token_group_comment) {
			token += 1;
			goto end_consume;
		}
		
		//- rjf: [node follow up] : following label -> work top parent has children. we need
		// to scan for explicit delimiters, else parse an implicitly delimited set of children
		if(work_top->kind == $md_parse_work_kind_node_optional_follow_up && str_match(token_string, strlit(":"), 0)) {
			$md_node *parent = work_top->parent;
			md_parse_work_pop();
			md_parse_work_push($md_parse_work_kind_node_children_style_scan, parent);
			token += 1;
			goto end_consume;
		}
		
		//- rjf: [node follow up] anything but : following label -> node has no children. just
		// pop & move on
		if(work_top->kind == $md_parse_work_kind_node_optional_follow_up) {
			md_parse_work_pop();
			goto end_consume;
		}
		
		//- rjf: [main] separators -> mark & inc
		if(work_top->kind == $md_parse_work_kind_main && token->flags & $md_token_flag_reserved &&
			(str_match(token_string, strlit(","), 0) ||
			str_match(token_string, strlit(";"), 0)))
		{
			$md_node *parent = work_top->parent;
			if(!md_node_is_nil(parent->last)) {
				parent->last->flags |=     $md_node_flag_is_before_comma*!!str_match(token_string, strlit(","), 0);
				parent->last->flags |= $md_node_flag_is_before_semicolon*!!str_match(token_string, strlit(";"), 0);
				work_top->gathered_node_flags |=     $md_node_flag_is_after_comma*!!str_match(token_string, strlit(","), 0);
				work_top->gathered_node_flags |= $md_node_flag_is_after_semicolon*!!str_match(token_string, strlit(";"), 0);
			}
			token += 1;
			goto end_consume;
		}
		
		//- rjf: [main_implicit] separators -> pop
		if(work_top->kind == $md_parse_work_kind_main_implicit && token->flags & $md_token_flag_reserved &&
			(str_match(token_string, strlit(","), 0) ||
			str_match(token_string, strlit(";"), 0)))
		{
			md_parse_work_pop();
			goto end_consume;
		}
		
		//- rjf: [main, main_implicit] unexpected reserved tokens
		if((work_top->kind == $md_parse_work_kind_main || work_top->kind == $md_parse_work_kind_main_implicit) &&
			token->flags & $md_token_flag_reserved &&
			(str_match(token_string, strlit("#"), 0) ||
			str_match(token_string, strlit("\\"), 0) ||
			str_match(token_string, strlit(":"), 0)))
		{
			$md_node *error = md_push_node(arena, $md_node_kind_error_marker, 0, token_string, token_string, token->range.min);
			$string error_string = push_strf(arena, "Unexpected reserved symbol \"%S\".", token_string);
			md_msg_list_push(arena, &msgs, error, $md_msg_kind_error, error_string);
			token += 1;
			goto end_consume;
		}
		
		//- rjf: [main, main_implicit] tag signifier -> create new tag
		if((work_top->kind == $md_parse_work_kind_main || work_top->kind == $md_parse_work_kind_main_implicit) &&
			token[0].flags & $md_token_flag_reserved && str_match(token_string, strlit("@"), 0))
		{
			if(token+1 >= tokens_opl || !(token[1].flags & $md_token_group_label)) {
				$md_node *error = md_push_node(arena, $md_node_kind_error_marker, 0, token_string, token_string, token->range.min);
				$string error_string = strlit("Tag label expected after @ symbol.");
				md_msg_list_push(arena, &msgs, error, $md_msg_kind_error, error_string);
				token += 1;
				goto end_consume;
			} else {
				$string tag_name_raw = str_substr(text, token[1].range);
				$string tag_name = md_content_string_from_token_flags_str(token[1].flags, tag_name_raw);
				$md_node *node = md_push_node(arena, $md_node_kind_tag, md_node_flags_from_token_flags(token[1].flags), tag_name, tag_name_raw, token[0].range.min);
				dll_push_back_npz(md_nil_node, work_top->first_gathered_tag, work_top->last_gathered_tag, node, next, prev);
				if(token+2 < tokens_opl && token[2].flags & $md_token_flag_reserved && str_match(str_substr(text, token[2].range), strlit("("), 0))
				{
					token += 3;
					md_parse_work_push($md_parse_work_kind_main, node);
				} else {
					token += 2;
				}
				goto end_consume;
			}
		}
		
		//- rjf: [main, main_implicit] label -> create new main
		if((work_top->kind == $md_parse_work_kind_main || work_top->kind == $md_parse_work_kind_main_implicit) &&
			token->flags & $md_token_group_label)
		{
			$string node_string_raw = token_string;
			$string node_string = md_content_string_from_token_flags_str(token->flags, node_string_raw);
			$md_node_flags flags = md_node_flags_from_token_flags(token->flags)|work_top->gathered_node_flags;
			work_top->gathered_node_flags = 0;
			$md_node *node = md_push_node(arena, $md_node_kind_main, flags, node_string, node_string_raw, token[0].range.min);
			node->first_tag = work_top->first_gathered_tag;
			node->last_tag = work_top->last_gathered_tag;
			for($md_node *tag = work_top->first_gathered_tag; !md_node_is_nil(tag); tag = tag->next)
			{
				tag->parent = node;
			}
			work_top->first_gathered_tag = work_top->last_gathered_tag = md_nil_node;
			md_node_push_child(work_top->parent, node);
			md_parse_work_push($md_parse_work_kind_node_optional_follow_up, node);
			token += 1;
			goto end_consume;
		}
		
		//- rjf: [main] {s, [s, and (s -> create new main
		if(work_top->kind == $md_parse_work_kind_main && token->flags & $md_token_flag_reserved &&
			(str_match(token_string, strlit("{"), 0) ||
			str_match(token_string, strlit("["), 0) ||
			str_match(token_string, strlit("("), 0)))
		{
			$md_node_flags flags = md_node_flags_from_token_flags(token->flags)|work_top->gathered_node_flags;
			flags |=   $md_node_flag_has_brace_left*!!str_match(token_string, strlit("{"), 0);
			flags |= $md_node_flag_has_bracket_left*!!str_match(token_string, strlit("["), 0);
			flags |=   $md_node_flag_has_paren_left*!!str_match(token_string, strlit("("), 0);
			work_top->gathered_node_flags = 0;
			$md_node *node = md_push_node(arena, $md_node_kind_main, flags, strlit(""), strlit(""), token[0].range.min);
			node->first_tag = work_top->first_gathered_tag;
			node->last_tag = work_top->last_gathered_tag;
			for($md_node *tag = work_top->first_gathered_tag; !md_node_is_nil(tag); tag = tag->next)
			{
				tag->parent = node;
			}
			work_top->first_gathered_tag = work_top->last_gathered_tag = md_nil_node;
			md_node_push_child(work_top->parent, node);
			md_parse_work_push($md_parse_work_kind_main, node);
			token += 1;
			goto end_consume;
		}
		
		//- rjf: [node children style scan] {s, [s, and (s -> explicitly delimited children
		if(work_top->kind == $md_parse_work_kind_node_children_style_scan && token->flags & $md_token_flag_reserved &&
			(str_match(token_string, strlit("{"), 0) ||
			str_match(token_string, strlit("["), 0) ||
			str_match(token_string, strlit("("), 0)))
		{
			$md_node *parent = work_top->parent;
			parent->flags |=   $md_node_flag_has_brace_left*!!str_match(token_string, strlit("{"), 0);
			parent->flags |= $md_node_flag_has_bracket_left*!!str_match(token_string, strlit("["), 0);
			parent->flags |=   $md_node_flag_has_paren_left*!!str_match(token_string, strlit("("), 0);
			md_parse_work_pop();
			md_parse_work_push($md_parse_work_kind_main, parent);
			token += 1;
			goto end_consume;
		}
		
		//- rjf: [node children style scan] count newlines
		if(work_top->kind == $md_parse_work_kind_node_children_style_scan && token->flags & $md_token_flag_newline)
		{
			work_top->counted_newlines += 1;
			token += 1;
			goto end_consume;
		}
		
		//- rjf: [main_implicit] newline -> pop
		if(work_top->kind == $md_parse_work_kind_main_implicit && token->flags & $md_token_flag_newline)
		{
			md_parse_work_pop();
			token += 1;
			goto end_consume;
		}
		
		//- rjf: [all but main_implicit] newline -> no-op & inc
		if(work_top->kind != $md_parse_work_kind_main_implicit && token->flags & $md_token_flag_newline)
		{
			token += 1;
			goto end_consume;
		}
		
		//- rjf: [node children style scan] anything causing implicit set -> <2 newlines, all good,
		// >=2 newlines, houston we have a problem
		if(work_top->kind == $md_parse_work_kind_node_children_style_scan)
		{
			if(work_top->counted_newlines >= 2)
			{
				$md_node *node = work_top->parent;
				$md_node *error = md_push_node(arena, $md_node_kind_error_marker, 0, token_string, token_string, token->range.min);
				$string error_string = push_strf(arena, "More than two newlines following \"%S\", which has implicitly-delimited children, resulting in an empty list of children.", node->string);
				md_msg_list_push(arena, &msgs, error, $md_msg_kind_warning, error_string);
				md_parse_work_pop();
			}
			else
			{
				$md_node *parent = work_top->parent;
				md_parse_work_pop();
				md_parse_work_push($md_parse_work_kind_main_implicit, parent);
			}
			goto end_consume;
		}
		
		//- rjf: [main] }s, ]s, and )s -> pop
		if(work_top->kind == $md_parse_work_kind_main && token->flags & $md_token_flag_reserved &&
			(str_match(token_string, strlit("}"), 0) ||
			str_match(token_string, strlit("]"), 0) ||
			str_match(token_string, strlit(")"), 0)))
		{
			$md_node *parent = work_top->parent;
			parent->flags |=   $md_node_flag_has_brace_right*!!str_match(token_string, strlit("}"), 0);
			parent->flags |= $md_node_flag_has_bracket_right*!!str_match(token_string, strlit("]"), 0);
			parent->flags |=   $md_node_flag_has_paren_right*!!str_match(token_string, strlit(")"), 0);
			md_parse_work_pop();
			token += 1;
			goto end_consume;
		}
		
		//- rjf: [main implicit] }s, ]s, and )s -> pop without advancing
		if(work_top->kind == $md_parse_work_kind_main_implicit && token->flags & $md_token_flag_reserved &&
			(str_match(token_string, strlit("}"), 0) ||
			str_match(token_string, strlit("]"), 0) ||
			str_match(token_string, strlit(")"), 0)))
		{
			md_parse_work_pop();
			goto end_consume;
		}
		
		//- rjf: no consumption -> unexpected token! we don't know what to do with this.
		{
			$md_node *error = md_push_node(arena, $md_node_kind_error_marker, 0, token_string, token_string, token->range.min);
			$string error_string = push_strf(arena, "Unexpected \"%S\" token.", token_string);
			md_msg_list_push(arena, &msgs, error, $md_msg_kind_error, error_string);
			token += 1;
		}
		
		end_consume:;
	}
	
	//- rjf: fill & return
	$md_parse_result result = zero_struct;
	result.root = root;
	result.msgs = msgs;
	scratch_end(scratch);
	return result;
}

proc $md_parse_result
md_parse_from_text($arena *arena, $string filename, $string text) {
	$temp scratch = scratch_begin(&arena, 1);
	$md_tokenize_result tokenize = md_tokenize_from_text(scratch.arena, text);
	$md_parse_result parse = md_parse_from_tokenize_text(arena, filename, text, tokenize.tokens);
	scratch_end(scratch);
	return parse;
}

//--------------------------------------------------------------------------------
// tree -> text procs

proc $string_list
md_debug_string_list_from_tree($arena *arena, $md_node *root) {
	$string_list strings = {0};
	{
		char *indentation = "                                                                                                                                ";
		s32 depth = 0;
		for ($md_node *node = root, *next = md_nil_node; !md_node_is_nil(node); node = next) {
			// rjf: get next recursion
			$md_node_rec rec = md_node_rec_depth_first_pre(node, root);
			next = rec.next;
			
			// rjf: extract node info
			$string kind_string = strlit("Unknown");
			switch (node->kind) {
				case $md_node_kind_file:        {kind_string = strlit("File");       }break;
				case $md_node_kind_error_marker:{kind_string = strlit("ErrorMarker");}break;
				case $md_node_kind_main:        {kind_string = strlit("Main");       }break;
				case $md_node_kind_tag:         {kind_string = strlit("Tag");        }break;
				case $md_node_kind_list:        {kind_string = strlit("List");       }break;
				case $md_node_kind_reference:   {kind_string = strlit("Reference");  }break;

				case $md_node_kind_nil:   { invalid_path; }
				case $md_node_kind_count: { invalid_path; }
			}
			
			// rjf: push node line
			str_list_pushf(arena, &strings, "%.*s\"%S\" : %S", depth, indentation, node->string, kind_string);
			
			// rjf: children -> open brace
			if (rec.push_count != 0) {
				str_list_pushf(arena, &strings, "%.*s{", depth, indentation);
			}
			
			// rjf: descend
			depth += rec.push_count;
			
			// rjf: popping -> close braces
			for (s32 pop_idx = 0; pop_idx < rec.pop_count; pop_idx += 1) {
				str_list_pushf(arena, &strings, "%.*s}", depth-1-pop_idx, indentation);
			}
			
			// rjf: ascend
			depth -= rec.pop_count;
		}
	}
	return strings;
}

