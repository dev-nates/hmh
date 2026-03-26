

proc array
os_data_from_path(Arena *arena, string path) {
	OS_File file = os_file_open(path, OS_File_Access_Flag_Read_Write);

	s64 size = os_file_get_properties(file).size;
	rawptr data = push_size(arena, size);
	s64 amt = os_file_read(file, r1s64(0, size), data);
	assert(amt == size);

	os_file_close(file);
	return make_array((u8*)data, amt);
}

proc b8
os_write_data_to_path(string path, array data) {
	OS_File file = os_file_open(path, (OS_File_Access_Flags)(OS_File_Access_Flag_Read_Write | OS_File_Access_Flag_Create));
	s64 amt = os_file_write(file, r1s64(0, data.size), data.m);
	os_file_close(file);
	return (amt == data.size);
}
