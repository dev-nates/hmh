
typedef u64 Logger_Level;
enum Logger_Level {
	Log_Debug   = 0,
	Log_Info    = 10,
	Log_Warning = 20,
	Log_Error   = 30,
	Log_Fatal   = 40,
};

typedef u64 Logger_Options;
enum Logger_Option {
	Logger_Option_Level,
	Logger_Option_Date,
	Logger_Option_Time,
	Logger_Option_Filepath,
	Logger_Option_Line,
	Logger_Option_Procedure,
	Logger_Option_Terminal_Color,
	Logger_Option_Thread_Id,
};

typedef void Logger_Proc(rawptr data, Logger_Level level, string text, Logger_Options options, Source_Code_Location loc);

typedef struct Logger Logger;
struct Logger {
	Logger_Proc *procedure;
	rawptr data;
	Logger_Level lowest_level;
	Logger_Options options;
};

proc Logger
nil_logger(void);

proc void
logger_set_user_data(Logger *logger, rawptr data);

#define log_msg(level, str) _log_msg(code_location, level, str)
proc void
_log_msg(Source_Code_Location loc, Logger_Level level, string str);

#define msgfv(level, str, fmt, args) _log_msgfv(code_location, level, fmt, args)
proc void
_log_msgfv(Source_Code_Location loc, Logger_Level level, cstring fmt, va_list args);

#define log_msgf(level, fmt, ...) _log_msgf(code_location, level, fmt, __VA_ARGS__)
proc void
_log_msgf(Source_Code_Location loc, Logger_Level level, cstring fmt, ...);

#define debugf(fmt, ...) _debugf(code_location, fmt, __VA_ARGS__)
proc void
_debugf(Source_Code_Location loc, cstring fmt, ...);

#define infof(fmt, ...) _infof(code_location, fmt, __VA_ARGS__)
proc void
_infof(Source_Code_Location loc, cstring fmt, ...);

#define warnf(fmt, ...) _warnf(code_location, fmt, __VA_ARGS__)
proc void
_warnf(Source_Code_Location loc, cstring fmt, ...);

#define errorf(fmt, ...) _errorf(code_location, fmt, __VA_ARGS__)
proc void
_errorf(Source_Code_Location loc, cstring fmt, ...);

#define fatalf(fmt, ...) _fatalf(code_location, fmt, __VA_ARGS__)
proc void
_fatalf(Source_Code_Location loc, cstring fmt, ...);
