
// third party

disable_warnings_begin
# define STB_SPRINTF_IMPLEMENTATION
# define STB_SPRINTF_STATIC
# include <third_party/stb/stb_sprintf.h>
disable_warnings_end

read_only global u8 integer_from_ascii_symbol[128] = {
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};

read_only global u8 ascii_symbol_from_integer[16] = {
	'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f',
};

proc b8 char_is_space(u8 c) {
	return (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\v' || c == '\f');
}
proc b8 char_is_upper(u8 c) {
	return ('A' <= c && c <= 'Z');
}
proc b8 char_is_lower(u8 c) {
	return ('a' <= c && c <= 'z');
}
proc b8 char_is_alpha(u8 c) {
	return (char_is_lower(c) || char_is_upper(c));
}
proc b8 char_is_slash(u8 c) {
	return (c == '/' || c == '\\');
}
proc b8 char_is_digit(u8 c, u32 base) {
	b8 result = 0;
	if (0 < base && base <= 16) {
		u8 val = integer_from_ascii_symbol[c];
		if (val < base) {
			result = 1;
		}
	}
	return result;
}
proc u8 char_to_lower(u8 c) {
	if (char_is_upper(c)) {
		c += 'a' - 'A';
	}
	return c;
}
proc u8 char_to_upper(u8 c) {
	if (char_is_lower(c)) {
		c += 'A' - 'a';
	}
	return c;
}
proc u8 char_to_correct_slash(u8 c) {
	if (char_is_slash(c)) {
		c = '/';
	}
	return c;
}
proc u8 char_to_weak_ass_slash(u8 c) {
	if (char_is_slash(c)) {
		c = '\\';
	}
	return c;
}

//--------------------------------------------------------------------------------
// cstring lengths

proc s64
cstring_length(cstring c) {
 u8 *p = c;
	for (;*p != 0; p += 1);
	return p - c;
}

proc s64
cstring16_length(cstring16 c) {
 u16 *p = c;
	for (;*p != 0; p += 1);
	return p - c;
}

proc s64
cstring32_length(cstring32 c) {
 u32 *p = c;
	for (;*p != 0; p += 1);
	return p - c;
}

//--------------------------------------------------------------------------------
// str macros && constructors

proc string8
make_str(u8 *m, s64 size) { return (string8){m, size}; }

proc string8
str_range(u8 *head, u8 *opl) {
	s64 size = (s64)(opl - head);
	assert(size >= 0);
	size = clamp_bot(0, size);
	return make_str(head, size);
}

proc string8
str_zero(void) {
	string8 result = zero_struct;
	return result;
}

proc string8
str_from_cstring(cstring c) {
	return make_str(c, cstring_length(c));
}

proc string8
str_cstring_scan_to_cap(rawptr start, rawptr cap) {
	assert(cap >= start);
	u8 *at = (u8*)start;
	u8 *opl = (u8*)cap;
	for (;*at != '\0' && at < opl; at += 1);
	return str_range((u8*)(start), (u8*)(at));
}

proc string8
str_cstring_scan_backwards_from_pos(rawptr start, rawptr pos) {
	assert(pos >= start);
	u8 *at = (u8*)pos;
	u8 *bot = (u8*)start;
	for (; *at != '\0' && at > bot; at -= 1);
	return str_range((u8*)start, (u8*)at);
}

proc string16
make_str16(u16 *m, s64 size) { return (string16){m, size}; }

proc string16
str16_range(u16 *head, u16 *opl) {
	s64 size = (s64)(opl - head);
	assert(size >= 0);
	size = clamp_bot(0, size);
	return make_str16(head, size);
}

proc string16
str16_zero(void) {
	string16 result = zero_struct;
	return result;
}

proc string16
str16_cstring(cstring16 c) {
	return make_str16(c, cstring16_length(c));
}

proc string16
str16_cstring_scan_to_cap(rawptr start, rawptr cap) {
	assert(cap >= start);
	u16 *at = (u16*)start;
	u16 *opl = (u16*)cap;
	for (;*at != '\0' && at < opl; at += 1);
	return str16_range((u16*)start, (u16*)at);
}

proc string16
str16_cstring_scan_backwards_from_pos(rawptr start, rawptr pos) {
	assert(pos >= start);
	u16 *at = (u16*)pos;
	u16 *bot = (u16*)start;
	for (; *at != '\0' && at > bot; at -= 1);
	return str16_range((u16*)start, (u16*)at);
}

proc string32
make_str32(u32 *m, s64 size) { return (string32){m, size}; }

proc string32
str32_range(u32 *head, u32 *opl) {
	s64 size = (s64)(opl - head);
	assert(size >= 0);
	size = clamp_bot(0, size);
	return make_str32(head, size);
}

proc string32
str32_zero(void) {
	string32 result = zero_struct;
	return result;
}

proc string32
str32_cstring(cstring32 c) {
	return make_str32(c, cstring32_length(c));
}

proc string32
str32_cstring_scan_to_cap(rawptr start, rawptr cap) {
	assert(cap >= start);
	u32 *at = (u32*)start;
	u32 *opl = (u32*)cap;
	for (;*at != '\0' && at < opl; at += 1);
	return str32_range((u32*)start, (u32*)at);
}

proc string32
str32_cstring_scan_backwards_from_pos(rawptr start, rawptr pos) {
	assert(pos >= start);
	u32 *at = (u32*)pos;
	u32 *bot = (u32*)start;
	for (; *at != '\0' && at > bot; at -= 1);
	return str32_range((u32*)start, (u32*)at);
}

//--------------------------------------------------------------------------------
// copy && format

proc string8
push_str_cat(Arena *arena, string8 a, string8 b) {
	assert(a.size >= 0 && b.size >= 0);
	s64 total_size = a.size + b.size;
	u8 *mem = push_array_no_zero(arena, u8, total_size + 1);
	memory_copy(mem, a.m, a.size);
	memory_copy(mem + a.size, b.m, b.size);
	mem[total_size] = '\0';
	return make_str(mem, total_size);
}

proc string8
push_str_copy(Arena *arena, string8 str) {
	assert(str.size >= 0);
	u8 *mem = push_array_no_zero(arena, u8, str.size + 1);
	memory_copy(mem, str.m, str.size);
	mem[str.size] = '\0';
	return make_str(mem, str.size);
}

proc string8
push_strfv(Arena *arena, cstring fmt, va_list args) {
	va_list args2;
	va_copy(args2, args);
	s32 needed_bytes = base_string_vsnprintf(0, 0, fmt, args) + 1;
	string8 result = zero_struct;
	result.m = push_array_no_zero(arena, u8, needed_bytes);
	result.size = base_string_vsnprintf((cstring)result.m, needed_bytes, fmt, args2);
	result.m[result.size] = 0;
	va_end(args2);
	return result;
}

proc string8
push_strf(Arena *arena, cstring fmt, ...) {
	va_list args;
	va_start(args, fmt);
	string8 result = push_strfv(arena, fmt, args);
	va_end(args);
	return result;
}

//--------------------------------------------------------------------------------
// str stylization

proc string8
upper_from_str(Arena *arena, string8 str) {
	assert(str.size >= 0);
	if (str.size <= 0) return str;
	str = push_str_copy(arena, str);
	for (s64 idx = 0; idx < str.size; ++idx) {
		str.m[idx] = char_to_upper(str.m[idx]);
	}
	return str;
}

proc string8
lower_from_str(Arena *arena, string8 str) {
	assert(str.size >= 0);
	if (str.size <= 0) return str;
	str = push_str_copy(arena, str);
	for (s64 idx = 0; idx < str.size; ++idx) {
		str.m[idx] = char_to_lower(str.m[idx]);
	}
	return str;
}

proc string8
correct_slash_from_str(Arena *arena, string8 str) {
	assert(str.size >= 0);
	if (str.size <= 0) return str;
	str = push_str_copy(arena, str);
	for (s64 idx = 0; idx < str.size; ++idx) {
		str.m[idx] = char_to_correct_slash(str.m[idx]);
	}
	return str;
}

proc string8
weak_ass_slash_from_str(Arena *arena, string8 str) {
	assert(str.size >= 0);
	if (str.size <= 0) return str;
	str = push_str_copy(arena, str);
	for (s64 idx = 0; idx < str.size; ++idx) {
		str.m[idx] = char_to_weak_ass_slash(str.m[idx]);
	}
	return str;
}

//--------------------------------------------------------------------------------
// str matching

proc b8
str_match(string8 a, string8 b, String_Match_Flags flags) {
	assert(a.size >= 0 && b.size >= 0);
	b8 result = 0;
	if(a.size == b.size && flags == 0) {
	  result = memory_match(a.m, b.m, b.size);
	} else if(a.size == b.size || (flags & String_Match_Flag_Right_Side_Sloppy)) {
		b32 case_insensitive  = (flags & String_Match_Flag_Case_Insensitive);
		b32 slash_insensitive = (flags & String_Match_Flag_Slash_Insensitive);
		s64 size              = min(a.size, b.size);
		result = 1;
		for(s64 i = 0; i < size; i += 1) {
			u8 at = a.m[i];
			u8 bt = b.m[i];
			if(case_insensitive) {
				at = char_to_upper(at);
				bt = char_to_upper(bt);
			}
			if(slash_insensitive) {
				at = char_to_correct_slash(at);
				bt = char_to_correct_slash(bt);
			}
			if(at != bt) {
				result = 0;
				break;
			}
		}
	}
	return result;
}

proc s64
str_find_needle(s64 start_pos, string8 needle, string8 haystack, String_Match_Flags flags) {
	assert(needle.size >= 0 && haystack.size >= 0);
	assert(start_pos >= 0);
	u8 *p = haystack.m + start_pos;
	s64 stop_offset = max(haystack.size + 1, needle.size) - needle.size;
	u8 *stop_p = haystack.m + stop_offset;
	if (needle.size > 0) {
		u8 *string_opl = haystack.m + haystack.size;
		string8 needle_tail = str_skip(needle, 1);
		String_Match_Flags adjusted_flags = flags | String_Match_Flag_Right_Side_Sloppy;
		u8 needle_first_char_adjusted = needle.m[0];
		if(adjusted_flags & String_Match_Flag_Case_Insensitive) {
			needle_first_char_adjusted = char_to_upper(needle_first_char_adjusted);
		}
		for (;p < stop_p; p += 1) {
			u8 haystack_char_adjusted = *p;
			if(adjusted_flags & String_Match_Flag_Case_Insensitive) {
				haystack_char_adjusted = char_to_upper(haystack_char_adjusted);
			}
			if (haystack_char_adjusted == needle_first_char_adjusted) {
				if (str_match(str_range(p + 1, string_opl), needle_tail, adjusted_flags)){
					break;
				}
			}
		}
	}
	s64 result = haystack.size;
	if (p < stop_p) {
		result = (s64)(p - haystack.m);
	}
	return(result);
}

proc s64
str_find_needle_reverse(s64 start_pos, string8 needle, string8 haystack, String_Match_Flags flags) {
	assert(needle.size >= 0 && haystack.size >= 0);
	assert(start_pos >= 0);

	b32 right_side_sloppy = (flags & String_Match_Flag_Right_Side_Sloppy);
	b32 case_insensitive = (flags & String_Match_Flag_Case_Insensitive);
	b32 slash_insensitive = (flags & String_Match_Flag_Slash_Insensitive);
	start_pos = clamp(0, start_pos, haystack.size-1);

	s64 result = haystack.size;
	s64 size = 0;
	u8 *start = haystack.m + start_pos;
	u8 *at = start;
	for (;at >= haystack.m; at -= 1) {
		u8 *nat = needle.m;
		result = haystack.size;
		size = 0;
		u8 *with = at;
		u8 *needle_opl = needle.m + min((s64)(start - at)+1, needle.size);
		for (;nat < needle_opl; nat += 1, with += 1) {
			u8 a = *with;
			u8 b = *nat;
			if (case_insensitive) {
				a = char_to_lower(a);
				b = char_to_lower(b);
			}
			if (slash_insensitive) {
				a = char_to_correct_slash(a);
				b = char_to_correct_slash(b);
			}
			if (a == b) {
				result = (result == haystack.size) ? (s64)(at - haystack.m) : result;
				size += 1;
			} else {
				result = (size == 0 || !right_side_sloppy) ? haystack.size : result;
				break;
			}
		}

		if (result != haystack.size) {
			assert(size > 0);
			if (size == needle.size || right_side_sloppy) {
				break;
			}
		}
	}
	return result;
}

proc b8
str_ends_with(string8 str, string8 end, String_Match_Flags flags) {
	assert(str.size >= 0 && end.size >= 0);
	string8 postfix = str_postfix(str, end.size);
	b8 is_match = str_match(end, postfix, flags);
	return is_match;
}

//--------------------------------------------------------------------------------
// str slicing

proc string8
str_substr(string8 str, rng1s64 range) {
	assert(str.size >= 0);
	range.min = clamp_bot(0, clamp_top(range.min, str.size));
	range.max = clamp_bot(0, clamp_top(range.max, str.size));
	return make_str(str.m + range.min, dim_1s64(range));
}

proc string8
str_prefix(string8 str, s64 size) {
	assert(str.size >= 0);
	str.size = clamp_bot(0, clamp_top(size, str.size));
	return str;
}

proc string8
str_skip(string8 str, s64 amt) {
	assert(str.size >= 0);
	amt = clamp_bot(0, clamp_top(amt, str.size));
	str.m += amt;
	str.size -= amt;
	return str;
}

proc string8
str_postfix(string8 str, s64 size) {
	assert(str.size >= 0);
	size = clamp_bot(0, clamp_top(size, str.size));
	str.m = (str.m + str.size) - size;
	str.size = size;
	return str;
}

proc string8
str_chop(string8 str, s64 amt) {
	assert(str.size >= 0);
	amt = clamp_bot(0, clamp_top(amt, str.size));
	str.size -= amt;
	return str;
}

proc string8
str_skip_chop_whitespace(string8 str) {
	assert(str.size >= 0);
	u8 *head = str.m;
	u8 *opl = head + str.size;
	for (;head < opl; head += 1) {
		if (!char_is_space(*head)) {
			break;
		}
	}
	for (;opl > head;) {
		opl -= 1;
		if (!char_is_space(*opl)) {
			opl += 1;
			break;
		}
	}
	string8 result = str_range(head, opl);
	return result;
}

//--------------------------------------------------------------------------------
// str -> integer

proc s64
sign_from_str(string8 str, string8 *string_tail) {
	assert(str.size >= 0);
	s64 neg_count = 0;
	s64 i = 0;
	for (; i < str.size; i += 1) {
		if (str.m[i] == '-') {
			neg_count += 1;
		} else if (str.m[i] != '+') {
			break;
		}
	}

	*string_tail = str_skip(str, i);
	s64 sign = (neg_count & 1) ? -1 : +1;
	return sign;
}

proc b8
str_is_integer(string8 str, u32 radix) {
	assert(str.size > 0);
	b8 result = 0;
	if (str.size > 0) {
		if (1 < radix && radix <= 16) {
			result = 1;
			for (s64 i = 0; i < str.size; i += 1) {
				u8 c = str.m[i];
				if ((c >= 0x80) || integer_from_ascii_symbol[c] >= radix) {
					result = 0;
					break;
				}
			}
		}
	}
	return result;
}

proc u64
u64_from_str(string8 str, u32 radix) {
	assert(str.size >= 0);
	u64 x = 0;
	if (1 < radix && radix <= 16) {
		for (s64 i = 0; i < str.size; i += 1) {
			assert(integer_from_ascii_symbol[str.m[i]] != 0xff);
			#if DEBUG
			s64 reverse = (s64)integer_from_ascii_symbol[str.m[i]&0x7f];
			assert(0 <= reverse && reverse < radix);
			#endif
			x *= radix;
			x += integer_from_ascii_symbol[str.m[i]&0x7f];
		}
	}
	return x;
}

proc s64
s64_from_str(string8 str, u32 radix) {
	assert(str.size >= 0);
	s64 sign = sign_from_str(str, &str);
	s64 x = (s64)u64_from_str(str, radix) * sign;
	return x;
}

proc u32
u32_from_str(string8 str, u32 radix) {
	assert(str.size >= 0);
	u64 x64 = u64_from_str(str, radix);
	u32 x32 = safe_cast_u32(x64);
	return x32;
}

proc s32
s32_from_str(string8 str, u32 radix) {
	assert(str.size >= 0);
	s64 sign = sign_from_str(str, &str);
	s32 result = (s32)u64_from_str(str, radix) * (s32)sign;
	return result;
}

proc b8
try_u64_from_str_c_rules(string8 str, u64 *x) {
	assert(str.size >= 0);
	b8 is_integer = 0;
	b8 is_decimal = str_is_integer(str, 10);
	b8 starts_with_zero = (str.size && str.m[0] == '0');

	if (!starts_with_zero && is_decimal) {
		is_integer = 1;
		*x = u64_from_str(str, 10);
	} else if (str_match(str_prefix(str, 2), S("0x"), 0)) {
			is_integer = 1;
			*x = u64_from_str(str_skip(str, 2), 0x10);
	} else if (str_match(str_prefix(str, 2), S("0b"), 0)) {
			is_integer = 1;
			*x = u64_from_str(str_skip(str, 2), 0b10);
	} else if (starts_with_zero) {
		is_integer = 1;
		*x = u64_from_str(str_skip(str, 1), 010);
	}
	return is_integer;
}

proc b8 try_s64_from_str_c_rules(string8 str, s64 *x) {
	assert(str.size >= 0);
	s64 sign = sign_from_str(str, &str);
	b8 is_integer = try_u64_from_str_c_rules(str, x);
	*x *= sign;
	return is_integer;
}

proc string8
str_from_memory_size(Arena *arena, s64 size) {
	if (size < 0) return str_zero();

	string8 result;
	if (size < kilobytes(1)) {
		result = push_strf(arena, "%ld bytes", size);
	} else if (size < megabytes(1)) {
		result = push_strf(arena, "%ld.%.02ld KiB", size / kilobytes(1), ((size*100) / kilobytes(1)) % 100);
	} else if (size < gigabytes(1)) {
		result = push_strf(arena, "%ld.%.02ld MiB", size / megabytes(1), ((size*100) / megabytes(1)) % 100);
	} else if (size < terabytes(1)) {
		result = push_strf(arena, "%ld.%.02ld GiB", size / gigabytes(1), ((size*100) / gigabytes(1)) % 100);
	} else {
		result = push_strf(arena, "%ld.%.02ld TiB", size / terabytes(1), ((size*100) / terabytes(1)) % 100);
	}
	return result;
}

proc string8
str_from_count(Arena *arena, s64 count) {
	if (count < 0) return str_zero();

	string8 result;
	if (count < thousands(1)) {
		result = push_strf(arena, "%ld", count);
	} else if (count < millions(1)) {
		s64 perc = ((count*100)/thousands(1))%100;
		if (perc > 0) {
			result = push_strf(arena, "%lld.%02ldK", count/thousands(1), perc);
		} else {
			result = push_strf(arena, "%lldK", count/thousands(1));
		}
	} else if (count < billions(1)) {
		s64 perc = ((count*100)/millions(1))%100;
		if (perc > 0) {
			result = push_strf(arena, "%lld.%02ldM", count/millions(1), perc);
		} else {
			result = push_strf(arena, "%lldM", count/millions(1));
		}
	} else {
		s64 perc = ((count*100)/billions(1))%100;
		if (perc > 0) {
			result = push_strf(arena, "%lld.%02ldB", count/billions(1), perc);
		} else {
			result = push_strf(arena, "%lldB", count/billions(1));
		}
	}
	return result;
}

proc string8
str_from_bits_u32(Arena *arena, u32 x) {
	u8 c0 = ascii_symbol_from_integer[((x & 0xf0000000) >> 28)];
	u8 c1 = ascii_symbol_from_integer[((x & 0x0f000000) >> 24)];
	u8 c2 = ascii_symbol_from_integer[((x & 0x00f00000) >> 20)];
	u8 c3 = ascii_symbol_from_integer[((x & 0x000f0000) >> 16)];
	u8 c4 = ascii_symbol_from_integer[((x & 0x0000f000) >> 12)];
	u8 c5 = ascii_symbol_from_integer[((x & 0x00000f00) >> 8)];
	u8 c6 = ascii_symbol_from_integer[((x & 0x000000f0) >> 4)];
	u8 c7 = ascii_symbol_from_integer[((x & 0x0000000f) >> 0)];
	string8 result = push_strf(arena, "%c%c%c%c%c%c%c%c", c0, c1, c2, c3, c4, c5, c6, c7);
	return result;
}

proc string8
str_from_bits_u64(Arena *arena, u64 x) {
	u8 c0 = ascii_symbol_from_integer[((x & 0xf000000000000000) >> 60)];
	u8 c1 = ascii_symbol_from_integer[((x & 0x0f00000000000000) >> 56)];
	u8 c2 = ascii_symbol_from_integer[((x & 0x00f0000000000000) >> 52)];
	u8 c3 = ascii_symbol_from_integer[((x & 0x000f000000000000) >> 48)];
	u8 c4 = ascii_symbol_from_integer[((x & 0x0000f00000000000) >> 44)];
	u8 c5 = ascii_symbol_from_integer[((x & 0x00000f0000000000) >> 40)];
	u8 c6 = ascii_symbol_from_integer[((x & 0x000000f000000000) >> 36)];
	u8 c7 = ascii_symbol_from_integer[((x & 0x0000000f00000000) >> 32)];
	u8 c8 = ascii_symbol_from_integer[((x & 0x00000000f0000000) >> 28)];
	u8 c9 = ascii_symbol_from_integer[((x & 0x000000000f000000) >> 24)];
	u8 ca = ascii_symbol_from_integer[((x & 0x0000000000f00000) >> 20)];
	u8 cb = ascii_symbol_from_integer[((x & 0x00000000000f0000) >> 16)];
	u8 cc = ascii_symbol_from_integer[((x & 0x000000000000f000) >> 12)];
	u8 cd = ascii_symbol_from_integer[((x & 0x0000000000000f00) >> 8)];
	u8 ce = ascii_symbol_from_integer[((x & 0x00000000000000f0) >> 4)];
	u8 cf = ascii_symbol_from_integer[((x & 0x000000000000000f) >> 0)];
	string8 result = push_strf(arena, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", c0, c1, c2, c3, c4, c5, c6, c7, c8, c9, ca, cb, cc, cd, ce, cf);
	return result;
}

proc string8
str_from_u64(Arena *arena, u64 val, u32 radix, s64 min_digits, u8 separator) {
	assert(min_digits >= 0);

	string8 prefix = str_zero();
	switch (radix) {
		case 0x10: prefix = S("0x"); break;
		case 010: prefix = S("0o"); break;
		case 0b10: prefix = S("0b"); break;
	}

	s64 digit_count = 1;
	u64 copy = val;
	for (;;) {
		copy /= radix;
		if (copy == 0) break;
		digit_count += 1;
	}

	s64 separator_group_size = 3;
	switch (radix) {
		case 16:
		case 2:
			separator_group_size = 4;
			break;
		case 8:
			separator_group_size = 3;
	}

	s64 leading_0s_count = clamp_bot(0, min_digits-digit_count);
	s64 total_digits_count = leading_0s_count + digit_count;

	s64 separator_count = total_digits_count/separator_group_size;
	if (separator_count*separator_group_size == total_digits_count) {
		separator_count -= 1;
	}

	s64 buffer_size = prefix.size + total_digits_count + separator_count;
	u8 *buf = push_array_no_zero(arena, u8, buffer_size + 1);

	u8 *at = buf + buffer_size-1;
	s64 i = buffer_size-1;
	s64 counter = 0;
	for (;i >= prefix.size; i -= 1, at -= 1, counter += 1) {
		if (counter == separator_group_size) {
			*at = separator;
			counter = -1;
			continue;
		}

		if (digit_count == 0) {
			*at = '0';
			continue;
		}

		if (digit_count > 0) {
			u64 integer = val % radix;
			val /= radix;
			assert(integer >= 0 && integer < array_count(ascii_symbol_from_integer));
			u8 c = ascii_symbol_from_integer[integer];
			*at = c;
			digit_count -= 1;
			continue;
		}
	}

	buf[buffer_size] = 0;
	assert(buffer_size >= prefix.size);
	memory_copy(buf, prefix.m, prefix.size);

	string8 result = make_str(buf, buffer_size);
	return result;
}

proc string8
str_from_s64(Arena *arena, s64 val, u32 radix, s64 min_digits, u8 separator) {
	assert(min_digits >= 0);
	assert(radix <= 16);

	b32 has_sign = (val < 0);
	val = abs_s64(val);

	string8 prefix = str_zero();
	switch (radix) {
		case 0x10: prefix = (has_sign) ? S("-0x") : S("0x"); break;
		case 010: prefix = (has_sign) ? S("-0o") : S("0o"); break;
		case 0b10: prefix = (has_sign) ? S("-0b") : S("0b"); break;
		default: prefix = (has_sign) ? S("-") : str_zero(); break;
	}

	s64 digit_count = 1;
	s64 copy = val;
	for (;;) {
		copy /= radix;
		if (copy == 0) break;
		digit_count += 1;
	}

	s64 separator_group_size = 3;
	switch (radix) {
		case 16:
		case 2:
			separator_group_size = 4;
			break;
		case 8:
			separator_group_size = 3;
	}

	s64 leading_0s_count = clamp_bot(0, min_digits-digit_count);
	s64 total_digits_count = leading_0s_count + digit_count;

	s64 separator_count = total_digits_count/separator_group_size;
	if (separator_count*separator_group_size == total_digits_count) {
		separator_count -= 1;
	}

	s64 buffer_size = prefix.size + total_digits_count + separator_count;
	u8 *buf = push_array_no_zero(arena, u8, buffer_size + 1);

	u8 *at = buf + buffer_size-1;
	s64 i = buffer_size-1;
	s64 counter = 0;
	for (;i >= prefix.size; i -= 1, at -= 1, counter += 1) {
		if (counter == separator_group_size) {
			*at = separator;
			counter = -1;
			continue;
		}

		if (digit_count == 0) {
			*at = '0';
			continue;
		}

		if (digit_count > 0) {
			s64 integer = val % radix;
			val /= radix;
			assert(integer >= 0 && integer < array_count(ascii_symbol_from_integer));
			u8 c = ascii_symbol_from_integer[integer];
			*at = c;
			digit_count -= 1;
			continue;
		}
	}

	buf[buffer_size] = 0;
	assert(buffer_size >= prefix.size);
	memory_copy(buf, prefix.m, prefix.size);

	string8 result = make_str(buf, buffer_size);
	return result;
}

proc f64
f64_from_str(string8 str) {
	f64 result = 0;
	if (str.size <= 0) { return result; }

	f64 sign = (f64)sign_from_str(str, &str);
	f64 exp_sign = 1;
	f64 num = 0;
	f64 exp = 0;

	// find ranges for [num_rng][e+-][exp_rng]
	rng1s64 num_rng = r1s64(0, str.size);
	rng1s64 exp_rng = (rng1s64){.min = 1, .max = 0};
	for (s64 i = 0; i < str.size; i += 1) {
		b32 is_exp_symbol = (char_to_lower(str.m[i]) == 'e');
		if (is_exp_symbol) {
			num_rng.max = i;
			s64 exp_sign_idx = i + 1;
			b32 is_exp_sign = ((exp_sign_idx) < str.size) ? (str.m[exp_sign_idx] == '-') || (str.m[exp_sign_idx] == '+') : 0;
			exp_sign = (is_exp_sign) ? ((str.m[exp_sign_idx] == '-') ? -1 : 1) : 1;
			if (!is_exp_sign) goto end;

			exp_rng = r1s64(exp_sign_idx+1, str.size);
			i += 1;
			continue;
		}

		assert(str.m[i] < array_count(integer_from_ascii_symbol));
		s64 integer = integer_from_ascii_symbol[str.m[i]];
		if (integer >= 10) {
			goto end;
		}
	}

	// solve for num
	{
		f64 pow = 1;
		assert(num_rng.min >= 0);
		for (s64 i = num_rng.max-1; i >= num_rng.min; i -= 1) {
			u8 c = str.m[i];
			assert(c < array_count(integer_from_ascii_symbol));
			s64 integer = integer_from_ascii_symbol[str.m[i]];
			assert(integer < 10);

			num += (f64)integer * pow;
			pow *= 10;
		}
	}


	// solve for exp
	{
		f64 pow = 1;
		assert(exp_rng.min >= 0);
		for (s64 i = exp_rng.max-1; i >= exp_rng.min; i -= 1) {
			u8 c = str.m[i];
			assert(c < array_count(integer_from_ascii_symbol));
			s64 integer = integer_from_ascii_symbol[str.m[i]];
			assert(integer < 10);

			exp += (f64)integer * pow;
			pow *= 10;
		}
	}

	result = sign*num*pow_f64(10, exp*exp_sign);
	end:;
	return result;
}

//--------------------------------------------------------------------------------
// str list procs

proc string_node*
str_list_push_node(string_list *list, string_node *node) {
	assert(node->str.size >= 0);

	string_node **ptr = check_nil(list->head, nil) ? &list->head : &list->tail->next;
	*ptr = node; list->tail = node;
	node->next = nil;
	list->node_count += 1;
	list->total_size += node->str.size;
	return node;
}

proc string_node*
str_list_push_node_set_string(string_list *list, string_node *node, string8 str) {
	assert(str.size >= 0);
	node->str = str;
	str_list_push_node(list, node);	
	return node;
}

proc string_node*
str_list_push_node_front(string_list *list, string_node *node) {
	assert(node->str.size >= 0);

	node->next = list->head;
	list->head = node;
	list->node_count += 1;
	list->total_size += node->str.size;
	return node;
}

proc string_node*
str_list_push_node_front_set_string(string_list *list, string_node *node, string8 str) {
	assert(str.size >= 0);
	node->str = str;

	node->next = list->head;
	list->head = node;
	return node;
}

proc string_node*
str_list_push(Arena *arena, string_list *list, string8 str) {
	assert(str.size >= 0);
	string_node *node = push_array_no_zero(arena, string_node, 1);
	node->str = str;
	str_list_push_node(list, node);
	return node;
}

proc string_node*
str_list_push_front(Arena *arena, string_list *list, string8 str) {
	assert(str.size >= 0);
	string_node *node = push_array_no_zero(arena, string_node, 1);
	node->str = str;
	str_list_push_node_front(list, node);
	return node;
}

proc string_node*
str_list_push_copy(Arena *arena, string_list *list, string8 str) {
	assert(str.size >= 0);
	string_node *node = push_array_no_zero(arena, string_node, 1);
	string8 copy = push_str_copy(arena, str);
	node->str = copy;
	str_list_push_node(list, node);
	return node;
}

proc string_node*
str_list_push_front_copy(Arena *arena, string_list *list, string8 str) {
	assert(str.size >= 0);
	string_node *node = push_array_no_zero(arena, string_node, 1);
	string8 copy = push_str_copy(arena, str);
	node->str = copy;
	str_list_push_node_front(list, node);
	return node;
}


proc void
str_list_concat_in_place(string_list *list, string_list *to_push) {
	if (list->head) {
		list->node_count += to_push->node_count;
		list->total_size += to_push->total_size;
		list->tail->next = to_push->head;
		list->tail = to_push->tail;
	} else {
		*list = *to_push;
	}
	memory_zero_struct(to_push);
}

proc string_node*
str_list_push_aligner(Arena *arena, string_list *list, s64 min, s64 align) {
	assert(min >= 0 && align > 0);
	assert(is_pow2(align));
	s64 new_size = list->total_size + min;
	new_size = align_pow2(new_size, align);

	s64 string_size = new_size - list->total_size;
	string_node *node = 0;
	local_persist u8 zeroes_buffer[64] = {0};
	if (string_size) {
		assert(string_size <= array_count(zeroes_buffer));
		node = push_array_no_zero(arena, string_node, 1);
		node->str.m = zeroes_buffer;
		node->str.size = string_size;
		str_list_push_node(list, node);
	}
	return node;
}

proc string_node*
str_list_pushf(Arena *arena, string_list *list, cstring fmt, ...) {
	va_list args;
	va_start(args, fmt);
	string8 str = push_strfv(arena, fmt, args);
	va_end(args);
	string_node *node = push_array_no_zero(arena, string_node, 1);
	node->str = str;
	str_list_push_node(list, node);
	return node;
}

proc string_node*
str_list_push_frontf(Arena *arena, string_list *list, cstring fmt, ...) {
	va_list args;
	va_start(args, fmt);
	string8 str = push_strfv(arena, fmt, args);
	va_end(args);
	string_node *node = push_array_no_zero(arena, string_node, 1);
	node->str = str;
	str_list_push_node_front(list, node);
	return node;
}

proc string_list
str_list_copy(Arena *arena, string_list *list) {
	string_list result = zero_struct;
	for (string_node *at = list->head; at != 0; at = at->next) {
		string_node *copy = push_array_no_zero(arena, string_node, 1);
		string8 copy_str = push_str_copy(arena, at->str);
		copy->str = copy_str;
		str_list_push_node(&result, copy);
	}
	return result;
}

//--------------------------------------------------------------------------------
// splitting & joining

proc string_list
str_split(Arena *arena, string8 str, u8 *split_chars, s32 split_char_count, String_Split_Flags flags) {
	assert(str.size >= 0);
	assert(split_char_count >= 0);
	string_list list = zero_struct;
	b32 keep_empties = flags & String_Split_Flag_Keep_Empties;

	u8 *at = str.m;
	u8 *opl = str.m + str.size;
	rng1s64 rng = zero_struct;
	for (;at < opl; at += 1) {
		if (at == opl-1) {
			rng.max = str.size;
		}

		b32 encountered_split_char = 0;
		for (u8 *split = split_chars; split < (split_chars + split_char_count); split += 1) {
			if (*split == *at) {
				rng.max = at - str.m;
				encountered_split_char = 1;
				break;
			}
		}

		string8 substr = str_substr(str, rng);
		if (keep_empties || substr.size) {
			str_list_push(arena, &list, substr);
		}

		if (encountered_split_char) {
			rng.min = rng.max = ((at+1) - str.m);
		}
	}
	return list;
}

proc string_list
str_split_by_string_chars(Arena *arena, string8 str, string8 split_chars, String_Split_Flags flags) {
	assert(str.size >= 0 && split_chars.size >= 0);
	string_list list = zero_struct;
	b32 keep_empties = flags & String_Split_Flag_Keep_Empties;

	u8 *at = str.m;
	u8 *opl = str.m + str.size;
	rng1s64 rng = zero_struct;
	for (;at < opl; at += 1) {
		if (at == opl-1) {
			rng.max = str.size;
		}

		for (u8 *split = split_chars.m; split < (split_chars.m + split_chars.size); split += 1) {
			if (*split == *at) {
				rng.max = at - str.m;
				break;
			}
		}

		string8 substr = str_substr(str, rng);
		if (keep_empties || substr.size) {
			str_list_push(arena, &list, substr);
			rng.min = rng.max = ((at+1) - str.m);
		}
	}
	return list;
}

proc string8
str_list_join(Arena *arena, string_list *list, String_Join *optional_params) {
	String_Join join = (optional_params) ? *optional_params : (String_Join)zero_struct;
	string8 str;
	s64 total_size = list->total_size + join.pre.size + join.sep.size*max(0, list->node_count-1) + join.pst.size;
	u8 *mem = (u8*)push_size(arena, total_size + 1);
	u8 *at = mem;

	memory_copy(at, join.pre.m, join.pre.size);
	at += join.pre.size;

	for (each_node(node, list->head, string_node)) {
		string8 node_string = node->str;
		memory_copy(at, node_string.m, node_string.size);
		at += node_string.size;

		if (!check_nil(node->next, nil)) {
			memory_copy(at, join.sep.m, join.sep.size);
			at += join.sep.size;
		}
	}

	memory_copy(at, join.pst.m, join.pst.size);
	#if DEBUG
	at += join.pst.size;
	assert(at == (mem + total_size));
	#endif

	mem[total_size] = 0;
	return make_str(mem, total_size);
}

proc string_list
str_list_from_flags(Arena *arena, u32 flags, string8 *flag_string_table, s32 flag_string_count) {
	string_list list = zero_struct;
	assert(flag_string_count >= 0);
	for (each_index(i, flag_string_count)) {
		u32 flag = (1<<i);
		if ((flag&flags) != 0) {
			str_list_push_copy(arena, &list, flag_string_table[i]);
		}
	}
	return list;
}

//--------------------------------------------------------------------------------
// str arrays

proc string_array
str_array_from_list(Arena *arena, string_list *list) {
	string_array arr = zero_struct;
	arr.count = list->node_count;
	arr.v = push_array_no_zero(arena, string8, list->node_count);
	s64 idx = 0;
	for (each_node(node, list->head, string_node)) {
		assert(idx < list->node_count);
		arr.v[idx] = node->str;
		idx += 1;
	}
	return arr;
}

proc string_array
str_array_reserve(Arena *arena, s64 count) {
	assert(count >= 0);
	string_array arr;
	arr.count = count;
	arr.v = push_array(arena, string8, count);
	return arr;
}

//--------------------------------------------------------------------------------
// path helpers

proc string8
str_chop_last_slash(string8 str) {
	assert(str.size >= 0);
	if (str.size <= 0) return str;

	u8 *at = str.m + (str.size-1);
	for (;at >= str.m; at -= 1) {
		if (*at == '/' || *at == '\\') {
			break;
		}
	}
	str = (at >= str.m) ? str_range(str.m, at) : str;
	return str;
}

proc string8
str_skip_last_slash(string8 str) {
	assert(str.size >= 0);
	if (str.size <= 0) return str;

	u8 *at = str.m + (str.size-1);
	for (;at >= str.m; at -= 1) {
		if (*at == '/' || *at == '\\') {
			break;
		}
	}
	u8 *opl = str.m + str.size;
	str = (at >= str.m) ? str_range(at+1, opl) : str;
	return str;
}

proc string8
str_chop_last_dot(string8 str) {
	assert(str.size >= 0);
	if (str.size <= 0) return str;

	u8 *at = str.m + (str.size-1);
	for (;at >= str.m; at -= 1) {
		if (*at == '.') {
			break;
		}
	}
	str = (at >= str.m) ? str_range(str.m, at) : str;
	return str;
}

proc string8
str_skip_last_dot(string8 str) {
	assert(str.size >= 0);
	if (str.size <= 0) return str;

	u8 *at = str.m + (str.size-1);
	for (;at >= str.m; at -= 1) {
		if (*at == '.') {
			break;
		}
	}
	u8 *opl = str.m + str.size;
	str = (at >= str.m) ? str_range(at+1, opl) : str;
	return str;
}

proc Path_Style
path_style_from_str(string8 str) {
	assert(str.size >= 0);
	Path_Style style = Path_Style_Null;
	assert(str.size >= 0);
	if (str.size == 0) { return style; }
	if (str.m[0] == '/') {
		style = Path_Style_Unix_Absolute;
		goto end;
	}

	if (str.size >= 2 && char_is_alpha(str.m[0]) && str.m[1] == ':') {
		style = Path_Style_Windows_Absolute;
		if (str.size > 2 && !char_is_slash(str.m[2])) {
			style = Path_Style_Null;
		}
		goto end;
	}

	end:;
	return style;
}

proc string_list
str_split_path(Arena *arena, string8 str) {
	assert(str.size >= 0);
	string_list result = str_split(arena, str, (u8*)"/\\", 2, 0);
	return result;
}

proc void
str_path_list_resolve_dots_in_place(string_list *path, Path_Style style) {
	Temp scratch = scratch_begin(0, 0);
	string_meta_node *stack = 0;
	string_meta_node *free_meta_nodes = 0;
	string_node *head = path->head;

	memory_zero_struct(path);
	for (string_node *node = head, *next = 0;
			node != 0;
			node = next) {
		// save next now
		next = node->next;
		
		// cases:
		if (node == head && style == Path_Style_Windows_Absolute) {
			goto save_without_stack;
		}
		if (node->str.size == 1 && node->str.m[0] == '.') {
			goto do_nothing;
		}
		if (node->str.size == 2 && node->str.m[0] == '.' && node->str.m[1] == '.') {
			if (stack != 0) {
				goto eliminate_stack_top;
			} else {
				goto save_without_stack;
			}
		}
		goto save_with_stack;
		
		
		// handlers:
		save_with_stack:
		{
		  str_list_push_node(path, node);

		  string_meta_node *stack_node = free_meta_nodes;
		  if (!check_nil(stack_node, nil)) {
		  	asan_cure(stack_node, size_of(*stack_node));
		  	free_meta_nodes = free_meta_nodes->next;
		  } else {
		  	stack_node = push_struct_no_zero(scratch.arena, string_meta_node);
		  }
		  memory_zero_struct(stack_node);
		  stack_node->next = stack; stack = stack_node;

		  stack_node->node = node;
		  continue;
		}
		
		save_without_stack:
		{
			str_list_push_node(path, node);
			continue;
		}
		
		eliminate_stack_top:
		{
			path->node_count -= 1;
			path->total_size -= stack->node->str.size;
			
			stack = stack->next;
			if (stack == nil) {
				path->tail = path->head;
			} else {
				path->tail = stack->node;
			}
			continue;
		}
		
		do_nothing: continue;
	}
	scratch_end(scratch);
}

proc string8
str_path_list_join_by_style(Arena *arena, string_list *path, Path_Style style) {
	String_Join params = {0};
	switch (style)
	{
		case Path_Style_Null:{}break;
		case Path_Style_Relative:
		case Path_Style_Windows_Absolute: {
			params.sep = S("/");
		}break;
		
		case Path_Style_Unix_Absolute: {
			params.pre = S("/");
			params.sep = S("/");
		}break;
	}
	string8 result = str_list_join(arena, path, &params);
	return result;
}

proc string8
str_extension_from_path(string8 path)
{
	assert(path.size >= 0);
	if (path.size <= 0) return path;

	u8 *head = path.m;
	u8 *opl = path.m + path.size;
	u8 *at = opl-1;
	s64 pos = path.size;
	for (;at>=head; at -= 1) {
		if (*at == '.') {
			pos = (at-head)+1;
		}
	}
	pos = clamp(0, pos, path.size);

	string8 result = str_substr(path, r1s64(pos, path.size));
	return result;
}

proc string8
str_remove_extension_from_path(string8 path) {
	assert(path.size >= 0);
	if (path.size <= 0) return path;
	u8 *head = path.m;
	u8 *opl = path.m + path.size;
	u8 *at = opl-1;
	s64 pos = path.size;
	for (;at>=head; at -= 1) {
		if (*at == '.') {
			pos = (at-head);
		}
	}
	pos = clamp(0, pos, path.size);

	string8 result = str_substr(path, r1s64(0, pos));
	return result;
}

proc String_Loc_Pair
str_loc_pair_from_string(string8 str) {
	assert(str.size >= 0);
	String_Loc_Pair pair = {0};
	{
		string8 file_part = {0};
		string8 line_part = {0};
		string8 col_part = {0};
		
		// rjf: grab file part
		for(s64 idx = 0; idx <= str.size; idx += 1)
		{
			u8 byte = (idx < str.size) ? (str.m[idx]) : 0;
			u8 next_byte = ((idx+1 < str.size) ? (str.m[idx+1]) : 0);
			if(byte == ':' && next_byte != '/' && next_byte != '\\') {
				file_part = str_prefix(str, idx);
				line_part = str_skip(str, idx+1);
				break;
			} else if(byte == 0) {
				file_part = str;
				break;
			}
		}
		
		// rjf: grab line/column
		{
			s64 colon_pos = str_find_needle(0, S(":"), line_part, 0);
			if(colon_pos < line_part.size) {
				col_part = str_skip(line_part, colon_pos+1);
				line_part = str_prefix(line_part, colon_pos);
			}
		}
		
		// rjf: convert line/column strings to numerics
		s64 line = 0;
		s64 column = 0;
		try_u64_from_str_c_rules(line_part, (u64 *)&line);
		try_u64_from_str_c_rules(col_part, (u64 *)&column);
		
		// rjf: fill
		pair.str = file_part;
		pair.loc = make_loc((s64)line, (s64)column);
		if(pair.loc.line == 0) { pair.loc.line = 1; }
		if(pair.loc.col == 0) { pair.loc.col = 1; }
	}
	return pair;
}

//--------------------------------------------------------------------------------
// utf-8 / utf-16 encoding / decoding
read_only global u8 utf8_class[32] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,2,2,2,2,3,3,4,5, };
proc Unicode_Decode
utf8_decode(u8 *str, s64 max) {
	Unicode_Decode result = {1, max_u32};
	u8 byte = str[0];
	u8 byte_class = utf8_class[byte >> 3];
	switch (byte_class) {
		case 1: {
			result.codepoint = byte;
		}break;
		case 2: {
			if (2 < max) {
				u8 cont_byte = str[1];
				if (utf8_class[cont_byte >> 3] == 0) {
					result.codepoint = (byte & cast(u32)(bitmask5)) << 6;
					result.codepoint |=  (cont_byte & cast(u32)(bitmask6));
					result.inc = 2;
				}
			}
		}break;
		case 3:
		{
			if (2 < max) {
				u8 cont_byte[2] = {str[1], str[2]};
				if (utf8_class[cont_byte[0] >> 3] == 0 &&
					utf8_class[cont_byte[1] >> 3] == 0)
				{
					result.codepoint = (byte & cast(u32)(bitmask4)) << 12;
					result.codepoint |= ((cont_byte[0] & cast(u32)(bitmask6)) << 6);
					result.codepoint |=  (cont_byte[1] & cast(u32)(bitmask6));
					result.inc = 3;
				}
			}
		} break;
		case 4: {
			if (3 < max) {
				u8 cont_byte[3] = {str[1], str[2], str[3]};
				if (utf8_class[cont_byte[0] >> 3] == 0 &&
					utf8_class[cont_byte[1] >> 3] == 0 &&
					utf8_class[cont_byte[2] >> 3] == 0)
				{
					result.codepoint = (byte & cast(u32)(bitmask3)) << 18;
					result.codepoint |= ((cont_byte[0] & cast(u32)(bitmask6)) << 12);
					result.codepoint |= ((cont_byte[1] & cast(u32)(bitmask6)) <<  6);
					result.codepoint |=  (cont_byte[2] & cast(u32)(bitmask6));
					result.inc = 4;
				}
			}
		}
	}
	return result;
}

proc Unicode_Decode
utf16_decode(u16 *str, s64 max) {
	Unicode_Decode result = {1, max_u32};
	result.codepoint = str[0];
	result.inc = 1;
	if (max > 1 && 0xD800 <= str[0] && str[0] < 0xDC00 && 0xDC00 <= str[1] && str[1] < 0xE000){
		result.codepoint = (u32)((str[0] - 0xD800) << 10) | (u32)((str[1] - 0xDC00) + 0x10000);
		result.inc = 2;
	}
	return result;
}

proc s32
utf8_encode(u8 *str, u32 codepoint) {
	s32 inc = 0;
	if (codepoint <= 0x7f){
		str[0] = (u8)codepoint;
		inc = 1;
	}
	else if (codepoint <= 0x7ff){
		str[0] = (cast(u32)(bitmask2) << 6) | ((codepoint >> 6) & cast(u32)(bitmask5));
		str[1] = bit8 | (codepoint & cast(u32)(bitmask6));
		inc = 2;
	}
	else if (codepoint <= 0xffff){
		str[0] = (cast(u32)(bitmask3) << 5) | ((codepoint >> 12) & cast(u32)(bitmask4));
		str[1] = bit8 | ((codepoint >> 6) & cast(u32)(bitmask6));
		str[2] = bit8 | ( codepoint       & cast(u32)(bitmask6));
		inc = 3;
	}
	else if (codepoint <= 0x10ffff){
		str[0] = (cast(u32)(bitmask4) << 4) | ((codepoint >> 18) & cast(u32)(bitmask3));
		str[1] = bit8 | ((codepoint >> 12) & cast(u32)(bitmask6));
		str[2] = bit8 | ((codepoint >>  6) & cast(u32)(bitmask6));
		str[3] = bit8 | ( codepoint        & cast(u32)(bitmask6));
		inc = 4;
	}
	else{
		str[0] = '?';
		inc = 1;
	}
	return inc;
}

proc s32
utf16_encode(u16 *str, u32 codepoint) {
	s32 inc = 1;
	if (codepoint == max_u32){
		str[0] = (u16)'?';
	}
	else if (codepoint < 0x10000){
		str[0] = (u16)codepoint;
	}
	else{
		u32 v = codepoint - 0x10000;
		str[0] = safe_cast_u16(0xd800 + (v >> 10));
		str[1] = safe_cast_u16(0xdc00 + (v & cast(u32)(bitmask10)));
		inc = 2;
	}
	return inc;
}

proc s32
utf8_from_utf32_single(u8 *buffer, u32 character) {
	return utf8_encode(buffer, character);
}


//--------------------------------------------------------------------------------
// unicode str conversions

proc string8
str_from_str16(Arena *arena, string16 in) {
	assert(in.size >= 0);
	string8 result = str_zero();
	if(in.size)
	{
		s64 cap = in.size*3;
		u8 *str = push_array_no_zero(arena, u8, cap + 1);
		u16 *ptr = in.m;
		u16 *opl = ptr + in.size;
		s64 size = 0;
		Unicode_Decode consume;
		for(;ptr < opl; ptr += consume.inc)
		{
			consume = utf16_decode(ptr, opl - ptr);
			size += utf8_encode(str + size, consume.codepoint);
		}
		str[size] = 0;
		arena_pop(arena, (cap - size));
		result = make_str(str, size);
	}
	return result;
}

proc string16
str16_from_str(Arena *arena, string8 in) {
	assert(in.size >= 0);
	string16 result = str16_zero();
	if(in.size)
	{
		s64 cap = in.size*2;
		u16 *str = push_array_no_zero(arena, u16, cap + 1);
		u8 *ptr = in.m;
		u8 *opl = ptr + in.size;
		s64 size = 0;
		Unicode_Decode consume;
		for(;ptr < opl; ptr += consume.inc)
		{
			consume = utf8_decode(ptr, opl - ptr);
			size += utf16_encode(str + size, consume.codepoint);
		}
		str[size] = 0;
		arena_pop(arena, (cap - size)*2);
		result = make_str16(str, size);
	}
	return result;
}

proc string8
str_from_str32(Arena *arena, string32 in) {
	assert(in.size >= 0);
	string8 result = str_zero();
	if (in.size) {
		s64 cap = in.size*4;
		u8 *str = push_array_no_zero(arena, u8, cap + 1);
		u32 *ptr = in.m;
		u32 *opl = ptr + in.size;
		s64 size = 0;
		for (;ptr < opl; ptr += 1) {
			size += utf8_encode(str + size, *ptr);
		}
		str[size] = 0;
		arena_pop(arena, (cap - size));
		result = make_str(str, size);
	}
	return result;
}

proc string32
str32_from_str(Arena *arena, string8 in) {
	assert(in.size >= 0);
	string32 result = str32_zero(); 
	if (in.size) {
		s64 cap = in.size;
		u32 *str = push_array_no_zero(arena, u32, cap + 1);
		u8 *ptr = in.m;
		u8 *opl = ptr + in.size;
		s64 size = 0;
		Unicode_Decode consume;
		for(;ptr < opl; ptr += consume.inc) {
			consume = utf8_decode(ptr, opl - ptr);
			str[size] = consume.codepoint;
			size += 1;
		}
		str[size] = 0;
		arena_pop(arena, (cap - size)*4);
		result = make_str32(str, size);
	}
	return result;
}


//--------------------------------------------------------------------------------
// text indenting

proc string8
indented_from_string(Arena *arena, string8 str, $indent_params params) {
	assert(str.size >= 0);
	read_only local_persist u8 spaces[] = "                                                                                                                                                                                                                                                              ";
	read_only local_persist u8 tabs[]   = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
	read_only local_persist u8 *table[$indent_kind_count] = { spaces, tabs };

	Temp scratch = scratch_begin(&arena, 1);
	string_list list = zero_struct;

	s64 depth = 0;
	s64 next_depth = 0;
	s64 line_start = 0;
	s64 ending_scopes_on_line = 0;
	for (s64 i = 0; i < str.size; i += 1) {
		u8 c = str.m[i];
		assert(c != '\r'); // this proc assumes no carriage returns

		switch (c) {
			case /*open brace */ 0x7b:
				next_depth += 1;
				break;
			case /*close brace */ 0x7d:
				next_depth = clamp_bot(0, next_depth-1);
				ending_scopes_on_line += 1;
				depth = (ending_scopes_on_line == 1) ? next_depth : depth;
				break;
		}

		if (c == '\n') {
			rng1s64 rng = r1s64(line_start, i);
			string8 line = str_skip_chop_whitespace(str_substr(str, rng));
			if (line.size > 0) {
				s64 total_indent = depth*(s64)params.width;
				for (;;) {
					if (total_indent <= 0) break;
					s64 this_indent = min(total_indent, array_count(spaces));
					str_list_push(scratch.arena, &list, push_strf(scratch.arena, "%.*s", cast(int)(this_indent), table[params.kind]));
					total_indent -= this_indent;
				}
				string8 indented = push_strf(scratch.arena, "%.*s\n", svarg(line));
				str_list_push(scratch.arena, &list, indented);
			}
			if (line.size == 0) {
				str_list_push(scratch.arena, &list, S("\n"));
			}

			line_start = i+1;
			depth = next_depth;
			ending_scopes_on_line = 0;
		}
	}

	if (line_start < str.size) {
		rng1s64 rng = r1s64(line_start, str.size);
		string8 line = str_skip_chop_whitespace(str_substr(str, rng));

		s64 total_indent = depth*(s64)params.width;
		for (;;) {
			if (total_indent <= 0) break;
			s64 this_indent = min(total_indent, array_count(spaces));
			str_list_push(scratch.arena, &list, push_strf(scratch.arena, "%.*s", cast(int)this_indent, table[params.kind]));
			total_indent -= this_indent;
		}
		string8 indented = push_strf(scratch.arena, "%.*s\n", svarg(line));
		str_list_push(scratch.arena, &list, indented);
	}

	string8 result = str_list_join(arena, &list, 0);
	scratch_end(scratch);
	return result;
}

//--------------------------------------------------------------------------------
// text escaping

proc string8
escaped_from_raw_str(Arena *arena, string8 str) {
	assert(str.size >= 0);
	Temp scratch = scratch_begin(&arena, 1);
	u8 *working_mem = push_array_no_zero(scratch.arena, u8, str.size*2);
	s64 idx = 0;
	for (s64 i = 0; i < str.size; i += 1) {
		assert(idx+1 < str.size*2);
		u8 c = str.m[i];
		switch (c) {
		case 0: break;
		case '\a':
			working_mem[idx] = '\\';
			working_mem[idx+1] = 'a';
			idx += 2;
			break;
		case '\b': 
			working_mem[idx] = '\\';
			working_mem[idx+1] = 'b';
			idx += 2;
			break;
		case '\f': 
			working_mem[idx] = '\\';
			working_mem[idx+1] = 'f';
			idx += 2;
			break;
		case '\n': 
			working_mem[idx] = '\\';
			working_mem[idx+1] = 'n';
			idx += 2;
			break;
		case '\r': 
			working_mem[idx] = '\\';
			working_mem[idx+1] = 'r';
			idx += 2;
			break;
		case '\t': 
			working_mem[idx] = '\\';
			working_mem[idx+1] = 't';
			idx += 2;
			break;
		case '\v': 
			working_mem[idx] = '\\';
			working_mem[idx+1] = 'v';
			idx += 2;
			break;
		case '\\': 
			working_mem[idx] = '\\';
			working_mem[idx+1] = '\\';
			idx += 2;
			break;
		case '"': 
			working_mem[idx] = '\\';
			working_mem[idx+1] = '"';
			idx += 2;
			break;
		case '?': 
			working_mem[idx] = '\\';
			working_mem[idx+1] = '?';
			idx += 2;
			break;
		default:
			working_mem[idx] = c;
			idx += 1;
			break;
		}
	}

	s64 size = idx;
	u8 *mem = push_array_no_zero(arena, u8, size + 1);
	memory_copy(mem, working_mem, size);
	mem[size] = 0;
	scratch_end(scratch);
	return make_str(mem, size);
}

proc string8
raw_from_escaped_str(Arena *arena, string8 str) {
	assert(str.size >= 0);
	Temp scratch = scratch_begin(&arena, 1);
	u8 *working_mem = push_array_no_zero(scratch.arena, u8, str.size*2);

	s64 idx = 0;
	for (s64 i = 0; i < str.size; i += 1) {
		u8 c = str.m[i];
		u8 next = (i+1 < str.size) ? str.m[i+1] : 0;
		if (c == '\\' && next != 0) {
			switch (next) {
			case 0: break;

			case 'a':
				working_mem[idx] = '\a';
				idx += 1;
				i += 1;
				break;
			case 'b':
				working_mem[idx] = '\b';
				idx += 1;
				i += 1;
				break;
			case 'f':
				working_mem[idx] = '\f';
				idx += 1;
				i += 1;
				break;
			case 'n': 
				working_mem[idx] = '\n';
				idx += 1;
				i += 1;
				break;
			case 'r':
				working_mem[idx] = '\r';
				idx += 1;
				i += 1;
				break;
			case 't':
				working_mem[idx] = '\t';
				idx += 1;
				i += 1;
				break;
			case 'v':
				working_mem[idx] = '\v';
				idx += 1;
				i += 1;
				break;
			case '\\': 
				working_mem[idx] = '\\';
				idx += 1;
				i += 1;
				break;
			case '"':
				working_mem[idx] = '"';
				idx += 1;
				i += 1;
				break;
			case '?': 
				working_mem[idx] = '?';
				idx += 1;
				i += 1;
				break;

			default:
				working_mem[idx] = '\\';
				working_mem[idx+1] = next;
				idx += 2;
				i += 1;
				break;
			}
		} else {
			working_mem[idx] = c;
			idx += 1;
		}
	}

	s64 size = idx;
	u8 *mem = push_array_no_zero(arena, u8, size + 1);
	memory_copy(mem, working_mem, size);
	mem[size] = 0;

	scratch_end(scratch);
	return make_str(mem, size);
}

//--------------------------------------------------------------------------------
// str <-> color

proc string8
hex_string_from_rgba_4f32(Arena *arena, vec4f32 rgba) {
	string8 hex_string = push_strf(arena, "%02x%02x%02x%02x", (u8)(rgba.x*255.f), (u8)(rgba.y*255.f), (u8)(rgba.z*255.f), (u8)(rgba.w*255.f));
	return hex_string;
}

proc vec4f32
rgba_from_hex_string_4f32(string8 hex_string) {
	assert(hex_string.size >= 0);
	u8 byte_text[8] = {0};
	s64 byte_text_idx = 0;
	for(s64 idx = 0; idx < hex_string.size && byte_text_idx < array_count(byte_text); idx += 1)
	{
		if(char_is_digit(hex_string.m[idx], 16))
		{
			byte_text[byte_text_idx] = char_to_lower(hex_string.m[idx]);
			byte_text_idx += 1;
		}
	}
	u8 byte_vals[4] = {0};
	for(s64 idx = 0; idx < 4; idx += 1) {
		byte_vals[idx] = (u8)u64_from_str(make_str(&byte_text[idx*2], 2), 16);
	}
	vec4f32 rgba = v4f32(byte_vals[0]/255.f, byte_vals[1]/255.f, byte_vals[2]/255.f, byte_vals[3]/255.f);
	return rgba;
}

//--------------------------------------------------------------------------------
// str fuzzy matching
proc Fuzzy_Match_Range_List
fuzzy_match_find(Arena *arena, string8 needle, string8 haystack) {
	assert(needle.size >= 0 && haystack.size >= 0);
	Fuzzy_Match_Range_List result = {0};
	Temp scratch = scratch_begin(&arena, 1);
	string_list needles = str_split(scratch.arena, needle, (u8*)" ", 1, 0);
	result.needle_part_count = needles.node_count;
	for (each_node(needle_n, needles.head, string_node)) {
		s64 find_pos = 0;
		for(;find_pos < haystack.size;) {
			find_pos = str_find_needle(find_pos, needle_n->str, haystack, String_Match_Flag_Case_Insensitive);
			b32 is_in_gathered_ranges = 0;
			for (each_node(n, result.head, Fuzzy_Match_Range_Node)) {
				if(n->range.min <= find_pos && find_pos < n->range.max) {
					is_in_gathered_ranges = 1;
					find_pos = n->range.max;
					break;
				}
			}
			if(!is_in_gathered_ranges) {
				break;
			}
		}
		if(find_pos < haystack.size) {
			rng1s64 range = r1s64(find_pos, find_pos + needle_n->str.size);
			Fuzzy_Match_Range_Node *node = push_array(arena, Fuzzy_Match_Range_Node, 1);
			node->range = range;

			Fuzzy_Match_Range_Node **ptr = check_nil(result.head, nil) ? &result.head : &result.tail->next;
			*ptr = node; result.tail = node;
			node->next = nil;
			result.count += 1;
			result.total_dim += dim_1s64(range);
		}
	}
	scratch_end(scratch);
	return result;
}

proc Fuzzy_Match_Range_List
fuzzy_match_range_list_copy(Arena *arena, Fuzzy_Match_Range_List *src) {
	Fuzzy_Match_Range_List dst = zero_struct;
	for (each_node(node, src->head, Fuzzy_Match_Range_Node)) {
		Fuzzy_Match_Range_Node *dstn = push_array(arena, Fuzzy_Match_Range_Node, 1);

		Fuzzy_Match_Range_Node **ptr = check_nil(dst.head, nil) ? &dst.head : &dst.tail->next;
		*ptr = dstn; dst.tail = dstn;
		dstn->next = nil;
		dstn->range = node->range;
	}
	dst.count = src->count;
	dst.needle_part_count = src->needle_part_count;
	dst.total_dim = src->total_dim;
	return dst;
}
