
#ifndef BASE_MATH_H
#define BASE_MATH_H

//--------------------------------------------------------------------------------
// vector types

// vector 2

typedef union vec2f32 vec2f32;
union vec2f32 {
	struct {
		f32 x;
		f32 y;
	};
	f32 v[2];
};
typedef union vec2s64 vec2s64;
union vec2s64 {
	struct {
		s64 x;
		s64 y;
	};
	s64 v[2];
};
typedef union vec2s32 vec2s32;
union vec2s32 {
	struct {
		s32 x;
		s32 y;
	};
	s32 v[2];
};
typedef union vec2s16 vec2s16;
union vec2s16 {
	struct {
		s16 x;
		s16 y;
	};
	s16 v[2];
};

// vector 3

typedef union vec3f32 vec3f32;
union vec3f32 {
	struct {
		f32 x;
		f32 y;
		f32 z;
	};
	struct {
		vec2f32 xy;
		f32 _z0;
	};
	struct {
		f32 _x0;
		vec2f32 yz;
	};
	f32 v[3];
};
typedef union vec3s32 vec3s32;
union vec3s32 {
	struct {
		s32 x;
		s32 y;
		s32 z;
	};
	struct {
		vec2s32 xy;
		s32 _z0;
	};
	struct {
		s32 _x0;
		vec2s32 yz;
	};
	s32 v[3];
};


// vector 4

typedef union vec4f32 vec4f32;
union vec4f32 {
	struct {
		f32 x;
		f32 y;
		f32 z;
		f32 w;
	};
	struct {
		vec2f32 xy;
		vec2f32 zw;
	};
	struct {
		vec3f32 xyz;
		f32 _w0;
	};
	struct {
		f32 _x0;
		vec3f32 yzw;
	};
	f32 v[4];
};
typedef union vec4s32 vec4s32;
union vec4s32 {
	struct {
		s32 x;
		s32 y;
		s32 z;
		s32 w;
	};
	struct {
		vec2s32 xy;
		vec2s32 zw;
	};
	struct {
		vec3s32 xyz;
		s32 _w0;
	};
	struct {
		s32 _x0;
		vec3s32 yzw;
	};
	s32 v[4];
};

//--------------------------------------------------------------------------------
// matrix types

// IMPORTANT NOTE ns: The matrices are row oriented. That is if you had for example
// 3d basis vectors, they would be stored as:
// {
// 	x_axis.x, x_axis.y, x_axis.z,
// 	y_axis.x, y_axis.y, y_axis.z,
// 	z_axis.x, z_axis.y, z_axis.z,
// }
// and not:
// {
// 	x_axis.x, y_axis.x, z_axis.x,
// 	x_axis.y, y_axis.y, z_axis.y,
// 	x_axis.z, y_axis.z, z_axis.z,
// }

typedef struct mat3x3f32 mat3x3f32;
struct mat3x3f32 {
	f32 v[3][3];
};
typedef struct mat4x4f32 mat4x4f32;
struct mat4x4f32 {
	f32 v[4][4];
};

//--------------------------------------------------------------------------------
// range types

// single axis range

typedef union rng1u32 rng1u32;
union rng1u32 {
	struct {
		u32 min;
		u32 max;
	};
	u32 v[2];
};
typedef union rng1s32 rng1s32;
union rng1s32 {
	struct {
		s32 min;
		s32 max;
	};
	s32 v[2];
};
typedef union rng1u64 rng1u64;
union rng1u64 {
	struct {
		u64 min;
		u64 max;
	};
	u64 v[2];
};
typedef union rng1s64 rng1s64;
union rng1s64 {
	struct {
		s64 min;
		s64 max;
	};
	s64 v[2];
};

typedef union rng1f32 rng1f32;
union rng1f32 {
	struct {
		f32 min;
		f32 max;
	};
	f32 v[2];
};

