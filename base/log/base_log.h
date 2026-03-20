
typedef struct Log_Scope Log_Scope;
struct Log_Scope {
	Log_Scope *next;
	string_list list;
	s64 restore_pos;
};

typedef struct Log Log;
struct Log {
	Arena *arena;
	Log_Scope *scope;
};

proc Log*
log_alloc(void);

proc void
log_release(Log *log);

proc void
log_msg(Log *log, string str);

proc void
log_msgf(Log *log, cstring fmt, ...);


proc void
log_scope_begin(Log *log);

proc string_list
log_scope_end(Arena *arena, Log *log);

