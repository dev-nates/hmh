
global Logger_Options default_console_logger_options = 
	Logger_Option_Level |
	Logger_Option_Terminal_Color |
	Logger_Option_Filepath |
	Logger_Option_Line |
	Logger_Option_Procedure;

proc Logger
make_console_logger(Logger_Level lowest, Logger_Options options);

proc void
console_logger_proc(rawptr data, Logger_Level level, string8 text, Logger_Options options, Source_Code_Location loc);
