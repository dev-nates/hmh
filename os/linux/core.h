
typedef struct Linux_File Linux_File;
struct Linux_File { s64 v[1]; };
proc Linux_File linux_file_zero(void) { return cast(Linux_File)zero_struct; }
proc b8 linux_file_match(Linux_File a, Linux_File b) { return cast(b8)(a.v[0] == b.v[0]); }

typedef struct Linux_Shared_Memory Linux_Shared_Memory;
struct Linux_Shared_Memory {
	s64 v[1];
};
proc Linux_Shared_Memory linux_shared_memory_zero(void) { return cast(Linux_Shared_Memory)zero_struct; }
proc b8 linux_shared_memory_match(Linux_Shared_Memory a, Linux_Shared_Memory b) { return cast(b8)(a.v[0] == b.v[0]); };

// -------------------------------------------------------------------------------------------------
// System info
typedef struct Linux_System_Info Linux_System_Info;
struct Linux_System_Info {
	s64 logical_processor_count;
	s64 page_size;
	s64 large_page_size;
	string8 machine_name;
};

// -------------------------------------------------------------------------------------------------
// Process info

typedef struct Linux_Process_Info Linux_Process_Info;
struct Linux_Process_Info {
	u32 pid;
	string8 bin_path;
	string8 user_path;
	string8 current_path;
};

// -------------------------------------------------------------------------------------------------
// File iterator flags

typedef s64 Linux_File_Iter_Flags;
enum Linux_File_Iter_Flags {
	Linux_File_Iter_Include_Directories = bit1,
	Linux_File_Iter_Include_Hidden_Files = bit2,
	Linux_File_Iter_Skip_Normal_Files = bit3,

	Linux_File_Iter_Only_Normal_Files = 0,
	Linux_File_Iter_Only_Hidden_Files = (Linux_File_Iter_Skip_Normal_Files),
	Linux_File_Iter_Only_Directories = (Linux_File_Iter_Include_Directories | Linux_File_Iter_Skip_Normal_Files),
};


typedef struct Linux_File_Iter Linux_File_Iter;
struct Linux_File_Iter {
	Linux_File_Iter_Flags flags;
	string8 path;
	DIR *dir;
};

// -------------------------------------------------------------------------------------------------
// Thread

typedef void Linux_Thread_Proc(rawptr data);

typedef struct Linux_Thread Linux_Thread;
struct Linux_Thread {
	pthread_t handle;
	Linux_Thread_Proc *procedure;
	rawptr data;
	string8 name;
};

// -------------------------------------------------------------------------------------------------
// State

typedef struct Linux_State Linux_State;
struct Linux_State {
	Arena *arena;
	Linux_System_Info system_info;
	Linux_Process_Info process_info;
};
global Linux_State *linux_state = nil;

// -------------------------------------------------------------------------------------------------
// Metadata

proc Linux_System_Info
linux_system_info(void);

proc Linux_Process_Info
linux_process_info(void);

proc string8
linux_current_path(Arena *arena);

// -------------------------------------------------------------------------------------------------
// File procs

// @Note: access_mode && flags are passed to `open`
proc Linux_File
linux_file_open(string8 path, int flags, int mode);

proc void
linux_file_close(Linux_File file);

proc s64 /*amt read*/
linux_file_read(Linux_File file, rng1s64 rng, rawptr out);

proc s64 /*amt written*/
linux_file_write(Linux_File file, rng1s64 rng, rawptr out);

proc File_Properties
linux_file_properties(Linux_File file);

// -------------------------------------------------------------------------------------------------
// Path

proc b8
linux_path_remove(string8 path);

proc b8
linux_path_exists(string8 path);

proc b8
linux_path_copy(string8 dst, string8 src);

proc array8
linux_data_from_path(Arena *arena, string8 path);

proc b8
linux_write_data_to_path(string8 path, array8 data);

// -------------------------------------------------------------------------------------------------
// Directory

proc b8
linux_make_directory(string8 path, int mode);

proc b8
linux_directory_exists(string8 path);

// -------------------------------------------------------------------------------------------------
// File Iterator

proc b8
linux_file_iter_begin(string8 path, Linux_File_Iter_Flags flags, Linux_File_Iter *it);

proc void
linux_file_iter_end(Linux_File_Iter *it);

proc b8
linux_file_iter_next(Arena *arena, Linux_File_Iter *it, _ret_ string8 *path, File_Properties *props);

// -------------------------------------------------------------------------------------------------
// Threads

proc void
linux_thread_launch(Linux_Thread *thread, Linux_Thread_Proc *procedure, rawptr data, string8 name);

proc b8
linux_thread_join(Linux_Thread thread);

// -------------------------------------------------------------------------------------------------
// Shared memory

// @Note: mode && flags are passed to `shm_open`
// @Note: Name specifies a path from root e.g. "/shared_mem_name" that two processes
//        use to share memory between each other
proc Linux_Shared_Memory
linux_shm_alloc(string8 str, s64 size, int flags, int mode);

proc Linux_Shared_Memory
linux_shm_open(string8 str, int flags, int mode);

proc void
linux_shm_close(Linux_Shared_Memory shm);

proc void
linux_shm_unlink(string8 name);

proc rawptr /*data*/
linux_shm_view_open(Linux_Shared_Memory shm, rng1s64 range);

proc void
linux_shm_view_close(Linux_Shared_Memory shm, rawptr ptr, rng1s64 range);

