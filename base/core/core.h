#ifndef BASE_CORE_H
#define BASE_CORE_H

//--------------------------------------------------------------------------------
// foreign includes

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

//--------------------------------------------------------------------------------
// Intrinsics
#if COMPILER_MSVC
# include <intrin.h>
#elif COMPILER_CLANG || COMPILER_GCC
# include <x86intrin.h>
#endif

//--------------------------------------------------------------------------------
// Keywords

#define true 1
#define false 0
#define nil 0

// Used to separate out arguments in a procedure
#define _ret_

#define proc static
#define global static
#define local_persist static

#define cast(stmt) (stmt)

#if COMPILER_MSVC || (COMPILER_CLANG && OS_WINDOWS)
# pragma section(".rdata$", read)
# define read_only __declspec(allocate(".rdata$"))
#elif (COMPILER_CLANG && OS_LINUX)
# define read_only __attribute__((section(".rodata")))
#else
# define read_only
#endif

#if COMPILER_MSVC
# define per_thread __declspec(thread)
#elif COMPILER_CLANG || COMPILER_GCC
# define per_thread __thread
#endif

#if COMPILER_MSVC
# define force_inline __forceinline
#elif COMPILER_CLANG || COMPILER_GCC
# define force_inline __attribute__((always_inline))
#endif

#if OS_WINDOWS
# define shared_function c_linkage __declspec(dllexport)
#else
# define shared_function c_linkage
#endif

#if LANG_CPP
# define c_linkage_begin extern "C"{
# define c_linkage_end }
# define c_linkage extern "C"
#else
# define c_linkage_begin
# define c_linkage_end
# define c_linkage
#endif

//--------------------------------------------------------------------------------
// helper macros

#define stringify_(s) #s
#define stringify(s) stringify_(s)

#define glue_(a,b) a##b
#define glue(a,b) glue_(a,b)

#if DEBUG
# define stmnt(expr) do{expr}while(0)
#else
# define stmnt(expr) expr
#endif

#define size_of(expr)  (s64)sizeof(expr)
#define array_count(a) (size_of(a) / size_of((a)[0]))
#define expand_array_s64(a) (a), array_count(a)
#define expand_array_s32(a) (a), (s32)array_count(a)
#define expand_array(a) expand_array_s64(a)

#define ceil_integer_div(a,b) (((a) + (b) - 1)/(b))

#define swap(T,a,b) stmnt(T t__ = a; a = b; b = t__;)

#if ARCH_64BIT
# define int_from_ptr(ptr) ((u64)(ptr))
# define ptr_from_int(i) (rawptr)((u64)i)
#elif ARCH_32BIT
# define int_from_ptr(ptr) ((u32)(ptr))
# define ptr_from_int(i) (rawptr)((u32)i)
#else
# error Missing pointer-to-integer cast for this architecture.
#endif

#define compose_64_bit(a,b)  ((((u64)a) << 32) | ((u64)b));
#define align_pow2(x,b)     (((x) + (b) - 1)&(~((b) - 1)))
#define align_down_pow2(x,b) ((x)&(~((b) - 1)))
#define align_pad_pow2(x,b)  ((0-(x)) & ((b) - 1))
#define is_pow2(x)          ((x)!=0 && ((x)&((x)-1))==0)
#define is_pow2_or_zero(x)    ((((x) - 1)&(x)) == 0)

#define extract_bit(word, idx) (((word) >> (idx)) & 1)

#if LANG_CPP
# define zero_struct {}
#else
# define zero_struct {0}
#endif

#if COMPILER_CLANG
# define this_function_name __func__
#elif COMPILER_GCC
# define this_function_name __func__
#else
# error this_function_name undefined for this compiler
#endif

#define this_file_name __FILE__
#define this_line_number __LINE__

//--------------------------------------------------------------------------------
// asserts

#if COMPILER_MSVC
# define debug_trap() __debugbreak()
#elif COMPILER_CLANG || COMPILER_GCC
# define debug_trap() __builtin_trap()
#else
# error Unknown trap intrinsic for this compiler.
#endif

#define ensure(x) do{if(!(x)) {debug_trap();}}while(0)
#if BUILD_DEBUG
# define assert(x) ensure(x)
#else
# define assert(x) (void)(x)
#endif
#define invalid_path assert(0)
#define not_implemented assert(0)
#define static_assert(C, ID) global u8 glue(ID, __LINE__)[(C)?1:-1]

//--------------------------------------------------------------------------------
// fallthrough

