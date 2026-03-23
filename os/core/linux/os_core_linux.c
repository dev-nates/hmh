
//--------------------------------------------------------------------------------
// entities

proc Linux_Entity*
os_lnx_entity_alloc(Linux_Entity_Kind kind) {
	Linux_Entity *entity = 0;
	defer_loop(pthread_mutex_lock(&linux_state.entity_mutex), pthread_mutex_unlock(&linux_state.entity_mutex)) {
		entity = linux_state.entity_freelist;
		if (!check_nil(entity, nil)) {
			asan_cure(entity, size_of(*entity));
			linux_state.entity_freelist = linux_state.entity_freelist->next;
		} else {
			entity = push_struct_no_zero(linux_state.arena, Linux_Entity);
		}
		memory_zero_struct(entity);
	}
	entity->kind = kind;
	return entity;
}

proc void
os_lnx_entity_release(Linux_Entity *entity) {
	defer_loop(
		pthread_mutex_lock(&linux_state.entity_mutex),
		pthread_mutex_unlock(&linux_state.entity_mutex))
	{
		entity->next = linux_state.entity_freelist; linux_state.entity_freelist = entity;
		asan_poison(entity, size_of(*entity));
	}
}

//--------------------------------------------------------------------------------_
// linux helpers

proc Date_Time
os_lnx_date_time_from_tm(struct tm in, u32 msec)
{
	Date_Time dt = {0};
	dt.sec  = (u16)in.tm_sec;
	dt.min  = (u16)in.tm_min;
	dt.hour = (u16)in.tm_hour;
	dt.day  = (u16)(in.tm_mday-1);
	dt.mon  = (u32)in.tm_mon;
	dt.year = (u32)(in.tm_year+1900);
	dt.msec = (u16)msec;
	return dt;
}

proc struct tm
os_lnx_tm_from_date_time(Date_Time dt)
{
	struct tm result = {0};
	result.tm_sec = (int)dt.sec;
	result.tm_min = (int)dt.min;
	result.tm_hour= (int)dt.hour;
	result.tm_mday= (int)dt.day+1;
	result.tm_mon = (int)dt.mon;
	result.tm_year= (int)dt.year-1900;
	return result;
}

proc struct timespec
os_lnx_timespec_from_date_time(Date_Time dt)
{
	struct tm tm_val = os_lnx_tm_from_date_time(dt);
	time_t seconds = timegm(&tm_val);
	struct timespec result = {0};
	result.tv_sec = seconds;
	return result;
}

proc Dense_Time
os_lnx_dense_time_from_timespec(struct timespec in)
{
	Dense_Time result = 0;
	{
		struct tm tm_time = {0};
		gmtime_r(&in.tv_sec, &tm_time);
		Date_Time date_time = os_lnx_date_time_from_tm(tm_time, (u32)(in.tv_nsec/million(1)));
		result = dense_time_from_date_time(date_time);
	}
return result;
}

proc File_Properties
os_lnx_file_properties_from_stat(struct stat *s)
{
	File_Properties props = {0};
	props.size     = s->st_size;
	props.created  = os_lnx_dense_time_from_timespec(s->st_ctim);
	props.modified = os_lnx_dense_time_from_timespec(s->st_mtim);
	int type = s->st_mode & S_IFMT;
	if(type == S_IFDIR)
	{
	  props.flags |= File_Property_Flag_Is_Folder;
	}
	return props;
}

//--------------------------------------------------------------------------------
// abort

proc noreturn void
os_abort(s32 exit_code) {
	_exit(exit_code);
}


//--------------------------------------------------------------------------------
// info procs

proc OS_System_Info
os_get_system_info(void) {
	return linux_state.system_info;
}

proc OS_Process_Info
os_get_process_info(void) {
	return linux_state.process_info;
}

proc string
os_get_current_path(Arena *arena) {
	cstring wd = getcwd(0, 0);
	string result = push_str_copy(arena, str_cstring(wd));
	free(wd);
	return result;
}

//--------------------------------------------------------------------------------
// file procs

