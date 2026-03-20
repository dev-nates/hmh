
//--------------------------------------------------------------------------------
// input buttons

proc b32
ibutton_pressed(IButton button)
{
	b32 in_order = (button.down == (button.htc&1));
	b32 result = (in_order) ? (button.htc >= 1) : (button.htc >= 2);
	return result;
}

proc b32
ibutton_released(IButton button)
{
	b32 in_order = (button.down == (button.htc&1));
	b32 result = (in_order) ? (button.htc >= 2) : (button.htc >= 1);
	return result;
}

proc b32
ibutton_down(IButton button)
{
	return button.down;
}

//--------------------------------------------------------------------------------
// time procs

proc Dense_Time
dense_time_from_date_time(Date_Time date_time) {
	Dense_Time result = 0;
	result += date_time.year;
	result *= 12;
	result += date_time.mon;
	result *= 31;
	result += date_time.day;
	result *= 24;
	result += date_time.hour;
	result *= 60;
	result += date_time.min;
	result *= 61;
	result += date_time.sec;
	result *= 1000;
	result += date_time.msec;
	return result;
}

proc Date_Time
date_time_from_dense_time(Dense_Time time) {
  Date_Time result = zero_struct;
  result.msec = time%1000;
  time /= 1000;
  result.sec  = time%61;
  time /= 61;
  result.min  = time%60;
  time /= 60;
  result.hour = time%24;
  time /= 24;
  result.day  = time%31;
  time /= 31;
  result.mon  = time%12;
  time /= 12;
  assert(time <= max_u32);
  result.year = (u32)time;
  return result;
}

proc Date_Time
date_time_from_micro_seconds(u64 time) {
	Date_Time result = zero_struct;
	result.usec = time%1000;
	time /= 1000;
	result.msec = time%1000;
	time /= 1000;
	result.sec = time%61;
	time /= 61;
	result.min = time%60;
	time /= 60;
	result.hour = time%24;
	time /= 24;
	result.day = time%31;
	time /= 31;
	result.mon = time%12;
	time /= 12;
	assert(time <= max_u32);
	result.year = (u32)time;
	return result;
}

proc Date_Time
date_time_from_unix_time(u64 unix_time) {
  Date_Time date = {0};
  date.year     = 1970;
  date.day      = (u16)(1 + (unix_time / 86400));
  date.sec      = (u32)unix_time % 60;
  date.min      = (u32)(unix_time / 60) % 60;
  date.hour     = (u32)(unix_time / 3600) % 24;

  for(;;)
  {
    for(date.month = (Month)(0); date.month < 12; ++date.month)
    {
      u64 c = 0;
      switch(date.month)
      {
        case Month_Jan: c = 31; break;
        case Month_Feb:
        {
          if((date.year % 4 == 0) && ((date.year % 100) != 0 || (date.year % 400) == 0))
          {
            c = 29;
          }
          else
          {
            c = 28;
          }
        } break;
        case Month_Mar: c = 31; break;
        case Month_Apr: c = 30; break;
        case Month_May: c = 31; break;
        case Month_Jun: c = 30; break;
        case Month_Jul: c = 31; break;
        case Month_Aug: c = 31; break;
        case Month_Sep: c = 30; break;
        case Month_Oct: c = 31; break;
        case Month_Nov: c = 30; break;
        case Month_Dec: c = 31; break;
        default: invalid_path;
      }
      if(date.day <= c)
      {
        goto exit;
      }
      date.day -= c;
    }
    ++date.year;
  }
  exit:;

  return date;
}

//--------------------------------------------------------------------------------
// safe casts

proc u16
safe_cast_u16(u32 x) {
	assert_always(x <= max_u16);
	return (u16)x;
}
proc u32
safe_cast_u32(u64 x) {
	assert_always(x <= max_u32);
	return (u32)x;
}

proc s16
safe_cast_s16(s32 x) {
	assert_always(min_s16 <= x && x <= max_s16);
	return (s16)x;
}

proc s32
safe_cast_s32(s64 x) {
	assert_always(min_s32 <= x && x <= max_s32);
	return (s32)x;
}

//--------------------------------------------------------------------------------
// large base type functions

proc u128
u128_zero(void) {
	u128 result = zero_struct;
	return result;
}

proc u128
u128_make(u64 v0, u64 v1) {
	u128 result = {
		.v[0] = v0,
		.v[1] = v1,
	};
	return result;
}

proc b32
u128_match(u128 a, u128 b) {
	b32 result = (a.v[0]==b.v[0]) && (a.v[1]==b.v[1]);
	return result;
}

//--------------------------------------------------------------------------------
// bit patterns

proc u32
u32_from_u64_saturate(u64 x) {
	return (x>max_u32) ? max_u32 : (u32)x;
}

proc u64
u64_up_to_pow2(u64 x) {
  if (x == 0){
    x = 1;
  }
  else{
    x -= 1;
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    x |= (x >> 32);
    x += 1;
  }
  return(x);
}