#if COMPILER_CLANG
# define fallthrough __attribute__((fallthrough))
#else
# define fallthrough
#endif

//--------------------------------------------------------------------------------
// noreturn

#if COMPILER_CLANG
# define noreturn __attribute__((noreturn))
#else
# define noreturn
#endif

// -------------------------------------------------------------------------------------------------
// Abort
#define abort_program(status) _exit(status)

// -------------------------------------------------------------------------------------------------
// Format
#if COMPILER_CLANG
#define CLANG_FORMAT_WARN(start, end) __attribute__((__format__(__printf__, start, end)))
#else
#define CLANG_FORMAT_WARN(start, end)
#endif

//--------------------------------------------------------------------------------
// warnings

#if COMPILER_CLANG
# define disable_warnings_begin _Pragma("clang diagnostic push") \
	_Pragma("clang diagnostic ignored \"-Weverything\"")
#define disable_warnings_end _Pragma("clang diagnostic pop")
#elif COMPILER_GCC
# define disable_warnings_begin
#define disable_warnings_end
#else
# error "warnings macro not defined for this compiler"
#endif

//--------------------------------------------------------------------------------
// address sanitizer

#if COMPILER_MSVC
# if defined(__SANITIZE_ADDRESS__)
#  define ASAN_ENABLED 1
#  define NO_ASAN __declspec(no_sanitize_address)
# else
#  define NO_ASAN
# endif
#elif COMPILER_CLANG
# if defined(__has_feature)
#  if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
#   define ASAN_ENABLED 1
#  endif
# endif
# define NO_ASAN __attribute__((no_sanitize("address")))
#else
# define NO_ASAN
#endif

#if ASAN_ENABLED
#pragma comment(lib, "clang_rt.asan-x86_64.lib")
c_linkage void __asan_poison_memory_region(void const volatile *addr, size_t size);
c_linkage void __asan_unpoison_memory_region(void const volatile *addr, size_t size);
# define asan_poison(addr, size)   __asan_poison_memory_region((addr), (size))
# define asan_cure(addr, size) __asan_unpoison_memory_region((addr), (size))
#else
# define asan_poison(addr, size)   ((void)(addr), (void)(size))
# define asan_cure(addr, size) ((void)(addr), (void)(size))
#endif

//--------------------------------------------------------------------------------
// type -> alignment

#if COMPILER_MSVC
# define align_of(T) __alignof(T)
#elif COMPILER_CLANG
# define align_of(T) __alignof(T)
#elif COMPILER_GCC
# define align_of(T) __alignof__(T)
#else
# error "align_of not defined for this compiler."
#endif

//--------------------------------------------------------------------------------
// branch predictor hints

#if defined(__clang__)
# define branch_expect(expr, val) __builtin_expect((expr), (val))
#else
# define branch_expect(expr, val) (expr)
#endif

#define branch_likely(expr)            branch_expect(expr,1)
#define branch_unlikely(expr)          branch_expect(expr,0)

//--------------------------------------------------------------------------------
// types

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t s64;
typedef int32_t s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef int64_t b64;
typedef int32_t b32;
typedef int16_t b16;
typedef int8_t  b8;

typedef float f32;
typedef double f64;

typedef u8*  cstring;
typedef u16* cstring16;
typedef u32* cstring32;
typedef void* rawptr;

typedef struct u128 u128;
struct u128 {
	u64 v[2];
};

typedef void rawproc(void);

//--------------------------------------------------------------------------------
// constants

global u32 sign32     = 0x80000000;
global u32 exponent32 = 0x7f800000;
global u32 mantissa32 = 0x007fffff;

global f32 big_golden32 = 1.61803398875f;
global f32 small_golden32 = 0.61803398875f;

global f32 pi32 = 3.1415926535897f;

global f64 machine_epsilon64 = 4.94065645841247e-324;

global u64 max_u64 = 0xffffffffffffffffull;
global u32 max_u32 = 0xffffffff;
global u16 max_u16 = 0xffff;
global u8  max_u8 = 0xff;

global s64 max_s64 = (s64)0x7fffffffffffffffull;
global s32 max_s32 = (s32)0x7fffffff;
global s16 max_s16 = (s16)0x7fff;
global s8  max_s8  =  (s8)0x7f;

global s64 min_s64 = (s64)0x8000000000000000ull;
global s32 min_s32 = (s32)0x80000000;
global s16 min_s16 = (s16)0x8000;
global s8  min_s8  =  (s8)0x80;