proc OS_File
os_file_open(string path, OS_File_Access_Flags flags) {
	Temp scratch = scratch_begin(0, 0);
	cstring cstr = push_str_copy(scratch.arena, path).m;

	int lnx_flags = O_RDONLY;
	if (flags & OS_File_Access_Flag_Read_Write) {
		lnx_flags = O_RDWR;
	}
	if (flags & OS_File_Access_Flag_Create) {
		lnx_flags |= O_CREAT;
	}
	if (flags & OS_File_Access_Flag_Append) {
		lnx_flags |= O_APPEND;
	}

	int fd = open(cstr, lnx_flags, 0755);
	scratch_end(scratch);
	OS_File handle = zero_handle(OS_File);
	if (fd >= 0) {
		handle.v[0] = (u64)fd;
	}
	return handle;
}


proc void
os_file_close(OS_File file) {
	assert(!handle_match(file, zero_handle(OS_File)));

	int fd = (int)file.v[0];
	assert(fd >= 0);
	close(fd);
}

proc s64
os_file_read(OS_File file, rng1s64 rng, rawptr out) {
	assert(!handle_match(file, zero_handle(OS_File)));
	assert(0 <= rng.min && rng.min <= rng.max);

	s64 off = rng.min;
	int fd = (int)file.v[0];
	assert(fd >= 0);
	#if DEBUG
	off_t offset = lseek(fd, off, SEEK_SET);
	assert(offset == off);
	#else
	lseek(fd, off, SEEK_SET);
	#endif

	s64 size = dim_1s64(rng);
	ssize_t amt = read(fd, out, (u64)size);
	s64 result = clamp_bot(0, amt);
	return result;
}

proc s64
os_file_write(OS_File file, rng1s64 rng, rawptr out) {
	assert(!handle_match(file, zero_handle(OS_File)));
	assert(0 <= rng.min && rng.min <= rng.max);

	int fd = (int)file.v[0];
	assert(fd >= 0);

	s64 written = 0;
	s64 left = dim_1s64(rng);
	for (;left > 0;) {
		#if DEBUG
		s64 seek_off = lseek(fd, rng.min + written, SEEK_SET);
		assert(seek_off == rng.min + written);
		#else
		lseek(fd, rng.min + written, SEEK_SET);
		#endif

		ssize_t amt = write(fd, out + written, (u64)left);
		if (amt >= 0) {
			written += amt;
			left -= amt;
		}
		else if (errno != EINTR) {
			break;
		}
	}
	return written;
}

proc File_Properties
os_file_get_properties(OS_File file) {
	assert(!handle_match(file, zero_handle(OS_File)));
	int fd = (int)file.v[0];
	assert(fd >= 0);

	struct stat stat;
	fstat(fd, &stat);

	File_Properties result = os_lnx_file_properties_from_stat(&stat);
	return result;
}

proc b32
os_path_remove(string path) {
	Temp scratch = scratch_begin(0, 0);
	cstring cstr = push_str_copy(scratch.arena, path).m;
	b32 success = (remove(cstr) != -1);
	scratch_end(scratch);
	return success;
}

proc b32
os_path_exists(string path) {
	Temp scratch = scratch_begin(0, 0);
	cstring cstr = push_str_copy(scratch.arena, path).m;
	int success = access(cstr, F_OK);
	b32 result = (success != -1);
	scratch_end(scratch);
	return result;
}

proc b32
os_path_copy(string dst, string src) {
	b32 result = 0;
	OS_File dst_handle = os_file_open(dst, (OS_File_Access_Flags)(OS_File_Access_Flag_Read_Write | OS_File_Access_Flag_Create));
	OS_File src_handle = os_file_open(src, (OS_File_Access_Flags)(OS_File_Access_Flag_Read_Only));
	if (!handle_match(dst_handle, zero_handle(OS_File)) && !handle_match(src_handle, zero_handle(OS_File))) {
		int dstfd = (int)dst_handle.v[0];
		int srcfd = (int)src_handle.v[0];
		assert(dstfd != srcfd);
		File_Properties src_props = os_file_get_properties(src_handle);

		s64 sent = 0;
		s64 left = src_props.size;

		for (;left > 0;) {
			off_t off = (off_t)sent;
			ssize_t this_sent = sendfile(dstfd, srcfd, &off, (size_t)left);
			if (this_sent <= 0) {
				break;
			}

			sent += this_sent;
			left -= this_sent;
		}

		assert(left >= 0);
		result = (left == 0);
	}

	os_file_close(dst_handle);
	os_file_close(src_handle);

	return result;
}

