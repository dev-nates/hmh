
proc Logger
make_console_logger(Logger_Level lowest, Logger_Options options) {
	if (options == 0) {
		options = default_console_logger_options;
	}
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
_do_level(Logger_Options options, Logger_Level level) {
	if (!(Logger_Option_Level & options)) { return; }

	assert(Log_Debug <= level && level < Log_Level_End);
	Temp scratch = scratch_begin(0,0);

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
	fprintf(stdout, "%s[%s]%s --- ", terminal_color, level_str, TERM_STYLE_RESET);

	scratch_end(scratch);
}

proc void
_do_loc(Logger_Options options, Source_Code_Location loc) {
	if (Logger_Option_Filepath & options) {
		string8 filename = str_skip_last_slash(str_from_cstring(loc.filename));
		fprintf(stdout, "[%.*s :%ld:]", svarg(filename), loc.line);
	}

	if (Logger_Option_Procedure & options) {
		fprintf(stdout, "[%s]", loc.procedure);
	}
}

proc void
console_logger_proc(rawptr data, Logger_Level level, string8 text, Logger_Options options, Source_Code_Location loc) {
	_do_level(options ,level);
	_do_loc(options ,loc);
	fprintf(stdout, ": %.*s\n", svarg(text));
}
