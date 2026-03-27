

#define LOG_DEBUG_COLOR   TERM_FOREGROUND_RGB(107, 107, 107)
#define LOG_INFO_COLOR    TERM_FOREGROUND_RGB(170, 196, 116)
#define LOG_WARNING_COLOR TERM_FOREGROUND_RGB(254, 202, 136)
#define LOG_ERROR_COLOR   TERM_FOREGROUND_RGB(197, 85, 85)
#define LOG_FATAL_COLOR   TERM_FOREGROUND_RGB(98, 97, 255)

typedef u64 Logger_Level;
enum Logger_Level {
	Log_Debug   = 0,
	Log_Info    = 10,
	Log_Warning = 20,
	Log_Error   = 30,
	Log_Fatal   = 40,
	Log_Level_End = 50,
};

typedef u64 Logger_Options;
enum Logger_Option {
	Logger_Option_Level          = bit1,
	Logger_Option_Date           = bit2,
	Logger_Option_Time           = bit3,
	Logger_Option_Filepath       = bit4,
	Logger_Option_Line           = bit5,
	Logger_Option_Procedure      = bit6,
	Logger_Option_Terminal_Color = bit7,
	Logger_Option_Thread_Id      = bit8,
};

typedef void Logger_Proc(rawptr data, Logger_Level level, string8 text, Logger_Options options, Source_Code_Location loc);

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
_log_msg(Source_Code_Location loc, Logger_Level level, string8 str);

#define msgfv(level, str, fmt, args) _log_msgfv(code_location, level, fmt, args)
proc void
_log_msgfv(Source_Code_Location loc, Logger_Level level, cstring fmt, va_list args);

#define log_msgf(level, ...) _log_msgf(code_location, level, __VA_ARGS__)
CLANG_FORMAT_WARN(3, 4)
proc void
_log_msgf(Source_Code_Location loc, Logger_Level level, cstring fmt, ...);

#define debugf(/*fmt*/ ...) _debugf(code_location, __VA_ARGS__)
CLANG_FORMAT_WARN(2, 3)
proc void
_debugf(Source_Code_Location loc, cstring fmt, ...);

#define infof(/*fmt*/ ...) _infof(code_location, __VA_ARGS__)
CLANG_FORMAT_WARN(2, 3)
proc void
_infof(Source_Code_Location loc, cstring fmt, ...);

#define warningf(/*fmt*/ ...) _warningf(code_location, __VA_ARGS__)
CLANG_FORMAT_WARN(2, 3)
proc void
_warningf(Source_Code_Location loc, cstring fmt, ...);

#define errorf(/*fmt*/ ...) _errorf(code_location,  __VA_ARGS__)
CLANG_FORMAT_WARN(2, 3)
proc void
_errorf(Source_Code_Location loc, cstring fmt, ...);

#define fatalf(/*fmt*/ ...) _fatalf(code_location, __VA_ARGS__)
CLANG_FORMAT_WARN(2, 3)
proc void
_fatalf(Source_Code_Location loc, cstring fmt, ...);