proc s32
extend_sign32(u32 x, u32 size) {
	size = clamp_top(size, 4);
  u32 high_bit = size * 8;
  u32 shift = 32 - high_bit;
  s32 result = ((s32)x << shift) >> shift;
  return result;
}
proc s64
extend_sign64(u64 x, u64 size) {
	size = clamp_top(size, 8);
  u64 high_bit = size * 8;
  u64 shift = 64 - high_bit;
  s64 result = ((s64)x << shift) >> shift;
  return result;
}

proc f32
inf32(void) {
	union { u32 u; f32 f; } x;
	x.u = exponent32;
	return x.f;
}
proc f32
neg_inf32(void) {
	union { u32 u; f32 f; } x;
	x.u = sign32 | exponent32;
	return x.f;
}

proc u16
bswap_u16(u16 x) {
	u16 result = (u16)((x & 0xff00) >> 8) | (u16)((x & 0x00ff) << 8);
	return result;
}
proc u32
bswap_u32(u32 x) {
	u32 result =
		((x & 0xff000000) >> 24) |
		((x & 0x00ff0000) >> 8) |
		((x & 0x0000ff00) << 8) |
		((x & 0x000000ff) << 24);
	return result;
}
proc u64
bswap_u64(u64 x) {
	u64 result =
		((x & 0xff00000000000000ull) >> 56) |
		((x & 0x00ff000000000000ull) >> 40) |
		((x & 0x0000ff0000000000ull) >> 24) |
		((x & 0x000000ff00000000ull) >> 8)  |
		((x & 0x00000000000000ffull) << 56) |
		((x & 0x000000000000ff00ull) << 40) |
		((x & 0x0000000000ff0000ull) << 24) |
		((x & 0x00000000ff000000ull) << 8);
	return result;
}

#if ARCH_LITTLE_ENDIAN
# define from_be_u16(x) bswap_u16(x)
# define from_be_u32(x) bswap_u32(x)
# define from_be_u64(x) bswap_u64(x)
#else
# define from_be_u16(x) (x)
# define from_be_u32(x) (x)
# define from_be_u64(x) (x)
#endif

#if COMPILER_MSVC || (COMPILER_CLANG && OS_WINDOWS)

proc u64
count_bits_set32(u32 val) {
  return __popcnt(val);
}

proc u64
count_bits_set64(u64 val) {
  return __popcnt64(val);
}

proc u64
ctz32(u32 mask) {
  unsigned long idx;
  _BitScanForward(&idx, mask);
  return idx;
}

proc u64
ctz64(u64 mask) {
  unsigned long idx;
  _BitScanForward64(&idx, mask);
  return idx;
}

proc u64
clz32(u32 mask) {
  unsigned long idx;
  _BitScanReverse(&idx, mask);
  return 31 - idx;
}

proc u64
clz64(u64 mask) {
  unsigned long idx;
  _BitScanReverse64(&idx, mask);
  return 63 - idx;
}

-----------------------------------------------------------------------
// rot bits
# error "TODO"
proc u32 rotl32(u32 val, u32 amt);
proc u64 rotl64(u64 val, u64 amt);
proc u32 rotr32(u32 val, u32 amt);
proc u64 rotr64(u64 val, u64 amt);

//--------------------------------------------------------------------------------
// cycles
proc u64 cpu_cycles();

#elif COMPILER_CLANG || COMPILER_GCC

proc u32
count_bits_set32(u32 val) {
  return (u32)__builtin_popcount(val);
}

proc u64
count_bits_set64(u64 val) {
  return (u64)__builtin_popcountll(val);
}

proc u32
ctz32(u32 val) {
  return (u32)__builtin_ctz(val);
}

proc u32
clz32(u32 val) {
  return (u32)__builtin_clz(val);
}

proc u64
ctz64(u64 val) {
  return (u64)__builtin_ctzll(val);
}

proc u64
clz64(u64 val) {
  return (u64)__builtin_clzll(val);
}

//--------------------------------------------------------------------------------
// rot bits
proc u32
rotl32(u32 val, u32 amt) {
	return __builtin_rotateleft32(val, amt);
}

proc u64
rotl64(u64 val, u64 amt) {
	return __builtin_rotateleft64(val, amt);
}

proc u32
rotr32(u32 val, u32 amt) {
	return __builtin_rotateright32(val, amt);
}

proc u64
rotr64(u64 val, u64 amt) {
	return __builtin_rotateright64(val, amt);
}

//--------------------------------------------------------------------------------
// cycles
proc u64
cpu_cycles() {
	return __rdtsc();
}
#else
# error "Bit intrinsic functions not defined for this compiler."
#endif


//--------------------------------------------------------------------------------
// enum -> sign