typedef union rng1f64 rng1f64;
union rng1f64 {
	struct {
		f64 min;
		f64 max;
	};
	f64 v[2];
};

// double axis range (rectangles)

typedef union rng2s64 rng2s64;
union rng2s64 {
	struct {
		vec2s64 min;
		vec2s64 max;
	};
	struct {
		vec2s64 p0;
		vec2s64 p1;
	};
	struct {
		s64 x0;
		s64 y0;
		s64 x1;
		s64 y1;
	};
	vec2s64 v[2];
};
typedef union rng2s32 rng2s32;
union rng2s32 {
	struct {
		vec2s32 min;
		vec2s32 max;
	};
	struct {
		vec2s32 p0;
		vec2s32 p1;
	};
	struct {
		s32 x0;
		s32 y0;
		s32 x1;
		s32 y1;
	};
	vec2s32 v[2];
};
typedef union rng2s16 rng2s16;
union rng2s16 {
	struct {
		vec2s16 min;
		vec2s16 max;
	};
	struct {
		vec2s16 p0;
		vec2s16 p1;
	};
	struct {
		s16 x0;
		s16 y0;
		s16 x1;
		s16 y1;
	};
	vec2s16 v[2];
};
typedef union rng2f32 rng2f32;
union rng2f32 {
	struct {
		vec2f32 min;
		vec2f32 max;
	};
	struct {
		vec2f32 p0;
		vec2f32 p1;
	};
	struct {
		f32 x0;
		f32 y0;
		f32 x1;
		f32 y1;
	};
	vec2f32 v[2];
};

//--------------------------------------------------------------------------------
// list & array types for ranges

typedef struct rng1u64_node rng1u64_node;
struct rng1u64_node {
	rng1u64_node *next;
	rng1u64 v;
};
typedef struct rng1u64_list rng1u64_list;
struct rng1u64_list {
	s64 count;
	rng1u64_node *head;
	rng1u64_node *tail;
};

typedef struct rng1u64_array rng1u64_array;
struct rng1u64_array {
	s64 count;
	rng1u64 *v;
};

typedef struct rng1u32_node rng1u32_node;
struct rng1u32_node {
	rng1u32_node *next;
	rng1u32 v;
};
typedef struct rng1u32_list rng1u32_list;
struct rng1u32_list {
	s64 count;
	rng1u32_node *head;
	rng1u32_node *tail;
};

typedef struct rng1u32_array rng1u32_array;
struct rng1u32_array {
	s64 count;
	rng1u32 *v;
};

typedef struct rng1s64_node rng1s64_node;
struct rng1s64_node {
	rng1s64_node *next;
	rng1s64 v;
};
typedef struct rng1s64_list rng1s64_list;
struct rng1s64_list {
	s64 count;
	rng1s64_node *head;
	rng1s64_node *tail;
};

typedef struct rng1s64_array rng1s64_array;
struct rng1s64_array {
	s64 count;
	rng1s64 *v;
};

typedef struct rng1s32_node rng1s32_node;
struct rng1s32_node {
	rng1s32_node *next;
	rng1s32 v;
};
typedef struct rng1s32_list rng1s32_list;
struct rng1s32_list {
	s32 count;
	rng1s32_node *head;
	rng1s32_node *tail;
};

typedef struct rng1s32_array rng1s32_array;
struct rng1s32_array {
	s32 count;
	rng1s32 *v;
};


//--------------------------------------------------------------------------------
// scalar ops

