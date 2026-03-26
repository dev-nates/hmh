
typedef u64 $mem_flags;
enum {
	// TODO ns: Is "grows down" memory portable? Would be cool to have for stack memory.
	$mem_large_pages = bit5,
};

proc s64 memory_page_size(void);
proc s64 memory_large_page_size(void);

proc rawptr memory_reserve(s64 size, $mem_flags flags);
proc b8    memory_commit(rawptr ptr, s64 size);

proc void memory_decommit(rawptr ptr, s64 size);
proc void memory_release(rawptr base, s64 size);
