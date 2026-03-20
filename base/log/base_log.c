
proc Log *
log_alloc(void) {
	Arena *arena = arena_alloc();
	Log *log = push_array(arena, Log, 1);
	log->arena = arena;
	return log;
}

proc void
log_release(Log *log) {
	if (log) {
		arena_release(log->arena);
	}
}

proc void
log_msg(Log *log, string str) {
	if (log) {
		Log_Scope *scope = log->scope;
		if (scope) {
			str_list_push(log->arena, &scope->list, str);
		}
	}
}

proc void
log_msgf(Log *log, cstring fmt, ...) {
	va_list args;
	va_start(args, fmt);
	string str = push_strfv(log->arena, fmt, args);
	va_end(args);
	log_msg(log, str);
}

proc void
log_scope_begin(Log *log) {
	if (log) {
		s64 restore_pos = arena_pos(log->arena);
		Log_Scope *scope = push_array_no_zero(log->arena, Log_Scope, 1);
		scope->restore_pos = restore_pos;
		sll_stack_push(log->scope, scope);
	}
}

proc string_list
log_scope_end(Arena *arena, Log *log) {
	string_list result = zero_struct;
	if (log) {
		Log_Scope *scope = log->scope;
		if (scope) {
			result = str_list_copy(arena, &scope->list);
			sll_stack_pop(log->scope);
			arena_pop_to(log->arena, scope->restore_pos);
		}
	}
	return result;
}
