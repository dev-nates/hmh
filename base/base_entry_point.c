

/*
	A commandline argument is parsed as follows:
	(--name=value || --name:value) is a name value pair denoted with a double dash prefix and a separator (= || :)
	(value || -value || --value) is a standalone value with no associated name.
	--name= is a name without a value
	--name="quote string" will be passed in as: `--name=quote string`
*/
proc Cmdline
base_cmdline_from_args(Arena *arena, s32 argc, cstring *args) {
	assert(argc > 0);
	Cmdline cl = zero_struct;

	for (s32 i = 1; i < argc; i += 1) {

		cstring arg = args[i];
		s64 size = cstring_length(arg);
		string arg_string = make_str(arg, size);

		b32 is_double_dash = 0;
		b32 is_single_dash = 0;
		string at = arg_string;

		if (at.size && at.m[0] == '-') {
			is_single_dash = 1;
			at = str_skip(at, 1);
		}

		if (at.size && at.m[0] == '-') {
			is_double_dash = 1;
			at = str_skip(at, 1);
		}

		s64 value_pos = (at.m - arg);

		s64 seperator_pos = -1;
		for (s64 i = 0; i < at.size; i += 1) {
			u8 c = at.m[i];
			if (c == '=' || c == ':') {
				seperator_pos = ((at.m+i) - arg);
				break;
			}
		}

		string name = str_zero();
		string value = str_zero();
		if (is_double_dash && seperator_pos != -1) {
			s64 name_pos = value_pos;
			value_pos = seperator_pos+1;

			name = str_substr(arg_string, r1s64(name_pos, seperator_pos));
			value = str_substr(arg_string, r1s64(value_pos, arg_string.size));
		} else {
			value = str_substr(arg_string, r1s64(value_pos, arg_string.size));
		}

		if (name.size || value.size) {
			Cmdline_Arg *arg_node = push_array(arena, Cmdline_Arg, 1);
			arg_node->name = name;
			arg_node->value = value;
			sll_queue_push(cl.head, cl.tail, arg_node);
			cl.count += 1;
		}
	}
	return cl;
}

proc string
value_from_cmdline_arg(Cmdline_Arg *arg, string optional_name, String_Match_Flags flags) {
	string result = str_zero();
	if (!optional_name.size || !arg->name.size) { result = arg->value; }

	if (optional_name.size && arg->name.size && str_match(optional_name, arg->name, flags)) {
		result = arg->value;
	}
	return result;
}

proc void
base_main_thread_entry(s32 argc, cstring *args)
{
	Temp scratch = scratch_begin(0, 0);
	Cmdline cl = base_cmdline_from_args(scratch.arena, argc, args);

	entry_point(cl);
	scratch_end(scratch);
}

#if BASE_ENTRY_POINT

int main(int argc, char **args) {
	Thread_Context tctx;
	tctx_init_and_equip(&tctx);
	tctx_set_name(S("main"));
	tctx_write_this_src_loc();

	base_main_thread_entry((s32)argc, (cstring*)args);
	return 0;
}

# ifdef OS_ENTRY_POINT
#  error "Both base and os entry points are defined, only one is allowed."
# endif
#else

# ifndef OS_ENTRY_POINT
#  error "entry point (base|os) must be selected via a compiler define."
# endif

#endif