#define sqrt_f32(v)   sqrtf(v)
#define mod_f32(a, b) fmodf((a), (b))
#define pow_f32(b, e) powf((b), (e))
#define ceil_f32(v)   ceilf(v)
#define floor_f32(v)  floorf(v)
#define round_f32(v)  roundf(v)
#define abs_f32(v)    fabsf(v)
#define radians_from_turns_f32(v) ((v)*2*3.1415926535897f)
#define turns_from_radians_f32(v) ((v)/2*3.1415926535897f)
#define degrees_from_turns_f32(v) ((v)*360.f)
#define turns_from_degrees_f32(v) ((v)/360.f)
#define degrees_from_radians_f32(v) (degrees_from_turns_f32(turns_from_radians_f32(v)))
#define radians_from_degrees_f32(v) (radians_from_turns_f32(turns_from_degrees_f32(v)))
#define sin_f32(v)    sinf(radians_from_turns_f32(v))
#define cos_f32(v)    cosf(radians_from_turns_f32(v))
#define tan_f32(v)    tanf(radians_from_turns_f32(v))

#define sqrt_f64(v)   sqrt(v)
#define mod_f64(a, b) fmod((a), (b))
#define pow_f64(b, e) pow((b), (e))
#define ceil_f64(v)   ceil(v)
#define floor_f64(v)  floor(v)
#define round_f64(v)  round(v)
#define abs_f64(v)    fabs(v)
#define radians_from_turns_f64(v) ((v)*2*3.1415926535897)
#define turns_from_radians_f64(v) ((v)/2*3.1415926535897)
#define degrees_from_turns_f64(v) ((v)*360.0)
#define turns_from_degrees_f64(v) ((v)/360.0)
#define degrees_from_radians_f64(v) (degrees_from_turns_f64(turns_from_radians_f64(v)))
#define radians_from_degrees_f64(v) (radians_from_turns_f64(turns_from_degrees_f64(v)))
#define sin_f64(v)    sin(radians_from_turns_f64(v))
#define cos_f64(v)    cos(radians_from_turns_f64(v))
#define tan_f64(v)    tan(radians_from_turns_f64(v))

proc f32 mix_f32(f32 a, f32 b, f32 t);
proc f64 mix_f64(f64 a, f64 b, f64 t);

proc s64 abs_s64(s64 v);

//--------------------------------------------------------------------------------
// vector ops

// vector 2
#define v2f32(x, y) vec_2f32((x), (y))
proc vec2f32 vec_2f32(f32 x, f32 y);
proc vec2f32 add_2f32(vec2f32 a, vec2f32 b);
proc vec2f32 sub_2f32(vec2f32 a, vec2f32 b);
proc vec2f32 mul_2f32(vec2f32 a, vec2f32 b);
proc vec2f32 div_2f32(vec2f32 a, vec2f32 b);
proc vec2f32 scale_2f32(vec2f32 v, f32 s);
proc f32     dot_2f32(vec2f32 a, vec2f32 b);
proc f32     length_squared_2f32(vec2f32 v);
proc f32     length_2f32(vec2f32 v);
proc vec2f32 normalize_2f32(vec2f32 v);
proc vec2f32 mix_2f32(vec2f32 a, vec2f32 b, f32 t);

#define v2s64(x, y) vec_2s64((x), (y))
proc vec2s64 vec_2s64(s64 x, s64 y);
proc vec2s64 add_2s64(vec2s64 a, vec2s64 b);
proc vec2s64 sub_2s64(vec2s64 a, vec2s64 b);
proc vec2s64 mul_2s64(vec2s64 a, vec2s64 b);
proc vec2s64 div_2s64(vec2s64 a, vec2s64 b);
proc vec2s64 scale_2s64(vec2s64 v, s64 s);
proc s64     dot_2s64(vec2s64 a, vec2s64 b);
proc s64     length_squared_2s64(vec2s64 v);
proc s64     length_2s64(vec2s64 v);
proc vec2s64 normalize_2s64(vec2s64 v);
proc vec2s64 mix_2s64(vec2s64 a, vec2s64 b, f64 t);