proc b32
os_make_directory(string path) {
	b32 result = 0;
	Temp scratch = scratch_begin(0, 0);
	cstring cstr = push_str_copy(scratch.arena, path).m;
	int success = mkdir(cstr, 0755);
	result = (success != -1);
	scratch_end(scratch);
	return result;
}

proc b32
os_directory_exists(string path) {
	Temp scratch = scratch_begin(0, 0);
	cstring cstr = push_str_copy(scratch.arena, path).m;
	b32 exists = 0;
	DIR *h = opendir(cstr);
	if (h) {
		closedir(h);
		exists = 1;
	}
	scratch_end(scratch);
	return exists;
}

//--------------------------------------------------------------------------------
// file iterator

proc b32
os_file_iter_begin(string path, OS_File_Iter_Flags flags, OS_File_Iter *it) {
	Temp scratch = scratch_begin(0, 0);
	Linux_File_Iter *lnx_it = (Linux_File_Iter*)it;

	cstring name = push_str_copy(scratch.arena, path).m;
	lnx_it->dir = opendir(name);
	lnx_it->path = path;
	lnx_it->flags = flags;
	scratch_end(scratch);

	b32 found_directory = (lnx_it->dir != 0);
	return found_directory;
}

proc void
os_file_iter_end(OS_File_Iter *it) {
	Linux_File_Iter *lnx_it = (Linux_File_Iter*)it;
	closedir(lnx_it->dir);
}

proc b32
os_file_iter_next(Arena *arena, OS_File_Iter *it, string *path, File_Properties *props) {
	Temp scratch = scratch_begin(&arena, 1);
	Linux_File_Iter *lnx_it = (Linux_File_Iter*)it;
	if (lnx_it->dir == 0) {
		*path = str_zero();
		*props = (File_Properties)zero_struct;
		return 0;
	}

	for (;;) {
		b32 good_entry = 0;
		struct dirent *entry = readdir(lnx_it->dir);
		if (entry == 0 || entry->d_name[0] == 0) {
			*path = str_zero();
			*props = (File_Properties)zero_struct;
			return 0;
		}

		// skip . && .. entries
		if ((entry->d_name[0] == '.' && entry->d_name[1] == 0) ||
			(entry->d_name[0] == '.' && entry->d_name[1] == '.' && entry->d_name[2] == 0)) {
			continue;
		}

		rawptr cap = (entry->d_name + size_of(entry->d_name));
		string at_name = str_cstring_scan_to_cap(entry->d_name, cap);
		string_list list = zero_struct;
		str_list_push(scratch.arena, &list, lnx_it->path);
		str_list_push(scratch.arena, &list, at_name);
		string full = str_list_join(scratch.arena, &list, &(String_Join){.sep = S("/")});
		struct stat st;
		stat(full.m, &st);

		int type = st.st_mode & S_IFMT;
		if (type == S_IFREG && !has_flags(lnx_it->flags, OS_File_Iter_Flag_Skip_Files)) {
			good_entry = 1;
			if (!has_flags(lnx_it->flags, OS_File_Iter_Flag_Include_Hidden_Files) && entry->d_name[0] == '.') {
				good_entry = 0;
			}
		} else if (type == S_IFDIR && has_flags(lnx_it->flags, OS_File_Iter_Flag_Include_Directories)) {
			good_entry = 1;
		}

		if (good_entry) {
			*path = push_str_copy(arena, full);
			*props = os_lnx_file_properties_from_stat(&st);
			break;
		}
	}
	scratch_end(scratch);
	return 1;
}

//--------------------------------------------------------------------------------
// time procs

proc u64
os_time_us(void) {
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	u64 result = (u64)t.tv_sec*million(1) + ((u64)t.tv_nsec/thousand(1));
	return result;
}

