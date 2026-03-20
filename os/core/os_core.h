

//--------------------------------------------------------------------------------
// Handles

typedef struct OS_File OS_File;
struct OS_File { u64 v[1]; };
typedef struct OS_Shared_Memory OS_Shared_Memory;
struct OS_Shared_Memory { u64 v[1]; };
typedef struct OS_Thread OS_Thread;
struct OS_Thread { u64 v[1]; };
typedef struct OS_Mutex OS_Mutex;
struct OS_Mutex { u64 v[1]; };
typedef struct OS_RW_Mutex OS_RW_Mutex;
struct OS_RW_Mutex { u64 v[1]; };
typedef struct OS_CV OS_CV;
struct OS_CV { u64 v[1]; };
typedef struct OS_Semaphore OS_Semaphore;
struct OS_Semaphore { u64 v[1]; };

//--------------------------------------------------------------------------------
// system info

typedef struct OS_System_Info OS_System_Info;
struct OS_System_Info
{
  s32 logical_processor_count;
  s32 page_size;
  s64 large_page_size;
  s64 allocation_granularity;
  string machine_name;
};

//--------------------------------------------------------------------------------
// process info

typedef struct OS_Process_Info OS_Process_Info;
struct OS_Process_Info
{
  u32 pid;
  b32 large_pages_allowed;
  string bin_path;
  string user_path;
  string current_path;
};

//--------------------------------------------------------------------------------
// access flags

typedef enum OS_File_Access_Flags OS_File_Access_Flags;
enum OS_File_Access_Flags {
	OS_File_Access_Flag_Read_Only = 0,
	OS_File_Access_Flag_Read_Write = bit1,

	OS_File_Access_Flag_Create = bit5,
	OS_File_Access_Flag_Append = bit6,
};

//--------------------------------------------------------------------------------
// file iterator flags & opaque data structure

typedef enum OS_File_Iter_Flags OS_File_Iter_Flags;
enum OS_File_Iter_Flags {
	OS_File_Iter_Flag_Include_Directories = 0x1,
	OS_File_Iter_Flag_Include_Hidden_Files = 0x2,
	OS_File_Iter_Flag_Skip_Files = 0x4,

	OS_File_Iter_Flag_Normal = 0,
	OS_File_Iter_Flag_Hidden = (OS_File_Iter_Flag_Include_Hidden_Files),
	OS_File_Iter_Flag_Only_Hidden = (OS_File_Iter_Flag_Include_Hidden_Files | OS_File_Iter_Flag_Skip_Files),
	OS_File_Iter_Flag_Directories = (OS_File_Iter_Flag_Include_Directories | OS_File_Iter_Flag_Skip_Files),
};

#define OS_FILE_ITERATOR_OPAQUE_SIZE 64
typedef struct OS_File_Iter OS_File_Iter;
struct OS_File_Iter {
	union {
		u64 aligned8_v[OS_FILE_ITERATOR_OPAQUE_SIZE/sizeof(u64)];
		u8 v[OS_FILE_ITERATOR_OPAQUE_SIZE];
	};
};

//--------------------------------------------------------------------------------
// thread proc

typedef void OS_Thread_Proc(rawptr ptr);

//--------------------------------------------------------------------------------
// abort

proc noreturn void
os_abort(s32 exit_code);

//--------------------------------------------------------------------------------
// info procs

proc OS_System_Info
os_get_system_info(void);

proc OS_Process_Info
os_get_process_info(void);

proc string
os_get_current_path(Arena *arena);

//--------------------------------------------------------------------------------
// file procs

proc OS_File
os_file_open(string path, OS_File_Access_Flags flags);

proc void
os_file_close(OS_File file);

proc s64
os_file_read(OS_File file, rng1s64 rng, rawptr out);

proc s64
os_file_write(OS_File file, rng1s64 rng, rawptr out);

proc File_Properties
os_file_get_properties(OS_File file);


proc b32
os_path_remove(string path);

proc b32
os_path_exists(string path);

proc b32
os_path_copy(string dst, string src);

proc array
os_data_from_path(Arena *arena, string path);

proc b32
os_write_data_to_path(string path, array data);


proc b32
os_make_directory(string path);

proc b32
os_directory_exists(string path);

//--------------------------------------------------------------------------------
// file iterator

proc b32
os_file_iter_begin(string path, OS_File_Iter_Flags flags, OS_File_Iter *it);

proc void
os_file_iter_end(OS_File_Iter *it);

proc b32
os_file_iter_next(Arena *arena, OS_File_Iter *it, string *path, File_Properties *props);

//--------------------------------------------------------------------------------
// time procs

proc u64
os_time_us(void);

proc u64
os_time_ms(void);

proc void
os_sleep_ms(u64 ms);

//--------------------------------------------------------------------------------
// shared memory

proc OS_Shared_Memory
os_shared_memory_alloc(string string, s64 size, OS_File_Access_Flags flags);

proc OS_Shared_Memory
os_shared_memory_open(string string, OS_File_Access_Flags flags);

proc void
os_shared_memory_close(OS_Shared_Memory shared_memory);

proc rawptr
os_shared_memory_view_open(OS_Shared_Memory shared_memory, rng1s64 range);

proc void
os_shared_memory_view_close(OS_Shared_Memory shared_memory, rawptr ptr, rng1s64 range);


//--------------------------------------------------------------------------------
// threads

proc OS_Thread
os_thread_launch(OS_Thread_Proc *procedure, rawptr params, string name);

proc b32
os_thread_join(OS_Thread handle);

proc void
os_thread_detach(OS_Thread handle);

//--------------------------------------------------------------------------------b
// mutexes

proc OS_Mutex
os_mutex_alloc(void);

proc void
os_mutex_release(OS_Mutex mutex);

proc void
os_mutex_take(OS_Mutex mutex);

proc void
os_mutux_drop(OS_Mutex mutex);

//--------------------------------------------------------------------------------
// rw mutexes

proc OS_RW_Mutex
os_rw_mutex_alloc(void);

proc void
os_rw_mutex_release(OS_RW_Mutex mutex);

proc void
os_mutex_take_r(OS_RW_Mutex mutex);

proc void
os_mutex_drop_r(OS_RW_Mutex mutex);

proc void
os_mutex_take_w(OS_RW_Mutex mutex);

proc void
os_mutex_drop_w(OS_RW_Mutex mutex);

//--------------------------------------------------------------------------------
// conditional variables

proc OS_CV
os_condition_variable_alloc(void);

proc void
os_condition_variable_release(OS_CV cv);

proc b32
os_condition_variable_wait(OS_CV cv, OS_Mutex mutex, u64 endt_us);

proc void
os_condition_variable_signal(OS_CV cv);

proc void
os_condition_variable_broadcast(OS_CV cv);

//--------------------------------------------------------------------------------
// semaphores

proc OS_Semaphore
os_semaphore_alloc(s32 initial_count, s32 max_count, string name);

proc void
os_semaphore_release(OS_Semaphore semaphore);

proc b32
os_semaphore_try(OS_Semaphore semaphore);

proc b32
os_semaphore_wait(OS_Semaphore semaphore, u64 endt_us);

proc b32
os_semaphore_post(OS_Semaphore semaphore);

