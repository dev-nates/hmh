
global Logger_Options os_default_console_logger_options = 
	Logger_Option_Level |
	Logger_Option_Terminal_Color |
	Logger_Option_Filepath |
	Logger_Option_Line |
	Logger_Option_Procedure;

typedef struct OS_Console_Logger_Data OS_Console_Logger_Data;
struct OS_Console_Logger_Data {
	Arena *arena;
	OS_File file;
};

proc Logger
os_make_console_logger(OS_File file, Logger_Level lowest, Logger_Options options);

proc void
os_destroy_console_logger(Logger *logger);

proc void
os_console_logger_proc(rawptr data, Logger_Level level, string text, Logger_Options options, Source_Code_Location loc);