#define bitmask1 0x00000001
#define bitmask2 0x00000003
#define bitmask3 0x00000007
#define bitmask4 0x0000000f
#define bitmask5 0x0000001f
#define bitmask6 0x0000003f
#define bitmask7 0x0000007f
#define bitmask8 0x000000ff
#define bitmask9 0x000001ff
#define bitmask10 0x000003ff
#define bitmask11 0x000007ff
#define bitmask12 0x00000fff
#define bitmask13 0x00001fff
#define bitmask14 0x00003fff
#define bitmask15 0x00007fff
#define bitmask16 0x0000ffff
#define bitmask17 0x0001ffff
#define bitmask18 0x0003ffff
#define bitmask19 0x0007ffff
#define bitmask20 0x000fffff
#define bitmask21 0x001fffff
#define bitmask22 0x003fffff
#define bitmask23 0x007fffff
#define bitmask24 0x00ffffff
#define bitmask25 0x01ffffff
#define bitmask26 0x03ffffff
#define bitmask27 0x07ffffff
#define bitmask28 0x0fffffff
#define bitmask29 0x1fffffff
#define bitmask30 0x3fffffff
#define bitmask31 0x7fffffff
#define bitmask32 0xffffffff

#define bitmask33 0x00000001ffffffffull
#define bitmask34 0x00000003ffffffffull
#define bitmask35 0x00000007ffffffffull
#define bitmask36 0x0000000fffffffffull
#define bitmask37 0x0000001fffffffffull
#define bitmask38 0x0000003fffffffffull
#define bitmask39 0x0000007fffffffffull
#define bitmask40 0x000000ffffffffffull
#define bitmask41 0x000001ffffffffffull
#define bitmask42 0x000003ffffffffffull
#define bitmask43 0x000007ffffffffffull
#define bitmask44 0x00000fffffffffffull
#define bitmask45 0x00001fffffffffffull
#define bitmask46 0x00003fffffffffffull
#define bitmask47 0x00007fffffffffffull
#define bitmask48 0x0000ffffffffffffull
#define bitmask49 0x0001ffffffffffffull
#define bitmask50 0x0003ffffffffffffull
#define bitmask51 0x0007ffffffffffffull
#define bitmask52 0x000fffffffffffffull
#define bitmask53 0x001fffffffffffffull
#define bitmask54 0x003fffffffffffffull
#define bitmask55 0x007fffffffffffffull
#define bitmask56 0x00ffffffffffffffull
#define bitmask57 0x01ffffffffffffffull
#define bitmask58 0x03ffffffffffffffull
#define bitmask59 0x07ffffffffffffffull
#define bitmask60 0x0fffffffffffffffull
#define bitmask61 0x1fffffffffffffffull
#define bitmask62 0x3fffffffffffffffull
#define bitmask63 0x7fffffffffffffffull
#define bitmask64 0xffffffffffffffffull

#define bit1  (1<<0)
#define bit2  (1<<1)
#define bit3  (1<<2)
#define bit4  (1<<3)
#define bit5  (1<<4)
#define bit6  (1<<5)
#define bit7  (1<<6)
#define bit8  (1<<7)
#define bit9  (1<<8)
#define bit10 (1<<9)
#define bit11 (1<<10)
#define bit12 (1<<11)
#define bit13 (1<<12)
#define bit14 (1<<13)
#define bit15 (1<<14)
#define bit16 (1<<15)
#define bit17 (1<<16)
#define bit18 (1<<17)
#define bit19 (1<<18)
#define bit20 (1<<19)
#define bit21 (1<<20)
#define bit22 (1<<21)
#define bit23 (1<<22)
#define bit24 (1<<23)
#define bit25 (1<<24)
#define bit26 (1<<25)
#define bit27 (1<<26)
#define bit28 (1<<27)
#define bit29 (1<<28)
#define bit30 (1<<29)
#define bit31 (1<<30)
#define bit32 ((u32)1<<(u32)31)