#define v2s32(x, y) vec_2s32((x), (y))
proc vec2s32 vec_2s32(s32 x, s32 y);
proc vec2s32 add_2s32(vec2s32 a, vec2s32 b);
proc vec2s32 sub_2s32(vec2s32 a, vec2s32 b);
proc vec2s32 mul_2s32(vec2s32 a, vec2s32 b);
proc vec2s32 div_2s32(vec2s32 a, vec2s32 b);
proc vec2s32 scale_2s32(vec2s32 v, s32 s);
proc s32     dot_2s32(vec2s32 a, vec2s32 b);
proc s32     length_squared_2s32(vec2s32 v);
proc s32     length_2s32(vec2s32 v);
proc vec2s32 normalize_2s32(vec2s32 v);
proc vec2s32 mix_2s32(vec2s32 a, vec2s32 b, f32 t);

#define v2s16(x, y) vec_2s16((x), (y))
proc vec2s16 vec_2s16(s16 x, s16 y);
proc vec2s16 add_2s16(vec2s16 a, vec2s16 b);
proc vec2s16 sub_2s16(vec2s16 a, vec2s16 b);
proc vec2s16 mul_2s16(vec2s16 a, vec2s16 b);
proc vec2s16 div_2s16(vec2s16 a, vec2s16 b);
proc vec2s16 scale_2s16(vec2s16 v, s16 s);
proc s16     dot_2s16(vec2s16 a, vec2s16 b);
proc s16     length_squared_2s16(vec2s16 v);
proc s16     length_2s16(vec2s16 v);
proc vec2s16 normalize_2s16(vec2s16 v);
proc vec2s16 mix_2s16(vec2s16 a, vec2s16 b, f32 t);

// vector 3
#define v3f32(x, y, z) vec_3f32((x), (y), (z))
proc vec3f32 vec_3f32(f32 x, f32 y, f32 z);
proc vec3f32 add_3f32(vec3f32 a, vec3f32 b);
proc vec3f32 sub_3f32(vec3f32 a, vec3f32 b);
proc vec3f32 mul_3f32(vec3f32 a, vec3f32 b);
proc vec3f32 div_3f32(vec3f32 a, vec3f32 b);
proc vec3f32 scale_3f32(vec3f32 v, f32 s);
proc f32     dot_3f32(vec3f32 a, vec3f32 b);
proc f32     length_squared_3f32(vec3f32 v);
proc f32     length_3f32(vec3f32 v);
proc vec3f32 normalize_3f32(vec3f32 v);
proc vec3f32 mix_3f32(vec3f32 a, vec3f32 b, f32 t);
proc vec3f32 cross_3f32(vec3f32 a, vec3f32 b);

#define v3s32(x, y, z) vec_3s32((x), (y), (z))
proc vec3s32 vec_3s32(s32 x, s32 y, s32 z);
proc vec3s32 add_3s32(vec3s32 a, vec3s32 b);
proc vec3s32 sub_3s32(vec3s32 a, vec3s32 b);
proc vec3s32 mul_3s32(vec3s32 a, vec3s32 b);
proc vec3s32 div_3s32(vec3s32 a, vec3s32 b);
proc vec3s32 scale_3s32(vec3s32 v, s32 s);
proc s32     dot_3s32(vec3s32 a, vec3s32 b);
proc s32     length_squared_3s32(vec3s32 v);
proc s32     length_3s32(vec3s32 v);
proc vec3s32 normalize_3s32(vec3s32 v);
proc vec3s32 mix_3s32(vec3s32 a, vec3s32 b, f32 t);
proc vec3s32 cross_3s32(vec3s32 a, vec3s32 b);

// vector 4
#define v4f32(x, y, z, w) vec_4f32((x), (y), (z), (w))
proc vec4f32 vec_4f32(f32 x, f32 y, f32 z, f32 w);
proc vec4f32 add_4f32(vec4f32 a, vec4f32 b);
proc vec4f32 sub_4f32(vec4f32 a, vec4f32 b);
proc vec4f32 mul_4f32(vec4f32 a, vec4f32 b);
proc vec4f32 div_4f32(vec4f32 a, vec4f32 b);
proc vec4f32 scale_4f32(vec4f32 v, f32 s);
proc f32     dot_4f32(vec4f32 a, vec4f32 b);
proc f32     length_squared_4f32(vec4f32 v);
proc f32     length_4f32(vec4f32 v);
proc vec4f32 normalize_4f32(vec4f32 v);
proc vec4f32 mix_4f32(vec4f32 a, vec4f32 b, f32 t);

