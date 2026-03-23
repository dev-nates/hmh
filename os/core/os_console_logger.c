
proc Logger
os_make_console_logger(OS_File file, Logger_Level lowest, Logger_Options options) {
	Arena *arena = arena_alloc();
	OS_Console_Logger_Data *data = push_struct(arena, OS_Console_Logger_Data);
	data->arena = arena;
	data->file = file;

	Logger logger = {
		.procedure = os_console_logger_proc,
		.data = cast(rawptr)(data),
	};
	return logger;
}

proc void
os_destroy_console_logger(Logger *logger) {
	#error "Continue here"
}

proc void
os_console_logger_proc(rawptr data, Logger_Level level, string text, Logger_Options options, Source_Code_Location loc);