#define bit33 (1ull<<32)
#define bit34 (1ull<<33)
#define bit35 (1ull<<34)
#define bit36 (1ull<<35)
#define bit37 (1ull<<36)
#define bit38 (1ull<<37)
#define bit39 (1ull<<38)
#define bit40 (1ull<<39)
#define bit41 (1ull<<40)
#define bit42 (1ull<<41)
#define bit43 (1ull<<42)
#define bit44 (1ull<<43)
#define bit45 (1ull<<44)
#define bit46 (1ull<<45)
#define bit47 (1ull<<46)
#define bit48 (1ull<<47)
#define bit49 (1ull<<48)
#define bit50 (1ull<<49)
#define bit51 (1ull<<50)
#define bit52 (1ull<<51)
#define bit53 (1ull<<52)
#define bit54 (1ull<<53)
#define bit55 (1ull<<54)
#define bit56 (1ull<<55)
#define bit57 (1ull<<56)
#define bit58 (1ull<<57)
#define bit59 (1ull<<58)
#define bit60 (1ull<<59)
#define bit61 (1ull<<60)
#define bit62 (1ull<<61)
#define bit63 (1ull<<62)
#define bit64 (1ull<<63)

// -------------------------------------------------------------------------------------------------
// Code location
typedef struct Code_Location Code_Location;
struct Code_Location {
	cstring filename;
	cstring procedure;
	s64 line;
};
#define code_location (Code_Location){this_file_name, this_function_name, this_line_number}

//--------------------------------------------------------------------------------
// basic types

//--------------------------------------------------------------------------------
// axis, side, corner, dir2

typedef enum Axis2 Axis2;
enum Axis2 {
	Axis2_Invalid = -1,
	Axis2_X,
	Axis2_Y,
	Axis2_COUNT,
};
#define axis2_flip(a) ((Axis2)(!(a)))

typedef enum Axis3 Axis3;
enum Axis3 {
	Axis3_Invalid = -1,
	Axis3_X,
	Axis3_Y,
	Axis3_Z,
	Axis3_COUNT,
};

typedef enum Side Side;
enum Side {
	Side_Invalid = -1,
	Side_Min,
	Side_Max,
	Side_COUNT,
};
#define side_flip(s) ((Sides)(!(s)))

typedef enum Corner Corner;
enum Corner {
	Corner_Invalid = -1,
	Corner_00,
	Corner_01,
	Corner_10,
	Corner_11,
	Corner_COUNT,
};

typedef enum Dir2 Dir2;
enum Dir2 {
	Dir2_Invalid = -1,
	Dir2_Left,
	Dir2_Up,
	Dir2_Right,
	Dir2_Down,
	Dir2_COUNT,
};
#define axis_from_dir2(d) (((d)&1) ? Axis2_Y : Axis2_X)
#define side_from_dir2(d) (((d) < Dir2_Right) ? Side_Min : Side_Max)

//--------------------------------------------------------------------------------
// loc 2d txt coordinates

typedef struct Loc Loc;
struct Loc {
	s64 line;
	s64 col;
};

typedef struct Loc_Range Loc_Range;
struct Loc_Range {
	Loc min;
	Loc max;
};

//--------------------------------------------------------------------------------
// Date

typedef enum Week_Day Week_Day;
enum Week_Day {
	Week_Day_Mon,
	Week_Day_Tue,
	Week_Day_Wen,
	Week_Day_Thu,
	Week_Day_Fri,
	Week_Day_Sat,
	Week_Day_Sun,
	Week_Day_Count,
};

typedef enum Month Month;
enum Month {
	Month_Jan,
	Month_Feb,
	Month_Mar,
	Month_Apr,
	Month_May,
	Month_Jun,
	Month_Jul,
	Month_Aug,
	Month_Sep,
	Month_Oct,
	Month_Nov,
	Month_Dec,
};


typedef struct Date_Time Date_Time;
struct Date_Time {
	u16 usec; // [0,999]
	u16 msec; // [0,999]
	u16 sec; // [0,60]
	u16 min; // [0,59]
	u16 hour; // [0,24]
	u16 day; // [0,30]
	union {
		Week_Day week_day;
		u32 wday;
	};
	union {
		Month month;
		u32 mon;
	};
	u32 year; // 1 = 1ec, 0 = 1 bc
};
typedef u64 Dense_Time;

proc Dense_Time dense_time_from_date_time(Date_Time date_time);
proc Date_Time date_time_from_dense_time(Dense_Time time);
proc Date_Time date_time_from_micro_seconds(u64 time);
proc Date_Time date_time_from_unix_time(u64 unix_time);

//--------------------------------------------------------------------------------
// file properties

typedef enum File_Property_Flags File_Property_Flags;
enum File_Property_Flags {
	File_Property_Flag_Is_Folder = bit1,
};

typedef struct File_Properties File_Properties;
struct File_Properties {
	Dense_Time created;
	Dense_Time modified;
	s64 size;
	File_Property_Flags flags;
};