#define v4s32(x, y, z, w) vec_4s32((x), (y), (z), (w))
proc vec4s32 vec_4s32(s32 x, s32 y, s32 z, s32 w);
proc vec4s32 add_4s32(vec4s32 a, vec4s32 b);
proc vec4s32 sub_4s32(vec4s32 a, vec4s32 b);
proc vec4s32 mul_4s32(vec4s32 a, vec4s32 b);
proc vec4s32 div_4s32(vec4s32 a, vec4s32 b);
proc vec4s32 scale_4s32(vec4s32 v, s32 s);
proc s32     dot_4s32(vec4s32 a, vec4s32 b);
proc s32     length_squared_4s32(vec4s32 v);
proc s32     length_4s32(vec4s32 v);
proc vec4s32 normalize_4s32(vec4s32 v);
proc vec4s32 mix_4s32(vec4s32 a, vec4s32 b, f32 t);

//--------------------------------------------------------------------------------
// matrix ops

// matrix 3
proc mat3x3f32 mat_3x3f32(f32 diagonal);
proc mat3x3f32 mk_translate_3x3f32(vec2f32 delta);
proc mat3x3f32 mk_scale_3x3f32(vec2f32 scale);
proc mat3x3f32 mul_3x3f32(mat3x3f32 a, mat3x3f32 b);

// matrix 4
proc mat4x4f32 mat_4x4f32(f32 diagonal);
proc mat4x4f32 mk_translate_4x4f32(vec3f32 delta);
proc mat4x4f32 mk_scale_4x4f32(vec3f32 scale);

// NOTE ns: near_z and far_z values should be passed in as non-negative, see impl.
proc mat4x4f32 mk_perspective_4x4f32(f32 fov, f32 aspect_ratio, f32 near_z, f32 far_z);
proc mat4x4f32 mk_orthographic_4x4f32(f32 left, f32 right, f32 bottom, f32 top, f32 near_z, f32 far_z);
proc mat4x4f32 mk_look_at_4x4f32(vec3f32 eye, vec3f32 center, vec3f32 up);
proc mat4x4f32 mk_rotate_4x4f32(vec3f32 axis, f32 turns);
proc mat4x4f32 mul_4x4f32(mat4x4f32 a, mat4x4f32 b);
proc mat4x4f32 scale_4x4f32(mat4x4f32 m, f32 scale);
proc mat4x4f32 inverse_4x4f32(mat4x4f32 m);
proc mat4x4f32 derotate_4x4f32(mat4x4f32 mat);

//--------------------------------------------------------------------------------
// single axis range ops

#define r1u32(min, max) rng_1u32((min), (max))
proc rng1u32 rng_1u32(u32 min, u32 max);
proc rng1u32 shift_1u32(rng1u32 r, u32 x);
proc rng1u32 pad_1u32(rng1u32 r, u32 x);
proc u32     center_1u32(rng1u32 r);
proc b8     contains_1u32(rng1u32 r, u32 x);
proc u32     dim_1u32(rng1u32 r);
proc rng1u32 union_1u32(rng1u32 a, rng1u32 b);
proc rng1u32 intersect_1u32(rng1u32 a, rng1u32 b);
proc u32     clamp_1u32(rng1u32 r, u32 v);

