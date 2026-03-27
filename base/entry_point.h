
typedef struct Cmdline_Arg Cmdline_Arg;
struct Cmdline_Arg {
	Cmdline_Arg *next;
	string8 name;
	string8 value;
};

typedef struct Cmdline Cmdline;
struct Cmdline {
	Cmdline_Arg *head, *tail;
	s64 count;
};

proc Cmdline base_cmdline_from_args(Arena *arena, s32 argc, cstring *args);
proc string8 value_from_cmdline_arg(Cmdline_Arg *arg, string8 optional_name, String_Match_Flags flags);

/*
	`base_main_thread_entry` is used for startup initialization for any layer in the codebase.
	It is either called in base_entry_point.c or in the os layer depending on if the os layer
	is included or not in the project.
*/
proc void base_main_thread_entry(s32 argc, cstring *args);

/*
	entry_point is user defined and will be called after startup initialization.
*/
proc void entry_point(Cmdline cl);