//--------------------------------------------------------------------------------
// safe casts

proc u16 safe_cast_u16(u32 x);
proc u32 safe_cast_u32(u64 x);
proc s32 safe_cast_s32(s64 x);

//--------------------------------------------------------------------------------
// large base type functions

proc u128 u128_zero(void);
proc u128 u128_make(u64 v0, u64 v1);
proc b8   u128_match(u128 a, u128 b);

//--------------------------------------------------------------------------------
// bit patterns

proc u32 u32_from_u64_saturate(u64 x);
proc u64 u64_up_to_pow2(u64 x);
proc s32 extend_sign32(u32 x, u32 size);
proc s64 extend_sign64(u64 x, u64 size);

proc f32 inf32(void);
proc f32 neg_inf32(void);

proc u16 bswap_u16(u16 x);
proc u32 bswap_u32(u32 x);
proc u64 bswap_u64(u64 x);

#if ARCH_LITTLE_ENDIAN
# define from_be_u16(x) bswap_u16(x)
# define from_be_u32(x) bswap_u32(x)
# define from_be_u64(x) bswap_u64(x)
#else
# define from_be_u16(x) (x)
# define from_be_u32(x) (x)
# define from_be_u64(x) (x)
#endif

proc u32 count_bits_set32(u32 val);
proc u64 count_bits_set64(u64 val);

proc u32 ctz32(u32 val);
proc u64 ctz64(u64 val);
proc u32 clz32(u32 val);
proc u64 clz64(u64 val);

//--------------------------------------------------------------------------------
// rot bits
proc u32 rotl32(u32 val, u32 amt);
proc u64 rotl64(u64 val, u64 amt);
proc u32 rotr32(u32 val, u32 amt);
proc u64 rotr64(u64 val, u64 amt);

//--------------------------------------------------------------------------------
// enum -> sign

proc s32 sign_from_side_s32(Side side);
proc f32 sign_from_side_f32(Side side);

//--------------------------------------------------------------------------------
// memory functions

proc b8 memory_is_zero(rawptr ptr, s64 size);

// -------------------------------------------------------------------------------------------------
// Handles

// #define zero_handle(T) (T){}
// #define handle_is_zero(handle) _handle_is_zero(handle.v, array_count(handle.v))
// #define handle_match(a, b) _handle_match(a.v, array_count(a.v), b.v, array_count(b.v))
// proc b8
// _handle_is_zero(u64 *arr, s32 count) {
// 	assert(count > 0);
// 	b8 result = true;
// 	for (s32 i = 0; i < count; i += 1) {
// 		if (arr[i] != 0) {
// 			result = false;
// 			break;
// 		}
// 	}
// 	return result;
// }
// proc b8
// _handle_match(u64 *a, s32 a_count, u64 *b, s32 b_count) {
// 	assert(a_count == b_count && a_count > 0 && b_count > 0);
// 	b8 result = true;
// 	for (s32 i = 0; i < a_count; i += 1) {
// 		if (a[i] != b[i]) {
// 			result = false;
// 			break;
// 		}
// 	}
// 	return result;
// }

//--------------------------------------------------------------------------------
// sorting

#define quick_sort(ptr, count, element_size, cmp_proc) qsort((ptr), (count), (element_size), (int (*)(const void *, const void *))(cmp_proc))

//--------------------------------------------------------------------------------
// text 2d coordinate range functions

proc Loc       make_loc(s64 line, s64 column);
proc b8        loc_match(Loc a, Loc b);
proc b8        loc_less_than(Loc a, Loc b);
proc Loc       loc_min(Loc a, Loc b);
proc Loc       loc_max(Loc a, Loc b);
proc Loc_Range make_loc_rng(Loc min, Loc max);
proc Loc_Range loc_rng_intersect(Loc_Range a, Loc_Range b);
proc Loc_Range loc_rng_union(Loc_Range a, Loc_Range b);
proc b8        loc_rng_contains(Loc_Range r, Loc pt);

//--------------------------------------------------------------------------------
// non-virtual ring buffer read/write

proc s64 ring_write(rawptr ring_base, s64 ring_size, s64 ring_pos, rawptr src_data, s64 src_data_size);
proc s64 ring_read(rawptr ring_base, s64 ring_size, s64 ring_pos, rawptr dst_data, s64 read_size);

//--------------------------------------------------------------------------------
// units

#define kilobytes(n) (((s64)(n)) << 10)
#define megabytes(n) (((s64)(n)) << 20)
#define gigabytes(n) (((s64)(n)) << 30)
#define terabytes(n) (((s64)(n)) << 40)