proc u64
os_time_ms(void) {
	u64 result = os_time_us()/thousand(1);
	return result;
}

proc void
os_sleep_ms(u64 ms) {
	usleep((__useconds_t)(ms*thousand(1)));
}

//--------------------------------------------------------------------------------
// shared memory

proc OS_Shared_Memory
os_shared_memory_alloc(string str, s64 size, OS_File_Access_Flags flags)
{
	OS_Shared_Memory handle = zero_struct;
	Temp scr = scratch_begin(0, 0);
	cstring cstr = push_str_copy(scr.arena, str).m;

	int lnx_flags = O_RDONLY;
	if (flags & OS_File_Access_Flag_Read_Write) {
		lnx_flags = O_RDWR;
	}
	if (flags & OS_File_Access_Flag_Create) {
		lnx_flags |= O_CREAT;
	}
	int fd = shm_open(cstr, lnx_flags, 0755);
	if (fd >= 0) {
		ftruncate(fd, size);
		handle = (OS_Shared_Memory){ .v[0] = (u64)fd };
	}

	scratch_end(scr);
	return handle;
}

proc OS_Shared_Memory
os_shared_memory_open(string str, OS_File_Access_Flags flags) {
	OS_Shared_Memory handle = zero_struct;
	Temp scr = scratch_begin(0, 0);
	cstring cstr = push_str_copy(scr.arena, str).m;

	int lnx_flags = O_RDONLY;
	if (flags & OS_File_Access_Flag_Read_Write) {
		lnx_flags = O_RDWR;
	}
	if (flags & OS_File_Access_Flag_Create) {
		lnx_flags |= O_CREAT;
	}
	int fd = shm_open(cstr, lnx_flags, 0755);
	if (fd >= 0) {
		handle = (OS_Shared_Memory){ .v[0] = (u64)fd };
	}

	scratch_end(scr);
	return handle;
}

proc void
os_shared_memory_close(OS_Shared_Memory shared_memory) {
	assert(!handle_match(shared_memory, zero_handle(OS_Shared_Memory)));
	int fd = (int)shared_memory.v[0];
	close(fd);
}

proc rawptr
os_shared_memory_view_open(OS_Shared_Memory shared_memory, rng1s64 range) {
	assert(!handle_match(shared_memory, zero_handle(OS_Shared_Memory)));
	int fd = (int)shared_memory.v[0];
	rawptr ptr = mmap(0, (size_t)dim_1s64(range), PROT_READ | PROT_WRITE, MAP_SHARED, fd, range.min);
	if (ptr == MAP_FAILED) {
		ptr = 0;
	}
	return ptr;
}

proc void
os_shared_memory_view_close(OS_Shared_Memory shared_memory, rawptr ptr, rng1s64 range) {
	assert(!handle_match(shared_memory, zero_handle(OS_Shared_Memory)));
	munmap(ptr, (size_t)dim_1s64(range));
}

//--------------------------------------------------------------------------------
// threads

proc rawptr
_linux_thread_entry_point(rawptr entity) {
	Linux_Entity *e = (Linux_Entity*)entity;

	Thread_Context tctx;
	tctx_init_and_set(&tctx);
	tctx_set_name(e->thread.name);
	tctx_write_this_src_loc();

	e->thread.procedure(e->thread.params);
	tctx_release();
	return 0;
}

proc OS_Thread
os_thread_launch(OS_Thread_Proc *procedure, rawptr params, string name) {
	Linux_Entity *entity = os_lnx_entity_alloc(Linux_Entity_Kind_Thread);
	entity->thread.procedure = procedure;
	entity->thread.params = params;
	entity->thread.name = name;
	int pthread_result = pthread_create(&entity->thread.handle, 0, _linux_thread_entry_point, entity);
	if (pthread_result != 0) {
		os_lnx_entity_release(entity);
		entity = 0;
	}
	OS_Thread handle = {.v[0] = (u64)(entity)};
	return handle;
}

proc b32
os_thread_join(OS_Thread handle) {
	assert(!handle_match(handle, zero_handle(OS_Thread)));

	Linux_Entity *entity = (Linux_Entity*)(handle.v[0]);
	assert(entity->kind == Linux_Entity_Kind_Thread);
	int join_result = pthread_join(entity->thread.handle, 0);
	b32 result = (join_result == 0);
	os_lnx_entity_release(entity);
	return result;
}

