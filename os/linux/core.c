// -------------------------------------------------------------------------------------------------
// Helpers

proc File_Properties
_file_properties_from_stat(struct stat *s)
{
	File_Properties props = {0};
	props.size     = s->st_size;
	props.created  = _dense_time_from_timespec(s->st_ctim);
	props.modified = _dense_time_from_timespec(s->st_mtim);
	int type = s->st_mode & S_IFMT;
	if(type == S_IFDIR)
	{
	  props.flags |= File_Property_Flag_Is_Folder;
	}
	return props;
}

#define _LOG_ERRNO(name) \
	if (errno != 0) { \
		errorf("Error calling %s", name); \
	}

// -------------------------------------------------------------------------------------------------
// Metadata

proc Linux_System_Info
linux_system_info(void) {
	return linux_state->system_info;
}

proc Linux_Process_Info
linux_process_info(void) {
	return linux_state->process_info;
}

proc string8
linux_current_path(Arena *arena) {
	cstring working_dir = getcwd(0,0);
	string8 result = push_str_copy(arena, str_from_cstring(working_dir));
	free(working_dir);
	return result;
}

// -------------------------------------------------------------------------------------------------
// File procs

proc Linux_File
linux_file_open(string8 path, int access_mode, int flags) {
	Temp scr = scratch_begin(0, 0);
	cstring cstr = push_str_copy(scr.arena, path).m;

	errno = 0;
	int fd = open(cstr, access_mode, flags);
	_LOG_ERRNO("open")

	scratch_end(scr);

	Linux_File file = { .v[0] = cast(s64)(fd), };
	return file;
}

proc void
linux_file_close(Linux_File file) {
	assert(!linux_file_match(file, linux_file_zero()));
	int fd = cast(int)(file.v[0]);
	assert(fd >= 0);
	errno = 0;
	close(fd);
	_LOG_ERRNO("close")
}

proc s64 /*amt read*/
linux_file_read(Linux_File file, rng1s64 rng, rawptr out) {
	assert(!linux_file_match(file, linux_file_zero()));
	assert(0 <= rng.min && rng.min <= rng.max);

	s64 off = rng.min;
	int fd = cast(int)(file.v[0]);
	assert(fd >= 0);

	errno = 0;
	off_t offset = lseek(fd, off, SEEK_SET);
	_LOG_ERRNO("lseek");
	assert(offset == off);

	s64 size = dim_1s64(rng);
	errno = 0;
	ssize_t amt = read(fd, out, cast(u64)(size));
	_LOG_ERRNO("read");
	s64 result = clamp_bot(0, amt);
	return result;
}

proc s64 /*amt written*/
linux_file_write(Linux_File file, rng1s64 rng, rawptr out) {
	assert(!linux_file_match(file, linux_file_zero()));
	assert(0 <= rng.min && rng.min <= rng.max);

	int fd = cast(int)(file.v[0]);
	assert(fd >= 0);

	s64 written = 0;
	s64 left = dim_1s64(rng);
	for (;left > 0;) {
		errno = 0;
		off_t offset = lseek(fd, rng.min + written, SEEK_SET);
		_LOG_ERRNO("lseek");
		assert(offset == rng.min + written);

		errno = 0;
		ssize_t amt = write(fd, out + written, cast(u64)left);
		_LOG_ERRNO("write");

		if (amt >= 0) {
			written += amt;
			left -= amt;
		} else if (errno != EINTR) {
			break;
		}
	}
	return written;
}

proc File_Properties
linux_file_properties(Linux_File file) {
	assert(!linux_file_match(file, linux_file_zero()));
	int fd = cast(int)(file.v[0]);
	assert(fd >= 0);

	struct stat stat;
	errno = 0;
	fstat(fd, &stat);
	_LOG_ERRNO("fstat");

	File_Properties result = _file_properties_from_stat(&stat);
	return result;
}

// -------------------------------------------------------------------------------------------------
// Path

proc b8
linux_path_remove(string8 path) {
	Temp scr = scratch_begin(0, 0);
	cstring cstr = push_str_copy(scr.arena, path).m;
	errno = 0;
	b8 success = (remove(cstr) != -1);
	_LOG_ERRNO("remove");
	scratch_end(scr);
	return success;
}