#define thousands(n)  ((n)*1000ll)
#define millions(n)   ((n)*1000000ll)
#define billions(n)   ((n)*1000000000ll)

//--------------------------------------------------------------------------------
// clamps, min, max

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))
#define clamp_top(a,x) min(a,x)
#define clamp_bot(x,b) max(x,b)
#define clamp(a,x,b) (((x)<(a))?(a):((x)>(b))?(b):(x))

//--------------------------------------------------------------------------------
// flags

#define has_flags(flags, subset) ((flags&(subset))==(subset))
#define add_flag(flags, flag) stmnt(flags |= flag;)
#define remove_flag(flags, flag) stmnt(flags &= (~flag))

//--------------------------------------------------------------------------------
// member offsets

#define member_of(T,m) (((T*)0)->m)
#define offset_of(T,m) int_from_ptr(&member_of(T,m))
#define member_from_offset(T,ptr,off) (T)((((u8*)ptr)+(off)))
#define cost_from_member(T,m,ptr)     (T*)(((u8*)ptr)-offset_of(T,m))

//--------------------------------------------------------------------------------
// container of

#define container_of(ptr, T, m) (T*)((u8*)(ptr)-offset_of(T,m))

//--------------------------------------------------------------------------------
// for loop

#define defer_loop(begin, end)         for(int _i_ = ((begin), 0); !_i_; _i_ += 1, (end))
#define defer_loop_checked(begin, end) for(int _i_ = 2 * !(begin); (_i_ == 2 ? ((end), 0) : !_i_); _i_ += 1, (end))

#define each_index(it, count) s64 it = 0; it < (count); it += 1
#define each_index_rng(it, rng) s64 it = (rng).min; it < (rng).max; it += 1
#define each_element(it, array) s64 it = 0; it < array_count(array); it += 1
#define each_enum_val(it, type) type it = (type)0; it < type##_count; it = (type)(it+1)
#define each_non_zero_enum_val(it, type) type it = (type)1; it < type##_count; it = (type)(it+1)

#define each_node_nz(it, head, type, nilv, next) type *it = head; !check_nil(it, nilv); it = it->next
#define each_node_nil(it, head, type, nilv) each_node_nz(it, head, type, nilv, next)
#define each_node(it, head, type) each_node_nz(it, head, type, nil, next)
#define each_node_reverse_nil(it, head, type, nilv) each_node_nz(it, head, type, nilv, prev)
#define each_node_reverse(it, head, type) each_node_nz(it, head, type, nil, prev)

//--------------------------------------------------------------------------------
// memory operations

#define memory_copy(dst, src, size)    memmove((dst), (src), (u64)(size))
#define memory_set(dst, byte, size)    memset((dst), (byte), (u64)(size))
#define memory_compare(a, b, size)     memcmp((a), (b), (u64)(size))
#define memory_strlen(ptr)             strlen(ptr)

#define memory_copy_struct(d,s)  memory_copy((d),(s),size_of(*(d)))
#define memory_copy_array(d,s)   memory_copy((d),(s),size_of(d))
#define memory_copy_typed(d,s,c) memory_copy((d),(s),size_of(*(d))*(c))

#define memory_zero(s,sz)        memset((s),0,(u64)(sz))
#define memory_zero_struct(s)   memory_zero((s),sizeof(*(s)))
#define memory_zero_array(a)    memory_zero((a),sizeof(a))
#define memory_zero_typed(m,c)  memory_zero((m),sizeof(*(m))*(c))

#define memory_match(a,b,z) (memory_compare((a),(b),(z)) == 0)
#define memory_match_struct(a,b) memory_match((a),(b),sizeof(*(a)))
#define memory_match_array(a,b) memory_match((a),(b),sizeof(a))

#define memory_read(T,p,e)    ( ((p)+sizeof(T)<=(e))?(*(T*)(p)):(0) )
#define memory_consume(T,p,e) ( ((p)+sizeof(T)<=(e))?((p)+=sizeof(T),*(T*)((p)-sizeof(T))):((p)=(e),0) )

//--------------------------------------------------------------------------------
// linked list macros

#define check_nil(p,nilv) ((p) == 0 || (p) == nilv)
#define set_nil(p,nilv) ((p) = nilv)