proc void
os_thread_detach(OS_Thread handle) {
	assert(!handle_match(handle, zero_handle(OS_Thread)));

	Linux_Entity *entity = (Linux_Entity*)(handle.v[0]);
	assert(entity->kind == Linux_Entity_Kind_Thread);
	os_lnx_entity_release(entity);
}

//--------------------------------------------------------------------------------
// mutexes

proc OS_Mutex
os_mutex_alloc(void) {
	Linux_Entity *entity = os_lnx_entity_alloc(Linux_Entity_Kind_Mutex);
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	int init_result = pthread_mutex_init(&entity->mutex.handle, &attr);
	pthread_mutexattr_destroy(&attr);
	if (init_result == -1) {
		os_lnx_entity_release(entity);
		entity = 0;
	}
	OS_Mutex handle = {.v[0] = (u64)(entity)};
	return handle;
}

proc void
os_mutex_release(OS_Mutex mutex) {
	assert(!handle_match(mutex, zero_handle(OS_Mutex)));

	Linux_Entity *entity = (Linux_Entity*)(mutex.v[0]);
	assert(entity->kind == Linux_Entity_Kind_Mutex);
	pthread_mutex_destroy(&entity->mutex.handle);
	os_lnx_entity_release(entity);
}

proc void
os_mutex_take(OS_Mutex mutex) {
	assert(!handle_match(mutex, zero_handle(OS_Mutex)));

	Linux_Entity *entity = (Linux_Entity*)(mutex.v[0]);
	assert(entity->kind == Linux_Entity_Kind_Mutex);
	pthread_mutex_lock(&entity->mutex.handle);
}

proc void
os_mutex_drop(OS_Mutex mutex) {
	assert(!handle_match(mutex, zero_handle(OS_Mutex)));

	Linux_Entity *entity = (Linux_Entity*)(mutex.v[0]);
	assert(entity->kind == Linux_Entity_Kind_Mutex);
	pthread_mutex_unlock(&entity->mutex.handle);
}

//--------------------------------------------------------------------------------
// rw mutexes

proc OS_RW_Mutex
os_rw_mutex_alloc(void) {
	Linux_Entity *entity = os_lnx_entity_alloc(Linux_Entity_Kind_RW_Mutex);
	int lock_result = pthread_rwlock_init(&entity->rw_mutex.handle, 0);
	if (lock_result == -1) {
		os_lnx_entity_release(entity);
		entity = 0;
	}
	OS_RW_Mutex result = {.v[0] = (u64)(entity)};
	return result;
}
proc void
os_rw_mutex_release(OS_RW_Mutex mutex) {
	assert(!handle_match(mutex, zero_handle(OS_RW_Mutex)));

	Linux_Entity *entity = (Linux_Entity*)(mutex.v[0]);
	assert(entity->kind == Linux_Entity_Kind_RW_Mutex);
	pthread_rwlock_destroy(&entity->rw_mutex.handle);
	os_lnx_entity_release(entity);
}

proc void
os_mutex_take_r(OS_RW_Mutex mutex) {
	assert(!handle_match(mutex, zero_handle(OS_RW_Mutex)));

	Linux_Entity *entity = (Linux_Entity*)(mutex.v[0]);
	assert(entity->kind == Linux_Entity_Kind_RW_Mutex);
	pthread_rwlock_rdlock(&entity->rw_mutex.handle);
}

proc void
os_mutex_drop_r(OS_RW_Mutex mutex) {
	assert(!handle_match(mutex, zero_handle(OS_RW_Mutex)));

	Linux_Entity *entity = (Linux_Entity*)(mutex.v[0]);
	assert(entity->kind == Linux_Entity_Kind_RW_Mutex);
	pthread_rwlock_unlock(&entity->rw_mutex.handle);
}