#define r1s32(min, max) rng_1s32((min), (max))
proc rng1s32 rng_1s32(s32 min, s32 max);
proc rng1s32 shift_1s32(rng1s32 r, s32 x);
proc rng1s32 pad_1s32(rng1s32 r, s32 x);
proc s32     center_1s32(rng1s32 r);
proc b8     contains_1s32(rng1s32 r, s32 x);
proc s32     dim_1s32(rng1s32 r);
proc rng1s32 union_1s32(rng1s32 a, rng1s32 b);
proc rng1s32 intersect_1s32(rng1s32 a, rng1s32 b);
proc s32     clamp_1s32(rng1s32 r, s32 v);

#define r1u64(min, max) rng_1u64((min), (max))
proc rng1u64 rng_1u64(u64 min, u64 max);
proc rng1u64 shift_1u64(rng1u64 r, u64 x);
proc rng1u64 pad_1u64(rng1u64 r, u64 x);
proc u64     center_1u64(rng1u64 r);
proc b8     contains_1u64(rng1u64 r, u64 x);
proc u64     dim_1u64(rng1u64 r);
proc rng1u64 union_1u64(rng1u64 a, rng1u64 b);
proc rng1u64 intersect_1u64(rng1u64 a, rng1u64 b);
proc u64     clamp_1u64(rng1u64 r, u64 v);

#define r1s64(min, max) rng_1s64((min), (max))
proc rng1s64 rng_1s64(s64 min, s64 max);
proc rng1s64 shift_1s64(rng1s64 r, s64 x);
proc rng1s64 pad_1s64(rng1s64 r, s64 x);
proc s64     center_1s64(rng1s64 r);
proc b8     contains_1s64(rng1s64 r, s64 x);
proc s64     dim_1s64(rng1s64 r);
proc rng1s64 union_1s64(rng1s64 a, rng1s64 b);
proc rng1s64 intersect_1s64(rng1s64 a, rng1s64 b);
proc s64     clamp_1s64(rng1s64 r, s64 v);

#define r1f32(min, max) rng_1f32((min), (max))
proc rng1f32 rng_1f32(f32 min, f32 max);
proc rng1f32 shift_1f32(rng1f32 r, f32 x);
proc rng1f32 pad_1f32(rng1f32 r, f32 x);
proc f32     center_1f32(rng1f32 r);
proc b8     contains_1f32(rng1f32 r, f32 x);
proc f32     dim_1f32(rng1f32 r);
proc rng1f32 union_1f32(rng1f32 a, rng1f32 b);
proc rng1f32 intersect_1f32(rng1f32 a, rng1f32 b);
proc f32     clamp_1f32(rng1f32 r, f32 v);

#define r1f64(min, max) rng_1f64((min), (max))
proc rng1f64 rng_1f64(f64 min, f64 max);
proc rng1f64 shift_1f64(rng1f64 r, f64 x);
proc rng1f64 pad_1f64(rng1f64 r, f64 x);
proc f64     center_1f64(rng1f64 r);
proc b8     contains_1f64(rng1f64 r, f64 x);
proc f64     dim_1f64(rng1f64 r);
proc rng1f64 union_1f64(rng1f64 a, rng1f64 b);
proc rng1f64 intersect_1f64(rng1f64 a, rng1f64 b);
proc f64     clamp_1f64(rng1f64 r, f64 v);

//--------------------------------------------------------------------------------
// double axis range ops

#define r2s16(min, max) rng_2s16((min), (max))
#define r2s16p(x, y, z, w) r2s16(v2s16((x), (y)), v2s16((z), (w)))
proc rng2s16 rng_2s16(vec2s16 min, vec2s16 max);
proc rng2s16 shift_2s16(rng2s16 r, vec2s16 x);
proc rng2s16 pad_2s16(rng2s16 r, s16 x);
proc vec2s16 center_2s16(rng2s16 r);
proc b8     contains_2s16(rng2s16 r, vec2s16 x);
proc vec2s16 dim_2s16(rng2s16 r);
proc rng2s16 union_2s16(rng2s16 a, rng2s16 b);
proc rng2s16 intersect_2s16(rng2s16 a, rng2s16 b);
proc vec2s16 clamp_2s16(rng2s16 r, vec2s16 v);