proc s32
sign_from_side_s32(Side side) {
	return (side == Side_Min) ? -1 : 1;
}
proc f32
sign_from_side_f32(Side side) {
	return (side == Side_Min) ? -1.f : 1.f;
}

//--------------------------------------------------------------------------------
// memory functions

proc b32
memory_is_zero(rawptr ptr, s64 size) {
	assert(size >= 0);
	b32 result = 1;

	s64 leftover_cnt = (size & 0x7);
	s64 u64_cnt = (size >> 3);

	u64 *p64 = (u64 *)ptr;
	for (s64 i = 0; i < u64_cnt; i += 1, p64 += 1) {
		if (*p64 != 0) {
			result = 0;
			goto done;
		}
	}

	if (result) {
		u8 *p8 = (u8 *)p64;
		for (s64 i = 0; i < leftover_cnt; i += 1, p8 += 1) {
			if (*p8 != 0) {
				result = 0;
				goto done;
			}
		}
	}

	done:;
	return result;
}

//--------------------------------------------------------------------------------
// text 2d coordinate range functions

proc Loc
make_loc(s64 line, s64 col) {
	return (Loc) { .line = line, .col = col };
}

proc b32
loc_match(Loc a, Loc b) {
	return (a.line == b.line) && (a.col == b.col);
}

proc b32
loc_less_than(Loc a, Loc b) {
	b32 result = (a.line < b.line) ? 1 : (a.col < b.col) ? 1 : 0;
	return result;
}

proc Loc
loc_min(Loc a, Loc b) {
	Loc result = (loc_less_than(a, b)) ? a : b;
	return result;
}

proc Loc
loc_max(Loc a, Loc b) {
	Loc result = (loc_less_than(a, b)) ? b : a;
	return result;
}

proc Loc_Range
make_loc_rng(Loc min, Loc max) {
	Loc_Range result;
	if (loc_less_than(min, max)) {
		result.min = min;
		result.max = max;
	} else {
		result.min = max;
		result.max = min;
	}
	return result;
}

proc Loc_Range
loc_rng_intersect(Loc_Range a, Loc_Range b) {
	Loc_Range result;
	result.max = loc_max(a.min, b.min);
	result.min = loc_min(a.max, b.max);
	if (loc_less_than(result.max, result.min)) {
		memory_zero_struct(&result);
	}
	return result;
}

proc Loc_Range
loc_rng_union(Loc_Range a, Loc_Range b) {
	Loc_Range result;
	result.min = loc_min(a.min, b.min);
	result.max = loc_max(a.max, b.max);
	return result;
}

proc b32
loc_rng_contains(Loc_Range r, Loc pt) {
	b32 result;
	result = (loc_less_than(r.min, pt) || loc_match(r.min, pt)) &&
		loc_less_than(pt, r.max);
	return result;
}


//--------------------------------------------------------------------------------
// non-virtual ring buffer read/write

proc s64
ring_write(rawptr ring_base, s64 ring_size, s64 ring_pos, rawptr src_data, s64 src_data_size) {
	if (ring_size == 0 || src_data_size == 0) return 0;
	assert(ring_size > 0); assert(src_data_size > 0); assert(src_data_size <= ring_size);

	s64 wrap_pos = ((ring_pos % ring_size) + ring_size) % ring_size;
	s64 remaining = (ring_size - wrap_pos);

	s64 end_chunk_size = min(remaining, src_data_size);
	rawptr end_chunk_base = (u8*)ring_base + wrap_pos;
	s64 start_chunk_size = src_data_size - end_chunk_size;
	rawptr start_chunk_base = ring_base;
	memory_copy(end_chunk_base, src_data, end_chunk_size);
	memory_copy(start_chunk_base, src_data + end_chunk_size, start_chunk_size);
	return src_data_size;
}

proc s64
ring_read(rawptr ring_base, s64 ring_size, s64 ring_pos, rawptr dst_data, s64 read_size) {
	if (ring_size == 0 || read_size == 0) return 0;
	assert(ring_size > 0); assert(read_size > 0);

	s64 wrap_pos = ((ring_pos % ring_size) + ring_size) % ring_size;
	s64 remaining = (ring_size - wrap_pos);

	rawptr pre_base = (u8*)ring_base + wrap_pos;
	s64 pre_size = min(read_size, remaining);

	s64 entire_ring_repeat_count = (read_size-pre_size)/ring_size;
	s64 pst_size = (read_size-pre_size) % ring_size;

	u8 *at = (u8*)dst_data;
	u8 *opl = (u8*)dst_data + read_size;

	memory_copy((rawptr)at, pre_base, pre_size);
	at += pre_size;
	for (s64 i = 0; i < entire_ring_repeat_count; ++i) {
		memory_copy((rawptr)at, ring_base, ring_size);
		at += ring_size;
	}
	memory_copy((rawptr)at, ring_base, pst_size);
	at += pst_size;

	assert(at == opl);
	return read_size;
}

