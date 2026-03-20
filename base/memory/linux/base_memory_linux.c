proc s64
memory_page_size(void) {
	return getpagesize();
}
proc s64
memory_large_page_size(void) {
	return megabyte(2);
}

proc rawptr
memory_reserve(s64 size, $mem_flags flags) {
	assert(size >= 0);
	int ln_flags = 0;
	ln_flags |= (MAP_ANONYMOUS | MAP_PRIVATE);
	ln_flags |= (flags & $mem_large_pages) ? (MAP_HUGETLB | MAP_HUGE_2MB) : 0;
	rawptr result = mmap(0, (size_t)size, PROT_NONE, ln_flags, -1, 0);
	if (result == MAP_FAILED) {
		result = 0;
	}
	return result;
}
proc b32
memory_commit(rawptr ptr, s64 size) {
	assert(size >= 0);
	b32 result = (mprotect(ptr, (size_t)size, PROT_READ|PROT_WRITE) == 0);
	return result;
}

proc void
memory_decommit(rawptr ptr, s64 size) {
	assert(size >= 0);
	madvise(ptr, (size_t)size, MADV_DONTNEED);
	mprotect(ptr, (size_t)size, PROT_NONE);
}
proc void
memory_release(rawptr base, s64 size) {
	assert(size >= 0);
	munmap(base, (size_t)size);
}
