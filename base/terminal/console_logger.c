
proc Logger
make_console_logger(Logger_Level lowest, Logger_Options options) {
	Logger logger = {
		.procedure = console_logger_proc,
		.options = options,
	};
	return logger;
}

/*
 *             @Todo
 * [INFO ] --- [time] [file] [procedure]: TEXT
 */

proc void
_do_level(Arena *arena, string_list *list, Logger_Options options, Logger_Level level) {
	if (!(Logger_Option_Level & options)) { return; }

	assert(Log_Debug <= level && level < Log_Level_End);
	cstring level_str = nil;
	cstring terminal_color = nil;
	if (Log_Debug <= level && level < Log_Info) {
		level_str = "DEBUG";
		terminal_color = LOG_DEBUG_COLOR;
	} else if (Log_Info <= level && level < Log_Warning) {
		level_str = "INFO ";
		terminal_color = LOG_INFO_COLOR;
	} else if (Log_Warning <= level && level < Log_Error) {
		level_str = "WARN ";
		terminal_color = LOG_WARNING_COLOR;
	} else if (Log_Error <= level && level < Log_Fatal) {
		level_str = "ERROR";
		terminal_color = LOG_ERROR_COLOR;
	} else if (Log_Fatal <= level && level < Log_Level_End) {
		level_str = "FATAL";
		terminal_color = LOG_FATAL_COLOR;
	}
	str_list_pushf(arena, list, "%s[%s]%s --- ", terminal_color, level_str, TERM_STYLE_RESET);
}

proc void
_do_loc(Arena *arena, string_list *list, Logger_Options options, Code_Location loc) {
	if (Logger_Option_Filepath & options) {
		string8 filename = str_skip_last_slash(str_from_cstring(loc.filename));
		str_list_pushf(arena, list, "[%.*s:%ld]", svarg(filename), loc.line);
	}

	if (Logger_Option_Procedure & options) {
		str_list_pushf(arena, list, "[%s]", loc.procedure);
	}
}

proc void
console_logger_proc(rawptr data, Logger_Level level, string8 text, Logger_Options options, Code_Location loc) {
	Temp scr = scratch_begin(0, 0);

	string_list list = zero_struct;
	_do_level(scr.arena, &list, options ,level);
	_do_loc(scr.arena, &list, options ,loc);
	
	String_Join join = zero_struct;
	string8 joined = str_list_join(scr.arena, &list, &join);
	if (joined.size > 0) {
		fprintf(stdout, "%.*s: %.*s\n", svarg(joined), svarg(text));
	} else {
		fprintf(stdout, "%.*s\n", svarg(text));
	}

	scratch_end(scr);
}
