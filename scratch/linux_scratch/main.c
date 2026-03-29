
#include "base/inc.h"
#include "os/linux/inc.h"

#include "base/inc.c"
#include "os/linux/inc.c"

typedef struct Job Job;
struct Job {
	s64 *numbers;
	s64 count;
	s64 sum;
};

global Mutex mutex;
volatile global s64 interleaved_index = 0;

proc void
thread_proc(rawptr data) {
	string8 name = tctx_get_name();

	Job *job = cast(Job*)(data);
	assert(job->count > 0);
	for (s64 i = 0; i < job->count; i += 1) {
		job->sum += job->numbers[i];
	}

	for (s64 i = 0; i < 2; i += 1) {
		mutex_take(mutex);
		interleaved_index += 1;
		infof(TC(20,200,80) "[%.*s] INDEX WRITE: " TC_END "%ld", svarg(name), interleaved_index);
		mutex_drop(mutex);
	}
	infof(TC(20,200,80) "[%.*s] complete" TC_END, svarg(name));
}

proc void
entry_point(Cmdline cl) {
	Arena *arena = arena_alloc();

	// -------------------------------------------------------------------------------------------------
	// System info
	Linux_System_Info sinfo = linux_system_info();
	infof("System info:");
	infof("\tLogical Processors: %ld", sinfo.logical_processor_count);
	infof("\tPage Size: %ld", sinfo.page_size);
	infof("\tLarge Page Size: %ld", sinfo.large_page_size);
	infof("\tMachine name: %.*s", svarg(sinfo.machine_name));

	// -------------------------------------------------------------------------------------------------
	// Process info
	Linux_Process_Info pinfo = linux_process_info();
	infof("Process info:");
	infof("\tbin_path: " TC(0,0,255) "%.*s" TC_END, svarg(pinfo.bin_path));
	infof("\tuser_path: " TC(0,0,255) "%.*s" TC_END, svarg(pinfo.user_path));
	infof("\tcurrent_path: " TC(0,0,255) "%.*s" TC_END, svarg(pinfo.current_path));

	// -------------------------------------------------------------------------------------------------
	// Simple test file
	string8 path = S("os");
	Linux_File file = linux_file_open(path, O_RDONLY, 0644);
	File_Properties props = linux_file_properties(file);
	infof("File:");
	infof("\tCreated: " TC(255,0,0) "%lu" TC_END " -- Modified: " TC(255,0,0) "%lu" TC_END, props.created, props.modified);
	infof("\tSize: " TC(0,255,0) "%ld" TC_END " Is Folder: " TC(255,255,0) "%s" TC_END, props.size, (props.flags & File_Property_Flag_Is_Folder) ? "true" : "false");
	linux_file_close(file);

	// -------------------------------------------------------------------------------------------------
	// Test directory procedures

	/*
	path = S("test_dir");
	b8 made_directory = linux_make_directory(path, 0755);
	b8 directory_exists = linux_directory_exists(S("os"));
	infof("made_directory: " TC(255,255,255) "%s" TC_END, (made_directory) ? "true" : "false");
	infof("directory_exists: " TC(255,255,255) "%s" TC_END, (directory_exists) ? "true" : "false");
	*/

	// -------------------------------------------------------------------------------------------------
	// File iter

	string8 current_path = pinfo.current_path;
	Linux_File_Iter it = zero_struct;
	linux_file_iter_begin(current_path, Linux_File_Iter_Only_Normal_Files, &it);

	for (;;) {
		Temp scr = scratch_begin(0, 0);

		string8 path = zero_struct;
		File_Properties props = zero_struct;
		if (!linux_file_iter_next(scr.arena, &it, _ret_ &path, &props)) { break; }

		infof(TC(255,255,0) "%s:" TC_END, path.m);
		infof("\tCreated: " TC(255,0,0) "%lu" TC_END " -- Modified: " TC(255,0,0) "%lu" TC_END, props.created, props.modified);
		infof("\tSize: " TC(0,255,0) "%ld" TC_END " Is Folder: " TC(255,255,0) "%s" TC_END, props.size, (props.flags & File_Property_Flag_Is_Folder) ? "true" : "false");

		scratch_end(scr);
	}

	// -------------------------------------------------------------------------------------------------
	// Shared memory
	
#if 0
	string8 name = S("/shared-memory-for-testing");
	Linux_Shared_Memory memory = linux_shm_alloc(name, megabytes(1), O_CREAT|O_RDWR, 0644);
	typedef struct Foo Foo;
	struct Foo {
		b8 exit;
	};

	Foo *foo = cast(Foo*)linux_shm_view_open(memory, r1s64(0, size_of(Foo)));
	memory_zero_struct(foo);
	for (;;) {
		if (foo->exit == true) { break; }
	}

	linux_shm_view_close(memory, cast(rawptr)(foo), r1s64(0, size_of(Foo)));
	linux_shm_unlink(name);
	linux_shm_close(memory);
#endif

	// -------------------------------------------------------------------------------------------------
	// Thread 

	s64 count = 1024;
	s64 *numbers = push_array(arena, s64, count);
	for (each_index(i, count)) {
		numbers[i] = i;
	}

	Linux_Thread thread_a = zero_struct;
	Linux_Thread thread_b = zero_struct;

	Job job_a = zero_struct;
	job_a.numbers = numbers;
	job_a.count = count/2;

	Job job_b = zero_struct;
	job_b.numbers = numbers + count/2;
	job_b.count = count/2;

	mutex = mutex_alloc(arena);
	linux_thread_launch(&thread_a, thread_proc, &job_a, S("Thread A"));
	linux_thread_launch(&thread_b, thread_proc, &job_b, S("Thread B"));

	linux_thread_join(thread_a);
	linux_thread_join(thread_b);

	s64 sum = job_a.sum + job_b.sum;
	infof(TC(255,255,255) "Job A: %ld -- Job B: %ld" TC_END, job_a.sum, job_b.sum);
	infof(TC(255,255,255) "Sum: %ld" TC_END, sum);
}
