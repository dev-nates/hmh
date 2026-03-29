

proc void
nil_logger_proc(rawptr data, Logger_Level level, string8 text, Logger_Options options, Code_Location loc) {
	// Do nothing
}

proc Logger
nil_logger(void) {
	return (Logger){.procedure = nil_logger_proc};
}

proc void
logger_set_user_data(Logger *logger, rawptr data) {
	logger->data = data;
}

proc void
_log_msg(Code_Location loc, Logger_Level level, string8 str) {
	Thread_Context *ctx = tctx_get_context();
	Logger *logger = ctx->logger;
	logger->procedure(logger->data, level, str, logger->options, loc);
}

proc void
_log_msgfv(Code_Location loc, Logger_Level level, cstring fmt, va_list args) {
	Temp scratch = scratch_begin(0,0);
	va_list args_copy;
	va_copy(args_copy, args);
	_log_msg(loc, level, push_strfv(scratch.arena, fmt, args_copy));
	va_end(args_copy);
	scratch_end(scratch);
}

proc void
_log_msgf(Code_Location loc, Logger_Level level, cstring fmt, ...) {
	Temp scratch = scratch_begin(0,0);
	va_list args;
	va_start(args, fmt);
	_log_msg(loc, level, push_strfv(scratch.arena, fmt, args));
	va_end(args);
	scratch_end(scratch);
}

proc void
_debugf(Code_Location loc, cstring fmt, ...) {
	Temp scratch = scratch_begin(0,0);
	va_list args;
	va_start(args, fmt);
	_log_msg(loc, Log_Debug, push_strfv(scratch.arena, fmt, args));
	va_end(args);
	scratch_end(scratch);
}

proc void
_infof(Code_Location loc, cstring fmt, ...) {
	Temp scratch = scratch_begin(0,0);
	va_list args;
	va_start(args, fmt);
	_log_msg(loc, Log_Info, push_strfv(scratch.arena, fmt, args));
	va_end(args);
	scratch_end(scratch);
}

proc void
_warningf(Code_Location loc, cstring fmt, ...) {
	Temp scratch = scratch_begin(0,0);
	va_list args;
	va_start(args, fmt);
	_log_msg(loc, Log_Warning, push_strfv(scratch.arena, fmt, args));
	va_end(args);
	scratch_end(scratch);
}

proc void
_errorf(Code_Location loc, cstring fmt, ...) {
	Temp scratch = scratch_begin(0,0);
	va_list args;
	va_start(args, fmt);
	_log_msg(loc, Log_Error, push_strfv(scratch.arena, fmt, args));
	va_end(args);
	scratch_end(scratch);
}

proc void
_fatalf(Code_Location loc, cstring fmt, ...) {
	Temp scratch = scratch_begin(0,0);
	va_list args;
	va_start(args, fmt);
	_log_msg(loc, Log_Fatal, push_strfv(scratch.arena, fmt, args));
	va_end(args);
	scratch_end(scratch);
}