// doubly-linked-lists
// #define dll_insert_npz(nilv,f,l,p,n,next,prev) (check_nil(f, nilv) ? \
// ((f) = (l) = (n), set_nil(nilv,(n)->next), set_nil((n)->prev), nilv) :\
// check_nil(p,nilv) ? \
// ((n)->next = (f), (f)->prev = (n), (f) = (n), set_nil((n)->prev, nilv)) :\
// ((p)==(l)) ? \
// ((l)->next = (n), (n)->prev = (l), (l) = (n), set_nil((n)->next), nilv) :\
// (((!check_nil(p,nilv) && check_nil((p)->next),nilv) ? (0) : ((p)->next->prev = (n))), ((n)->next = (p)->next), ((p)->next = (n)), ((n)->prev = (p))))
// #define dll_push_back_npz(nilv,f,l,n,next,prev) dll_insert_npz(nilv,f,l,l,n,next,prev)
// #define dll_push_front_npz(nilv,f,l,n,next,prev) dll_insert_npz(nilv,l,f,f,n,prev,next)
// #define dll_remove_npz(nilv,f,l,n,next,prev) (((n) == (f) ? (f) = (n)->next : (0)),\
// ((n) == (l) ? (l) = (l)->prev : (0)),\
// (check_nil((n)->prev, nilv) ? (0) :\
// ((n)->prev->next = (n)->next)),\
// (check_nil((n)->next, nilv) ? (0) :\
// ((n)->next->prev = (n)->prev)))

// singly-linked, doubly-headed lists (queues)
// #define sll_queue_push_nz(nilv,f,l,n,next) (check_nil(f,nilv)?\
// ((f)=(l)=(n),set_nil((n)->next, nilv)):\
// ((l)->next=(n),(l)=(n),set_nil((n)->next, nilv)))
// #define sll_queue_push_front_nz(nilv,f,l,n,next) (check_nil(f,nilv)?\
// ((f)=(l)=(n),set_nil((n)->next, nilv)):\
// ((n)->next=(f),(f)=(n)))
// #define sll_queue_pop_nz(nilv,f,l,next) ((f)==(l)?\
// (set_nil(f,nilv),set_nil(l,nilv)):\
// ((f)=(f)->next))

// singly-linked, singly-headed lists (stacks)
// #define sll_stack_push_n(f,n,next) ((n)->next=(f), (f)=(n))
// #define sll_stack_pop_n(f,next) ((f)=(f)->next)

// doubly-linked-list helpers
// #define dll_insert_np(f,l,p,n,next,prev) dll_insert_npz(0,f,l,p,n,next,prev)
// #define dll_push_back_np(f,l,n,next,prev) dll_push_back_npz(0,f,l,n,next,prev)
// #define dll_push_front_np(f,l,n,next,prev) dll_push_front_npz(0,f,l,n,next,prev)
// #define dll_remove_np(f,l,n,next,prev) dll_remove_npz(0,f,l,n,next,prev)
// #define dll_insert(f,l,p,n) dll_insert_npz(0,f,l,p,n,next,prev)
// #define dll_push_back(f,l,n) dll_push_back_npz(0,f,l,n,next,prev)
// #define dll_push_front(f,l,n) dll_push_front_npz(0,f,l,n,next,prev)
// #define dll_remove(f,l,n) dll_remove_npz(0,f,l,n,next,prev)

// singly-linked, doubly-headed list helpers
// #define sll_queue_push_n(f,l,n,next) sll_queue_push_nz(0,f,l,n,next)
// #define sll_queue_push_front_n(f,l,n,next) sll_queue_push_front_nz(0,f,l,n,next)
// #define sll_queue_pop_n(f,l,next) sll_queue_pop_nz(0,f,l,next)
// #define sll_queue_push(f,l,n) sll_queue_push_nz(0,f,l,n,next)
// #define sll_queue_push_front(f,l,n) sll_queue_push_front_nz(0,f,l,n,next)
// #define sll_queue_pop(f,l) sll_queue_pop_nz(0,f,l,next)

// singly-linked, singly-headed list helpers
// #define sll_stack_push(f,n) sll_stack_push_n(f,n,next)
// #define sll_stack_pop(f) sll_stack_pop_n(f,next)

// #define freelist_push(list, p) stmnt(sll_stack_push(list, p); asan_poison(p, sizeof(*p));)
// #define freelist_pop(list) stmnt(asan_cure(list, sizeof(*list)); sll_stack_pop(list);)

//--------------------------------------------------------------------------------
// atomic operations