#define r2s32(min, max) rng_2s32((min), (max))
#define r2s32p(x, y, z, w) r2s32(v2s32((x), (y)), v2s32((z), (w)))
proc rng2s32 rng_2s32(vec2s32 min, vec2s32 max);
proc rng2s32 shift_2s32(rng2s32 r, vec2s32 x);
proc rng2s32 pad_2s32(rng2s32 r, s32 x);
proc vec2s32 center_2s32(rng2s32 r);
proc b8     contains_2s32(rng2s32 r, vec2s32 x);
proc vec2s32 dim_2s32(rng2s32 r);
proc rng2s32 union_2s32(rng2s32 a, rng2s32 b);
proc rng2s32 intersect_2s32(rng2s32 a, rng2s32 b);
proc vec2s32 clamp_2s32(rng2s32 r, vec2s32 v);

#define r2s64(min, max) rng_2s64((min), (max))
#define r2s64p(x, y, z, w) r2s64(v2s64((x), (y)), v2s64((z), (w)))
proc rng2s64 rng_2s64(vec2s64 min, vec2s64 max);
proc rng2s64 shift_2s64(rng2s64 r, vec2s64 x);
proc rng2s64 pad_2s64(rng2s64 r, s64 x);
proc vec2s64 center_2s64(rng2s64 r);
proc b8     contains_2s64(rng2s64 r, vec2s64 x);
proc vec2s64 dim_2s64(rng2s64 r);
proc rng2s64 union_2s64(rng2s64 a, rng2s64 b);
proc rng2s64 intersect_2s64(rng2s64 a, rng2s64 b);
proc vec2s64 clamp_2s64(rng2s64 r, vec2s64 v);

#define r2f32(min, max) rng_2f32((min), (max))
#define r2f32p(x, y, z, w) r2f32(v2f32((x), (y)), v2f32((z), (w)))
proc rng2f32 rng_2f32(vec2f32 min, vec2f32 max);
proc rng2f32 shift_2f32(rng2f32 r, vec2f32 x);
proc rng2f32 pad_2f32(rng2f32 r, f32 x);
proc vec2f32 center_2f32(rng2f32 r);
proc b8     contains_2f32(rng2f32 r, vec2f32 x);
proc vec2f32 dim_2f32(rng2f32 r);
proc rng2f32 union_2f32(rng2f32 a, rng2f32 b);
proc rng2f32 intersect_2f32(rng2f32 a, rng2f32 b);
proc vec2f32 clamp_2f32(rng2f32 r, vec2f32 v);

//--------------------------------------------------------------------------------
// miscellaneous ops

proc vec3f32 hsv_from_rgb(vec3f32 rgb);
proc vec3f32 rgb_from_hsv(vec3f32 hsv);
proc vec4f32 hsva_from_rgba(vec4f32 rgba);
proc vec4f32 rgba_from_hsva(vec4f32 hsva);
proc vec4f32 rgba_from_u32(u32 hex);
proc u32 u32_from_rgba(vec4f32 rgba);

#define rgba_from_u32_lit_comp(h) { (((h)&0xff000000)>>24)/255.f, (((h)&0x00ff0000)>>16)/255.f, (((h)&0x0000ff00)>> 8)/255.f, (((h)&0x000000ff)>> 0)/255.f }

//--------------------------------------------------------------------------------
// list type functions

proc void rng1u64_list_push(Arena *arena, rng1u64_list *list, rng1u64 rng);
proc void rng1u64_list_concat(rng1u64_list *list, rng1u64_list *to_concat);
proc rng1u64_array rng1u64_array_from_list(Arena *arena, rng1u64_list *list);

proc void          rng1s64_list_push(Arena *arena, rng1s64_list *list, rng1s64 rng);
proc rng1s64_array rng1s64_array_from_list(Arena *arena, rng1s64_list *list);

#endif // BASE_MATH_H