proc b8
linux_path_exists(string8 path) {
	Temp scr = scratch_begin(0, 0);
	cstring cstr = push_str_copy(scr.arena, path).m;
	errno = 0;
	int success = access(cstr, F_OK);
	_LOG_ERRNO("access");
	b8 result = cast(b8)(success != -1);
	scratch_end(scr);
	return result;
}

proc b8
linux_path_copy(string8 dst, string8 src) {
	b8 result = false;
	Linux_File dst_file = linux_file_zero();
	Linux_File src_file = linux_file_zero();
	if (str_match(dst, src, String_Match_Flag_Slash_Insensitive)) { goto end; }

	dst_file = linux_file_open(dst, O_RDWR, O_CREAT);
	src_file = linux_file_open(src, O_RDONLY, 0);
	if (!linux_file_match(dst_file, linux_file_zero()) && !linux_file_match(src_file, linux_file_zero())) {
		int dst_fd = cast(int)(dst_file.v[0]);
		int src_fd = cast(int)(src_file.v[0]);
		assert(dst_fd != src_fd);
	}
	linux_file_close(dst_file);
	linux_file_close(src_file);
end:
	return result;
}

proc array8
linux_data_from_path(Arena *arena, string8 path) {
	Linux_File file = linux_file_open(path, O_RDWR, 0);
	s64 size = linux_file_properties(file).size;
	rawptr data = push_size(arena, size);
	s64 amt = linux_file_read(file, r1s64(0, size), data);
	assert(amt == size);
	linux_file_close(file);
	return make_array(cast(u8*)(data), amt);
}

proc b8
linux_write_data_to_path(string8 path, array8 data) {
	Linux_File file = linux_file_open(path, O_RDWR, O_CREAT);
	s64 amt = linux_file_write(file, r1s64(0, data.size), data.m);
	linux_file_close(file);
	return cast(b8)(amt == data.size);
}


//--------------------------------------------------------------------------------
// Entry Point

#if OS_ENTRY_POINT
int main(int argc, char **args) {
	// Thread context
	Thread_Context tctx;
	tctx_init_and_set(&tctx);
	tctx_set_name(S("main"));
	tctx_write_this_src_loc();

	// Arena
	Arena *arena = arena_alloc();
	linux_state = push_struct(arena, Linux_State);
	linux_state->arena = arena;

	// System Info
	{
		Linux_System_Info *info = &linux_state->system_info;
		info->logical_processor_count = get_nprocs();
		info->page_size = getpagesize();
		info->large_page_size = megabytes(2);

		// Machine name
		{
			Temp scratch = scratch_begin(0, 0);
			u8 *buffer = 0;
			s64 host_name_size = 0;
			b8 got_host_name = 0;
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
				u8 *copy = push_array_no_zero(linux_state->arena, u8, host_name_size+1);
				memory_copy(copy, buffer, host_name_size);
				copy[host_name_size] = 0;
				info->machine_name = make_str(copy, host_name_size);
			}

			scratch_end(scratch);
		}
	}

	// Process Info
	{
		Linux_Process_Info *info = &linux_state->process_info;
		info->pid = (u32)getpid();

		// get binary path
		{
			Temp scratch = scratch_begin(0, 0);
			b8 got_bin_path = 0;

			u8 *buffer = 0;
			s64 size = 0;
			for (s64 r = 0, cap = 16; r < 8; r += 1, cap *= 2) {
				temp_end(scratch);
				buffer = push_array_no_zero(scratch.arena, u8, cap);
				ssize_t buf_size = readlink("/proc/self/exe", buffer, cast(size_t)cap);
				if (buf_size < cap) {
					got_bin_path = 1;
					size = (s64)buf_size;
					break;
				}
			}

			if (got_bin_path && size > 0) {
				info->bin_path = push_str_copy(linux_state->arena, make_str(buffer, size));
			}

			scratch_end(scratch);
		}

		// current path
		{
			info->current_path = linux_current_path(linux_state->arena);
		}

		// user directory
		{
			char *home = getenv("HOME");
			info->user_path = push_str_copy(linux_state->arena, str_from_cstring(home));
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