proc void
os_mutex_take_w(OS_RW_Mutex mutex) {
	assert(!handle_match(mutex, zero_handle(OS_RW_Mutex)));

	Linux_Entity *entity = (Linux_Entity*)(mutex.v[0]);
	assert(entity->kind == Linux_Entity_Kind_RW_Mutex);
	pthread_rwlock_wrlock(&entity->rw_mutex.handle);
}

proc void
os_mutex_drop_w(OS_RW_Mutex mutex) {
	assert(!handle_match(mutex, zero_handle(OS_RW_Mutex)));

	Linux_Entity *entity = (Linux_Entity*)(mutex.v[0]);
	assert(entity->kind == Linux_Entity_Kind_RW_Mutex);
	pthread_rwlock_unlock(&entity->rw_mutex.handle);
}

//--------------------------------------------------------------------------------
// condition variables

proc OS_CV os_condition_variable_alloc(void) {
	Linux_Entity *entity = os_lnx_entity_alloc(Linux_Entity_Kind_CV);
	int init_result = pthread_cond_init(&entity->cv.handle, 0);
	if (init_result == -1) {
		os_lnx_entity_release(entity);
		entity = 0;
	}
	OS_CV result = {.v[0] = (u64)(entity)};
	return result;
}

proc void
os_condition_variable_release(OS_CV cv) {
	assert(!handle_match(cv, zero_handle(OS_CV)));

	Linux_Entity *entity = (Linux_Entity*)(cv.v[0]);
	assert(entity->kind == Linux_Entity_Kind_CV);
	pthread_cond_destroy(&entity->cv.handle);
	os_lnx_entity_release(entity);
}

proc b32
os_condition_variable_wait(OS_CV cv, OS_Mutex mutex, u64 endt_us) {
	assert(!handle_match(cv, zero_handle(OS_CV)));
	assert(!handle_match(mutex, zero_handle(OS_Mutex)));

	Linux_Entity *cv_entity = (Linux_Entity*)(cv.v[0]);
	Linux_Entity *mutex_entity = (Linux_Entity*)(mutex.v[0]);
	struct timespec timespec;
	timespec.tv_sec = endt_us/million(1);
	timespec.tv_nsec = thousand(1) * (s64)(endt_us - (endt_us/million(1))*million(1));
	assert(timespec.tv_nsec >= 0 && timespec.tv_nsec < billion(1));
	int wait_result = pthread_cond_timedwait(&cv_entity->cv.handle, &mutex_entity->mutex.handle, &timespec);
	b32 result = (wait_result != ETIMEDOUT);
	return result;
}

proc void
os_condition_variable_signal(OS_CV cv) {
	assert(!handle_match(cv, zero_handle(OS_CV)));

	Linux_Entity *entity = (Linux_Entity*)(cv.v[0]);
	pthread_cond_signal(&entity->cv.handle);
}

proc void
os_condition_variable_broadcast(OS_CV cv) {
	assert(!handle_match(cv, zero_handle(OS_CV)));

	Linux_Entity *entity = (Linux_Entity*)(cv.v[0]);
	pthread_cond_signal(&entity->cv.handle);
}

//--------------------------------------------------------------------------------
// semaphores

