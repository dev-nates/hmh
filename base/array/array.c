
proc array8
make_array(u8 *m, s64 size) {
	return (array8) { m, size };
}

proc void
array_list_push_node(array_list *list, array_node *node) {
	array_node **ptr = check_nil(list->head, nil) ? &list->head : &list->tail->next;
	*ptr = node; list->tail = node;
	node->next = nil;
	list->count += 1;
	list->total_size += node->arr.size;
}

proc void
array_list_push(Arena *arena, array_list *list, array8 arr) {
	array_node *node = push_array(arena, array_node, 1);
	node->arr = arr;
	array_list_push_node(list, node);
}

proc meta_array
reserve_meta_array(Arena *arena, s64 count) {
	array8 *v = push_array(arena, array8, count);
	meta_array result = {
		.v = v,
		.count = count,
	};
	return result;
}

proc meta_array
meta_array_from_list(Arena *arena, array_list list) {
	meta_array arr = reserve_meta_array(arena, list.count);
	s64 idx = 0;
	for (each_node(node, list.head, array_node)) {
		assert(idx < arr.count);
		arr.v[idx] = node->arr;
		idx += 1;
	}
	return arr;
}

//--------------------------------------------------------------------------------
// Array 16
proc array16
make_array16(u16 *m, s64 size) {
	return (array16) { m, size };
}

//--------------------------------------------------------------------------------
// Array 32
proc array32
make_array32(u32 *m, s64 size) {
	return (array32) { m, size };
}

//--------------------------------------------------------------------------------
// Array 64
proc array64
make_array64(u64 *m, s64 size) {
	return (array64) { m, size };
}