#if COMPILER_MSVC
# if ARCH_X64
#  define ins_atomic_u64_eval(x)                 *((volatile u64 *)(x))
#  define ins_atomic_u64_inc_eval(x)             InterlockedIncrement64((volatile __int64 *)(x))
#  define ins_atomic_u64_dec_eval(x)             InterlockedDecrement64((volatile __int64 *)(x))
#  define ins_atomic_u64_eval_assign(x,c)        InterlockedExchange64((volatile __int64 *)(x),(c))
#  define ins_atomic_u64_add_eval(x,c)           InterlockedAdd64((volatile __int64 *)(x), c)
#  define ins_atomic_u64_eval_cond_assign(x,k,c) InterlockedCompareExchange64((volatile __int64 *)(x),(k),(c))
#  define ins_atomic_u32_eval(x)                 *((volatile u32 *)(x))
#  define ins_atomic_u32_inc_eval(x)             InterlockedIncrement((volatile LONG *)x)
#  define ins_atomic_u32_eval_assign(x,c)        InterlockedExchange((volatile LONG *)(x),(c))
#  define ins_atomic_u32_eval_cond_assign(x,k,c) InterlockedCompareExchange((volatile LONG *)(x),(k),(c))
#  define ins_atomic_u32_add_eval(x,c)           InterlockedAdd((volatile LONG *)(x), c)
# else
#  error Atomic intrinsics not defined for this compiler / architecture combination.
# endif
#elif COMPILER_CLANG || COMPILER_GCC
#  define ins_atomic_u64_eval(x)                 __atomic_load_n(x, __ATOMIC_SEQ_CST)
#  define ins_atomic_u64_inc_eval(x)             (__atomic_fetch_add((volatile u64 *)(x), 1, __ATOMIC_SEQ_CST) + 1)
#  define ins_atomic_u64_dec_eval(x)             (__atomic_fetch_sub((volatile u64 *)(x), 1, __ATOMIC_SEQ_CST) - 1)
#  define ins_atomic_u64_eval_assign(x,c)        __atomic_exchange_n(x, c, __ATOMIC_SEQ_CST)
#  define ins_atomic_u64_add_eval(x,c)           (__atomic_fetch_add((volatile u64 *)(x), c, __ATOMIC_SEQ_CST) + (c))
#  define ins_atomic_u64_eval_cond_assign(x,k,c) ({ u64 _new = (c); __atomic_compare_exchange_n((volatile u64 *)(x),&_new,(k),0,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST); _new; })
#  define ins_atomic_u32_eval(x)                 __atomic_load_n(x, __ATOMIC_SEQ_CST)
#  define ins_atomic_u32_inc_eval(x)             (__atomic_fetch_add((volatile u32 *)(x), 1, __ATOMIC_SEQ_CST) + 1)
#  define ins_atomic_u32_add_eval(x,c)           (__atomic_fetch_add((volatile u32 *)(x), c, __ATOMIC_SEQ_CST) + (c))
#  define ins_atomic_u32_eval_assign(x,c)        __atomic_exchange_n(x, c, __ATOMIC_SEQ_CST)
#  define ins_atomic_u32_eval_cond_assign(x,k,c) ({ u32 _new = (c); __atomic_compare_exchange_n((volatile u32 *)(x),&_new,(k),0,__ATOMIC_SEQ_CST,__ATOMIC_SEQ_CST); _new; })
#else
#  error Atomic intrinsics not defined for this compiler / architecture.
#endif

#if ARCH_64BIT
# define ins_atomic_ptr_eval_cond_assign(x,k,c) (void*)ins_atomic_u64_eval_cond_assign((volatile u64 *)(x), (u64)(k), (u64)(c))
# define ins_atomic_ptr_eval_assign(x,c)        (void*)ins_atomic_u64_eval_assign((volatile u64 *)(x), (u64)(c))
# define ins_atomic_ptr_eval(x)                 (void*)ins_atomic_u64_eval((volatile u64 *)x)
#elif ARCH_32BIT
# define ins_atomic_ptr_eval_cond_assign(x,k,c) (void*)ins_atomic_u32_eval_cond_assign((volatile u32 *)(x), (u32)(k), (u32)(c))
# define ins_atomic_ptr_eval_assign(x,c)        (void*)ins_atomic_u32_eval_assign((volatile u32 *)(x), (u32)(c))
# define ins_atomic_ptr_eval(x)                 (void*)ins_atomic_u32_eval((volatile u32 *)x)
#else
# error Atomic intrinsics for pointers not defined for this architecture.
#endif

#endif // BASE_CORE_H