proc OS_Semaphore
os_semaphore_alloc(s32 initial_count, s32 max_count, string name) {
	assert(initial_count >= 0);
	assert(max_count >= 0);

	OS_Semaphore result = zero_struct;
	if (name.size > 0) {
		// shared memory for cross process semaphores
		not_implemented;
	} else {
		sem_t *s = (sem_t*)mmap(0, size_of(*s), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
		assert_always(s != MAP_FAILED);
		int err = sem_init(s, 0, (u32)initial_count);
		if (err == 0) {
			result.v[0] = (u64)(s);
		}
	}
	return result;
}
proc void
os_semaphore_release(OS_Semaphore semaphore) {
	assert(!handle_match(semaphore, zero_handle(OS_Semaphore)));

	sem_t *s = (sem_t*)(semaphore.v[0]);
	int err = munmap((rawptr)s, size_of(sem_t));
	assert_always(err == 0);
}

proc b32
os_semaphore_try(OS_Semaphore semaphore) {
	assert(!handle_match(semaphore, zero_handle(OS_Semaphore)));
	sem_t *s = (sem_t*)(semaphore.v[0]);

	int err = sem_trywait(s);
	b32 result = (err != EAGAIN);
	return result;
}

proc b32
os_semaphore_wait(OS_Semaphore semaphore, u64 endt_us) {
	assert(!handle_match(semaphore, zero_handle(OS_Semaphore)));
	sem_t *s = (sem_t*)(semaphore.v[0]);

	struct timespec timespec;
	timespec.tv_sec = endt_us/million(1);
	timespec.tv_nsec = thousand(1) * (s64)(endt_us - (endt_us/million(1))*million(1));
	assert(timespec.tv_nsec >= 0 && timespec.tv_nsec < billion(1));
	int err  = sem_timedwait(s, &timespec);
	assert(err != EINVAL);
	b32 result = (err != ETIMEDOUT);
	return result;
}

proc b32
os_semaphore_post(OS_Semaphore semaphore) {
	assert(!handle_match(semaphore, zero_handle(OS_Semaphore)));
	sem_t *s = (sem_t*)(semaphore.v[0]);
	int err = sem_post(s);
	assert(err != EINVAL);
	b32 result = (err != EOVERFLOW);
	return result;
}

//--------------------------------------------------------------------------------
// entry point

#if OS_ENTRY_POINT

int main(int argc, char **args) {
	// set up thread ctx
	Thread_Context tctx;
	tctx_init_and_equip(&tctx);
	tctx_set_name(S("main"));
	tctx_write_this_src_loc();

	// initialize miscellaneous
	linux_state.arena = arena_alloc();
	pthread_mutex_init(&linux_state.entity_mutex, 0);

	// Initialize system info
	{
		OS_System_Info *info = &linux_state.system_info;
		info->logical_processor_count = get_nprocs();
		info->page_size = getpagesize();
		info->large_page_size = megabyte(2);
		info->allocation_granularity = info->page_size;

		// get machine name
		{
			Temp scratch = scratch_begin(0, 0);
			u8 *buffer = 0;
			s64 host_name_size = 0;
			b32 got_host_name = 0;
			for (s64 r = 0, cap = 16; r < 8; r += 1, cap *= 2) {
				temp_end(scratch);
				buffer = push_array_no_zero(scratch.arena, u8, cap);
				int success = gethostname(buffer, (size_t)cap);
				if (success != -1) {
					got_host_name = 1;
					host_name_size = cstring_length(buffer);
					break;
				}
			}

			if (got_host_name && host_name_size > 0) {
				u8 *copy = push_array_no_zero(linux_state.arena, u8, host_name_size+1);
				memory_copy(copy, buffer, host_name_size);
				copy[host_name_size] = 0;
				OS_System_Info *info = &linux_state.system_info;
				info->machine_name = make_str(copy, host_name_size);
			}

			scratch_end(scratch);
		}
	}

	// initialize process info
	{
		OS_Process_Info *info = &linux_state.process_info;
		info->pid = (u32)getpid();

		// get binary path
		{
			Temp scratch = scratch_begin(0, 0);
			b32 got_bin_path = 0;

			u8 *buffer = 0;
			s64 size = 0;
			for (s64 r = 0, cap = 16; r < 8; r += 1, cap *= 2) {
				temp_end(scratch);
				buffer = push_array_no_zero(scratch.arena, u8, cap);
				ssize_t buf_size = readlink("/proc/self/exe", buffer, (size_t)cap);
				if (buf_size < cap) {
					got_bin_path = 1;
					size = (s64)buf_size;
					break;
				}
			}

			if (got_bin_path && size > 0) {
				info->bin_path = push_str_copy(linux_state.arena, make_str(buffer, size));
			}

			scratch_end(scratch);
		}

		// current path
		{
			info->current_path = os_get_current_path(linux_state.arena);
		}

		// user directory
		{
			char *home = getenv("HOME");
			info->user_path = push_str_copy(linux_state.arena, str_cstring(home));
		}
	}

	// call into base thread entry
	base_main_thread_entry((s32)argc, (cstring*)args);
	return 0;
}

#ifdef BASE_ENTRY_POINT
# error "Both base and os entry points are defined, only one is allowed."
#endif
#endif
