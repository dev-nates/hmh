
//--------------------------------------------------------------------------------
// scalar operations

proc f32 mix_f32(f32 a, f32 b, f32 t) {
	return a + (b-a)*clamp(0.f, t, 1.f);
}
proc f64 mix_f64(f64 a, f64 b, f64 t) {
	return a + (b-a)*clamp(0, t, 1);
}

proc s64 abs_s64(s64 v) {
	return (v < 0) ? -v : v;
}


//--------------------------------------------------------------------------------
// vector ops

proc vec2f32 vec_2f32(f32 x, f32 y) { return (vec2f32){x, y}; }
proc vec2f32 add_2f32(vec2f32 a, vec2f32 b) { return (vec2f32){a.x+b.x, a.y+b.y}; }
proc vec2f32 sub_2f32(vec2f32 a, vec2f32 b) { return (vec2f32){a.x-b.x, a.y-b.y}; }
proc vec2f32 mul_2f32(vec2f32 a, vec2f32 b) { return (vec2f32){a.x*b.x, a.y*b.y}; }
proc vec2f32 div_2f32(vec2f32 a, vec2f32 b) { return (vec2f32){a.x/b.x, a.y/b.y}; }
proc vec2f32 scale_2f32(vec2f32 v, f32 s) { return (vec2f32){v.x*s, v.y*s}; }
proc f32 dot_2f32(vec2f32 a, vec2f32 b) { return a.x*b.x + a.y*b.y; }
proc f32 length_squared_2f32(vec2f32 v) { return v.x*v.x + v.y*v.y; }
proc f32 length_2f32(vec2f32 v) { return sqrt_f32(v.x*v.x + v.y*v.y); }
proc vec2f32 normalize_2f32(vec2f32 v) { return scale_2f32(v, 1.f/length_2f32(v)); }
proc vec2f32 mix_2f32(vec2f32 a, vec2f32 b, f32 t) { return (vec2f32){mix_f32(a.x, b.x, t), mix_f32(a.y, b.y, t)}; }

proc vec2s64 vec_2s64(s64 x, s64 y) { return (vec2s64){x, y}; }
proc vec2s64 add_2s64(vec2s64 a, vec2s64 b) { return (vec2s64){a.x+b.x, a.y+b.y}; }
proc vec2s64 sub_2s64(vec2s64 a, vec2s64 b) { return (vec2s64){a.x-b.x, a.y-b.y}; }
proc vec2s64 mul_2s64(vec2s64 a, vec2s64 b) { return (vec2s64){a.x*b.x, a.y*b.y}; }
proc vec2s64 div_2s64(vec2s64 a, vec2s64 b) { return (vec2s64){a.x/b.x, a.y/b.y}; }
proc vec2s64 scale_2s64(vec2s64 v, s64 s) { return (vec2s64){v.x*s, v.y*s}; }
proc s64 dot_2s64(vec2s64 a, vec2s64 b) { return a.x*b.x + a.y*b.y; }
proc s64 length_squared_2s64(vec2s64 v) { return v.x*v.x + v.y*v.y; }
proc s64 length_2s64(vec2s64 v) { return (s64)sqrt_f64((f64)(v.x*v.x + v.y*v.y)); }
proc vec2s64 normalize_2s64(vec2s64 v) { return scale_2s64(v, (s64)(1/(f64)length_2s64(v))); }
proc vec2s64 mix_2s64(vec2s64 a, vec2s64 b, f64 t) { return (vec2s64){(s64)mix_f64((f64)a.x, (f64)b.x, t), (s64)mix_f64((f64)a.y, (f64)b.y, t)}; }

proc vec2s32 vec_2s32(s32 x, s32 y) { return (vec2s32){x, y}; }
proc vec2s32 add_2s32(vec2s32 a, vec2s32 b) { return (vec2s32){a.x+b.x, a.y+b.y}; }
proc vec2s32 sub_2s32(vec2s32 a, vec2s32 b) { return (vec2s32){a.x-b.x, a.y-b.y}; }
proc vec2s32 mul_2s32(vec2s32 a, vec2s32 b) { return (vec2s32){a.x*b.x, a.y*b.y}; }
proc vec2s32 div_2s32(vec2s32 a, vec2s32 b) { return (vec2s32){a.x/b.x, a.y/b.y}; }
proc vec2s32 scale_2s32(vec2s32 v, s32 s) { return (vec2s32){v.x*s, v.y*s}; }
proc s32 dot_2s32(vec2s32 a, vec2s32 b) { return a.x*b.x + a.y*b.y; }
proc s32 length_squared_2s32(vec2s32 v) { return v.x*v.x + v.y*v.y; }
proc s32 length_2s32(vec2s32 v) { return (s32)sqrt_f32((f32)(v.x*v.x + v.y*v.y)); }
proc vec2s32 normalize_2s32(vec2s32 v) { return scale_2s32(v, (s32)(1.f/(f32)length_2s32(v))); }
proc vec2s32 mix_2s32(vec2s32 a, vec2s32 b, f32 t) { return (vec2s32){(s32)mix_f32((f32)a.x, (f32)b.x, t), (s32)mix_f32((f32)a.y, (f32)b.y, t)}; }

proc vec2s16 vec_2s16(s16 x, s16 y) { return (vec2s16){x, y}; }
proc vec2s16 add_2s16(vec2s16 a, vec2s16 b) { return (vec2s16){a.x+b.x, a.y+b.y}; }
proc vec2s16 sub_2s16(vec2s16 a, vec2s16 b) { return (vec2s16){a.x-b.x, a.y-b.y}; }
proc vec2s16 mul_2s16(vec2s16 a, vec2s16 b) { return (vec2s16){a.x*b.x, a.y*b.y}; }
proc vec2s16 div_2s16(vec2s16 a, vec2s16 b) { return (vec2s16){a.x/b.x, a.y/b.y}; }
proc vec2s16 scale_2s16(vec2s16 v, s16 s) { return (vec2s16){v.x*s, v.y*s}; }
proc s16 dot_2s16(vec2s16 a, vec2s16 b) { return a.x*b.x + a.y*b.y; }
proc s16 length_squared_2s16(vec2s16 v) { return v.x*v.x + v.y*v.y; }
proc s16 length_2s16(vec2s16 v) { return (s16)sqrt_f32(v.x*v.x + v.y*v.y); }
proc vec2s16 normalize_2s16(vec2s16 v) { return scale_2s16(v, (s16)(1.f/length_2s16(v))); }
proc vec2s16 mix_2s16(vec2s16 a, vec2s16 b, f32 t) { return (vec2s16){(s16)mix_f32((f32)a.x, (f32)b.x, t), (s16)mix_f32((f32)a.y, (f32)b.y, t)}; }

// vector 3

proc vec3f32 vec_3f32(f32 x, f32 y, f32 z) { return (vec3f32){x, y, z}; }
proc vec3f32 add_3f32(vec3f32 a, vec3f32 b) { return (vec3f32){a.x+b.x, a.y+b.y, a.z+b.z}; }
proc vec3f32 sub_3f32(vec3f32 a, vec3f32 b) { return (vec3f32){a.x-b.x, a.y-b.y, a.z-b.z}; }
proc vec3f32 mul_3f32(vec3f32 a, vec3f32 b) { return (vec3f32){a.x*b.x, a.y*b.y, a.z*b.z}; }
proc vec3f32 div_3f32(vec3f32 a, vec3f32 b) { return (vec3f32){a.x/b.x, a.y/b.y, a.z/b.z}; }
proc vec3f32 scale_3f32(vec3f32 v, f32 s) { return (vec3f32){v.x*s, v.y*s, v.z*s}; }
proc f32 dot_3f32(vec3f32 a, vec3f32 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
proc f32 length_squared_3f32(vec3f32 v) { return v.x*v.x + v.y*v.y + v.z*v.z; }
proc f32 length_3f32(vec3f32 v) { return sqrt_f32(v.x*v.x + v.y*v.y + v.z*v.z); }
proc vec3f32 normalize_3f32(vec3f32 v) { return scale_3f32(v, 1.f/length_3f32(v)); }
proc vec3f32 mix_3f32(vec3f32 a, vec3f32 b, f32 t) { return (vec3f32){mix_f32(a.x, b.x, t), mix_f32(a.y, b.y, t), mix_f32(a.z, b.z, t)}; }
proc vec3f32 cross_3f32(vec3f32 a, vec3f32 b) { return (vec3f32){a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x}; }

proc vec3s32 vec_3s32(s32 x, s32 y, s32 z) { return (vec3s32){x, y, z}; }
proc vec3s32 add_3s32(vec3s32 a, vec3s32 b) { return (vec3s32){a.x+b.x, a.y+b.y, a.z+b.z}; }
proc vec3s32 sub_3s32(vec3s32 a, vec3s32 b) { return (vec3s32){a.x-b.x, a.y-b.y, a.z-b.z}; }
proc vec3s32 mul_3s32(vec3s32 a, vec3s32 b) { return (vec3s32){a.x*b.x, a.y*b.y, a.z*b.z}; }
proc vec3s32 div_3s32(vec3s32 a, vec3s32 b) { return (vec3s32){a.x/b.x, a.y/b.y, a.z/b.z}; }
proc vec3s32 scale_3s32(vec3s32 v, s32 s) { return (vec3s32){v.x*s, v.y*s, v.z*s}; }
proc s32 dot_3s32(vec3s32 a, vec3s32 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
proc s32 length_squared_3s32(vec3s32 v) { return v.x*v.x + v.y*v.y + v.z*v.z; }
proc s32 length_3s32(vec3s32 v) { return (s32)sqrt_f32((f32)(v.x*v.x + v.y*v.y + v.z*v.z)); }
proc vec3s32 normalize_3s32(vec3s32 v) { return scale_3s32(v, (s32)(1.f/(f32)length_3s32(v))); }
proc vec3s32 mix_3s32(vec3s32 a, vec3s32 b, f32 t) { return (vec3s32){(s32)mix_f32((f32)a.x, (f32)b.x, t), (s32)mix_f32((f32)a.y, (f32)b.y, t), (s32)mix_f32((f32)a.z, (f32)b.z, t)}; }
proc vec3s32 cross_3s32(vec3s32 a, vec3s32 b) { return (vec3s32){a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x}; }

// vector 4
proc vec4f32 vec_4f32(f32 x, f32 y, f32 z, f32 w) { return (vec4f32){x, y, z, w}; }
proc vec4f32 add_4f32(vec4f32 a, vec4f32 b) { return (vec4f32){a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w}; }
proc vec4f32 sub_4f32(vec4f32 a, vec4f32 b) { return (vec4f32){a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w}; }
proc vec4f32 mul_4f32(vec4f32 a, vec4f32 b) { return (vec4f32){a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w}; }
proc vec4f32 div_4f32(vec4f32 a, vec4f32 b) { return (vec4f32){a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w}; }
proc vec4f32 scale_4f32(vec4f32 v, f32 s) { return (vec4f32){v.x*s, v.y*s, v.z*s, v.w*s}; }
proc f32 dot_4f32(vec4f32 a, vec4f32 b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }
proc f32 length_squared_4f32(vec4f32 v) { return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w; }
proc f32 length_4f32(vec4f32 v) { return sqrt_f32(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w); }
proc vec4f32 normalize_4f32(vec4f32 v) { return scale_4f32(v, 1.f/length_4f32(v)); }
proc vec4f32 mix_4f32(vec4f32 a, vec4f32 b, f32 t) { return (vec4f32){mix_f32(a.x, b.x, t), mix_f32(a.y, b.y, t), .z  = mix_f32(a.z, b.z, t), mix_f32(a.w, b.w, t)}; }

proc vec4s32 vec_4s32(s32 x, s32 y, s32 z, s32 w) { return (vec4s32){x, y, z, w}; }
proc vec4s32 add_4s32(vec4s32 a, vec4s32 b) { return (vec4s32){a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w}; }
proc vec4s32 sub_4s32(vec4s32 a, vec4s32 b) { return (vec4s32){a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w}; }
proc vec4s32 mul_4s32(vec4s32 a, vec4s32 b) { return (vec4s32){a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w}; }
proc vec4s32 div_4s32(vec4s32 a, vec4s32 b) { return (vec4s32){a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w}; }
proc vec4s32 scale_4s32(vec4s32 v, s32 s) { return (vec4s32){v.x*s, v.y*s, v.z*s, v.w*s}; }
proc s32 dot_4s32(vec4s32 a, vec4s32 b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }
proc s32 length_squared_4s32(vec4s32 v) { return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w; }
proc s32 length_4s32(vec4s32 v) { return (s32)sqrt_f32((f32)(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w)); }
proc vec4s32 normalize_4s32(vec4s32 v) { return scale_4s32(v, (s32)(1.f/(f32)length_4s32(v))); }
proc vec4s32 mix_4s32(vec4s32 a, vec4s32 b, f32 t) { return (vec4s32){(s32)mix_f32((f32)a.x, (f32)b.x, t), (s32)mix_f32((f32)a.y, (f32)b.y, t), .z  = (s32)mix_f32((f32)a.z, (f32)b.z, t), (s32)mix_f32((f32)a.w, (f32)b.w, t)}; }

//--------------------------------------------------------------------------------
// matrix ops

// matrix 3
proc mat3x3f32 mat_3x3f32(f32 diagonal) {
	mat3x3f32 result = zero_struct;
	result.v[0][0] = diagonal;
	result.v[1][1] = diagonal;
	result.v[2][2] = diagonal;
	return result;
}
proc mat3x3f32 mk_translate_3x3f32(vec2f32 delta) {
	mat3x3f32 result = mat_3x3f32(1.f);
	result.v[0][2] = delta.x;
	result.v[1][2] = delta.y;
	return result;
}
proc mat3x3f32 mk_scale_3x3f32(vec2f32 scale) {
	mat3x3f32 result = mat_3x3f32(1.f);
	result.v[0][0] = scale.x;
	result.v[1][1] = scale.x;
	return result;
}
proc mat3x3f32 mul_3x3f32(mat3x3f32 a, mat3x3f32 b) {
	mat3x3f32 result;
	for (s32 r = 0; r < 3; ++r)
	for (s32 c = 0; c < 3; ++c) {
		result.v[r][c] =
			a.v[r][0]*b.v[0][c] +
			a.v[r][1]*b.v[1][c] +
			a.v[r][2]*b.v[2][c];
	}
	return result;
}

// matrix 4
proc mat4x4f32 mat_4x4f32(f32 diagonal) {
	mat4x4f32 result = zero_struct;
	result.v[0][0] = diagonal;
	result.v[1][1] = diagonal;
	result.v[2][2] = diagonal;
	result.v[3][3] = diagonal;
	return result;
}
proc mat4x4f32 mk_translate_4x4f32(vec3f32 delta) {
	mat4x4f32 result = mat_4x4f32(1.f);
	result.v[3][0] = delta.x;
	result.v[3][1] = delta.y;
	result.v[3][2] = delta.z;
	return result;
}
proc mat4x4f32 mk_scale_4x4f32(vec3f32 scale) {
	mat4x4f32 result = mat_4x4f32(1.f);
	result.v[0][0] = scale.x;
	result.v[1][1] = scale.x;
	result.v[2][2] = scale.x;
	return result;
}
proc mat4x4f32 mk_perspective_4x4f32(f32 fov, f32 aspect_ratio, f32 near_z, f32 far_z) {
	// IMPORTANT NOTE ns: This code is subject to change depending on how you want the projection to deal with
	// 2 factors: (depth range projection && z is negative or positive)
	// depth could be: normal_depth(0 -> 1) -- reverse_depth(1 -> 0) -- less_precision_depth(-1 -> 1) --- NOTE eg (0 -> 1) means (near -> far)
	// Input z values that multiply with [2][2] can be:
	//  right handed coord system(negative) or left handed coord system (positive).
	// ns: right handed (negative) reverse depth (1 -> 0) for vulkan

	mat4x4f32 result = mat_4x4f32(1.f);
	f32 tan_half_theta = tan_f32(fov/2.f);
	// IMPORTANT NOTE ns: We expect z value of vector multipled with this to be negative
	// near_z && far_z shouldn't be negative otherwise the equation would be wrong.
	assert(near_z >= 0.f && far_z >= 0.f);
	f32 n = near_z, f = far_z;
	f32 c = n / (n - f);
	f32 a = c;
	f32 b = f*c;
	result.v[0][0] = 1.f / tan_half_theta;
	result.v[1][1] = aspect_ratio / tan_half_theta;
	result.v[2][3] = 1.f;
	result.v[2][2] = a;
	result.v[3][2] = b;
	result.v[3][3] = 0.f;
	return result;
}
proc mat4x4f32 mk_orthographic_4x4f32(f32 left, f32 right, f32 bottom, f32 top, f32 near_z, f32 far_z) {
	mat4x4f32 result = mat_4x4f32(1.f);

	assert(near_z >= 0.f && far_z >= 0.f);
	f32 n = near_z, f = far_z;
	f32 c = n / (n - f);
	f32 a = c;
	f32 b = f*c;
	result.v[0][0] = 2.f / (right - left);
	result.v[1][1] = 2.f / (top - bottom);
	result.v[2][2] = 2.f / (far_z - near_z);
	result.v[2][2] = a;
	result.v[3][2] = b;
	result.v[3][3] = 1.f;
	return result;
}
proc mat4x4f32 mk_look_at_4x4f32(vec3f32 eye, vec3f32 center, vec3f32 up) {
	mat4x4f32 result;
	vec3f32 f = normalize_3f32(sub_3f32(eye, center));
	vec3f32 s = normalize_3f32(cross_3f32(f, up));
	vec3f32 u = cross_3f32(s, f);
	result.v[0][0] = s.x;
	result.v[1][0] = s.y;
	result.v[2][0] = s.z;
	result.v[0][1] = u.x;
	result.v[1][1] = u.y;
	result.v[2][1] = u.z;
	result.v[0][2] = -f.x;
	result.v[1][2] = -f.y;
	result.v[2][2] = -f.z;
	result.v[0][3] = 0.0f;
	result.v[1][3] = 0.0f;
	result.v[2][3] = 0.0f;
	result.v[3][0] = -dot_3f32(s, eye);
	result.v[3][1] = -dot_3f32(u, eye);
	result.v[3][2] = dot_3f32(f, eye);
	result.v[3][3] = 1.0f;
	return result;
}
proc mat4x4f32 mk_rotate_4x4f32(vec3f32 axis, f32 turns) {
	mat4x4f32 result = mat_4x4f32(1.f);
	axis = normalize_3f32(axis);
	f32 sin_theta = sin_f32(turns);
	f32 cos_theta = cos_f32(turns);
	f32 cos_value = 1.f - cos_theta;
	result.v[0][0] = (axis.x * axis.x * cos_value) + cos_theta;
	result.v[0][1] = (axis.x * axis.y * cos_value) + (axis.z * sin_theta);
	result.v[0][2] = (axis.x * axis.z * cos_value) - (axis.y * sin_theta);
	result.v[1][0] = (axis.y * axis.x * cos_value) - (axis.z * sin_theta);
	result.v[1][1] = (axis.y * axis.y * cos_value) + cos_theta;
	result.v[1][2] = (axis.y * axis.z * cos_value) + (axis.x * sin_theta);
	result.v[2][0] = (axis.z * axis.x * cos_value) + (axis.y * sin_theta);
	result.v[2][1] = (axis.z * axis.y * cos_value) - (axis.x * sin_theta);
	result.v[2][2] = (axis.z * axis.z * cos_value) + cos_theta;
	return result;
}
proc mat4x4f32 mul_4x4f32(mat4x4f32 a, mat4x4f32 b) {
	mat4x4f32 result;
	for (s32 r = 0; r < 4; ++r)
	for (s32 c = 0; c < 4; ++c) {
		result.v[r][c] =
			a.v[r][0]*b.v[0][c] +
			a.v[r][1]*b.v[1][c] +
			a.v[r][2]*b.v[2][c] +
			a.v[r][3]*b.v[3][c];
	}
	return result;
}
proc mat4x4f32 scale_4x4f32(mat4x4f32 m, f32 scale) {
	for (s32 r = 0; r < 4; ++r)
	for (s32 c = 0; c < 4; ++c) {
		m.v[r][c] *= scale;
	}
	return m;
}
proc mat4x4f32 inverse_4x4f32(mat4x4f32 m) {
	f32 coef00 = m.v[2][2] * m.v[3][3] - m.v[3][2] * m.v[2][3];
	f32 coef02 = m.v[1][2] * m.v[3][3] - m.v[3][2] * m.v[1][3];
	f32 coef03 = m.v[1][2] * m.v[2][3] - m.v[2][2] * m.v[1][3];
	f32 coef04 = m.v[2][1] * m.v[3][3] - m.v[3][1] * m.v[2][3];
	f32 coef06 = m.v[1][1] * m.v[3][3] - m.v[3][1] * m.v[1][3];
	f32 coef07 = m.v[1][1] * m.v[2][3] - m.v[2][1] * m.v[1][3];
	f32 coef08 = m.v[2][1] * m.v[3][2] - m.v[3][1] * m.v[2][2];
	f32 coef10 = m.v[1][1] * m.v[3][2] - m.v[3][1] * m.v[1][2];
	f32 coef11 = m.v[1][1] * m.v[2][2] - m.v[2][1] * m.v[1][2];
	f32 coef12 = m.v[2][0] * m.v[3][3] - m.v[3][0] * m.v[2][3];
	f32 coef14 = m.v[1][0] * m.v[3][3] - m.v[3][0] * m.v[1][3];
	f32 coef15 = m.v[1][0] * m.v[2][3] - m.v[2][0] * m.v[1][3];
	f32 coef16 = m.v[2][0] * m.v[3][2] - m.v[3][0] * m.v[2][2];
	f32 coef18 = m.v[1][0] * m.v[3][2] - m.v[3][0] * m.v[1][2];
	f32 coef19 = m.v[1][0] * m.v[2][2] - m.v[2][0] * m.v[1][2];
	f32 coef20 = m.v[2][0] * m.v[3][1] - m.v[3][0] * m.v[2][1];
	f32 coef22 = m.v[1][0] * m.v[3][1] - m.v[3][0] * m.v[1][1];
	f32 coef23 = m.v[1][0] * m.v[2][1] - m.v[2][0] * m.v[1][1];

	vec4f32 fac0 = { coef00, coef00, coef02, coef03 };
	vec4f32 fac1 = { coef04, coef04, coef06, coef07 };
	vec4f32 fac2 = { coef08, coef08, coef10, coef11 };
	vec4f32 fac3 = { coef12, coef12, coef14, coef15 };
	vec4f32 fac4 = { coef16, coef16, coef18, coef19 };
	vec4f32 fac5 = { coef20, coef20, coef22, coef23 };

	vec4f32 vec0 = { m.v[1][0], m.v[0][0], m.v[0][0], m.v[0][0] };
	vec4f32 vec1 = { m.v[1][1], m.v[0][1], m.v[0][1], m.v[0][1] };
	vec4f32 vec2 = { m.v[1][2], m.v[0][2], m.v[0][2], m.v[0][2] };
	vec4f32 vec3 = { m.v[1][3], m.v[0][3], m.v[0][3], m.v[0][3] };

	vec4f32 inv0 = add_4f32(sub_4f32(mul_4f32(vec1, fac0), mul_4f32(vec2, fac1)), mul_4f32(vec3, fac2));
	vec4f32 inv1 = add_4f32(sub_4f32(mul_4f32(vec0, fac0), mul_4f32(vec2, fac3)), mul_4f32(vec3, fac4));
	vec4f32 inv2 = add_4f32(sub_4f32(mul_4f32(vec0, fac1), mul_4f32(vec1, fac3)), mul_4f32(vec3, fac5));
	vec4f32 inv3 = add_4f32(sub_4f32(mul_4f32(vec0, fac2), mul_4f32(vec1, fac4)), mul_4f32(vec2, fac5));

	vec4f32 sign_a = { +1, -1, +1, -1 };
	vec4f32 sign_b = { -1, +1, -1, +1 };

	mat4x4f32 inverse;
	for(s32 i = 0; i < 4; i += 1)
	{
		inverse.v[0][i] = inv0.v[i] * sign_a.v[i];
		inverse.v[1][i] = inv1.v[i] * sign_b.v[i];
		inverse.v[2][i] = inv2.v[i] * sign_a.v[i];
		inverse.v[3][i] = inv3.v[i] * sign_b.v[i];
	}

	vec4f32 row0 = { inverse.v[0][0], inverse.v[1][0], inverse.v[2][0], inverse.v[3][0] };
	vec4f32 m0 = { m.v[0][0], m.v[0][1], m.v[0][2], m.v[0][3] };
	vec4f32 dot0 = mul_4f32(m0, row0);
	f32 dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);

	f32 one_over_det = 1 / dot1;

	return scale_4x4f32(inverse, one_over_det);
}
proc mat4x4f32 derotate_4x4f32(mat4x4f32 m) {
	vec3f32 scale =
	{
	  length_3f32(v3f32(m.v[0][0], m.v[0][1], m.v[0][2])),
	  length_3f32(v3f32(m.v[1][0], m.v[1][1], m.v[1][2])),
	  length_3f32(v3f32(m.v[2][0], m.v[2][1], m.v[2][2])),
	};
	m.v[0][0] = scale.x;
	m.v[1][0] = 0.f;
	m.v[2][0] = 0.f;
	m.v[0][1] = 0.f;
	m.v[1][1] = scale.y;
	m.v[2][1] = 0.f;
	m.v[0][2] = 0.f;
	m.v[1][2] = 0.f;
	m.v[2][2] = scale.z;
	return m;
}

//--------------------------------------------------------------------------------
// single axis range ops

proc rng1u32 rng_1u32(u32 min, u32 max)                     {rng1u32 r = {min, max}; if(r.min > r.max) { swap(u32, r.min, r.max); } return r;}
proc rng1u32 shift_1u32(rng1u32 r, u32 x)                   {r.min += x; r.max += x; return r;}
proc rng1u32 pad_1u32(rng1u32 r, u32 x)                     {r.min -= x; r.max += x; return r;}
proc u32 center_1u32(rng1u32 r)                             {u32 c = (r.min+r.max)/2; return c;}
proc b8 contains_1u32(rng1u32 r, u32 x)                     {b8 c = (r.min <= x && x < r.max); return c;}
proc u32 dim_1u32(rng1u32 r)                                {u32 c = ((r.max > r.min) ? (r.max - r.min) : 0); return c;}
proc rng1u32 union_1u32(rng1u32 a, rng1u32 b)               {rng1u32 c = {min(a.min, b.min), max(a.max, b.max)}; return c;}
proc rng1u32 intersect_1u32(rng1u32 a, rng1u32 b)           {rng1u32 c = {max(a.min, b.min), min(a.max, b.max)}; return c;}
proc u32 clamp_1u32(rng1u32 r, u32 v)                       {v = clamp(r.min, v, r.max); return v;}

proc rng1s32 rng_1s32(s32 min, s32 max)                     {rng1s32 r = {min, max}; if(r.min > r.max) { swap(s32, r.min, r.max); } return r;}
proc rng1s32 shift_1s32(rng1s32 r, s32 x)                   {r.min += x; r.max += x; return r;}
proc rng1s32 pad_1s32(rng1s32 r, s32 x)                     {r.min -= x; r.max += x; return r;}
proc s32 center_1s32(rng1s32 r)                             {s32 c = (r.min+r.max)/2; return c;}
proc b8 contains_1s32(rng1s32 r, s32 x)                     {b8 c = (r.min <= x && x < r.max); return c;}
proc s32 dim_1s32(rng1s32 r)                                {s32 c = ((r.max > r.min) ? (r.max - r.min) : 0); return c;}
proc rng1s32 union_1s32(rng1s32 a, rng1s32 b)               {rng1s32 c = {min(a.min, b.min), max(a.max, b.max)}; return c;}
proc rng1s32 intersect_1s32(rng1s32 a, rng1s32 b)           {rng1s32 c = {max(a.min, b.min), min(a.max, b.max)}; return c;}
proc s32 clamp_1s32(rng1s32 r, s32 v)                       {v = clamp(r.min, v, r.max); return v;}

proc rng1u64 rng_1u64(u64 min, u64 max)                     {rng1u64 r = {min, max}; if(r.min > r.max) { swap(u64, r.min, r.max); } return r;}
proc rng1u64 shift_1u64(rng1u64 r, u64 x)                   {r.min += x; r.max += x; return r;}
proc rng1u64 pad_1u64(rng1u64 r, u64 x)                     {r.min -= x; r.max += x; return r;}
proc u64 center_1u64(rng1u64 r)                             {u64 c = (r.min+r.max)/2; return c;}
proc b8 contains_1u64(rng1u64 r, u64 x)                     {b8 c = (r.min <= x && x < r.max); return c;}
proc u64 dim_1u64(rng1u64 r)                                {u64 c = ((r.max > r.min) ? (r.max - r.min) : 0); return c;}
proc rng1u64 union_1u64(rng1u64 a, rng1u64 b)               {rng1u64 c = {min(a.min, b.min), max(a.max, b.max)}; return c;}
proc rng1u64 intersect_1u64(rng1u64 a, rng1u64 b)           {rng1u64 c = {max(a.min, b.min), min(a.max, b.max)}; return c;}
proc u64 clamp_1u64(rng1u64 r, u64 v)                       {v = clamp(r.min, v, r.max); return v;}

proc rng1s64 rng_1s64(s64 min, s64 max)                     {rng1s64 r = {min, max}; if(r.min > r.max) { swap(s64, r.min, r.max); } return r;}
proc rng1s64 shift_1s64(rng1s64 r, s64 x)                   {r.min += x; r.max += x; return r;}
proc rng1s64 pad_1s64(rng1s64 r, s64 x)                     {r.min -= x; r.max += x; return r;}
proc s64 center_1s64(rng1s64 r)                             {s64 c = (r.min+r.max)/2; return c;}
proc b8 contains_1s64(rng1s64 r, s64 x)                     {b8 c = (r.min <= x && x < r.max); return c;}
proc s64 dim_1s64(rng1s64 r)                                {s64 c = ((r.max > r.min) ? (r.max - r.min) : 0); return c;}
proc rng1s64 union_1s64(rng1s64 a, rng1s64 b)               {rng1s64 c = {min(a.min, b.min), max(a.max, b.max)}; return c;}
proc rng1s64 intersect_1s64(rng1s64 a, rng1s64 b)           {rng1s64 c = {max(a.min, b.min), min(a.max, b.max)}; return c;}
proc s64 clamp_1s64(rng1s64 r, s64 v)                       {v = clamp(r.min, v, r.max); return v;}

proc rng1f32 rng_1f32(f32 min, f32 max)                     {rng1f32 r = {min, max}; if(r.min > r.max) { swap(f32, r.min, r.max); } return r;}
proc rng1f32 shift_1f32(rng1f32 r, f32 x)                   {r.min += x; r.max += x; return r;}
proc rng1f32 pad_1f32(rng1f32 r, f32 x)                     {r.min -= x; r.max += x; return r;}
proc f32 center_1f32(rng1f32 r)                             {f32 c = (r.min+r.max)/2.f; return c;}
proc b8 contains_1f32(rng1f32 r, f32 x)                     {b8 c = (r.min <= x && x < r.max); return c;}
proc f32 dim_1f32(rng1f32 r)                                {f32 c = ((r.max > r.min) ? (r.max - r.min) : 0.f); return c;}
proc rng1f32 union_1f32(rng1f32 a, rng1f32 b)               {rng1f32 c = {min(a.min, b.min), max(a.max, b.max)}; return c;}
proc rng1f32 intersect_1f32(rng1f32 a, rng1f32 b)           {rng1f32 c = {max(a.min, b.min), min(a.max, b.max)}; return c;}
proc f32 clamp_1f32(rng1f32 r, f32 v)                       {v = clamp(r.min, v, r.max); return v;}

proc rng1f64 rng_1f64(f64 min, f64 max)                     {rng1f64 r = {min, max}; if(r.min > r.max) { swap(f64, r.min, r.max); } return r;}
proc rng1f64 shift_1f64(rng1f64 r, f64 x)                   {r.min += x; r.max += x; return r;}
proc rng1f64 pad_1f64(rng1f64 r, f64 x)                     {r.min -= x; r.max += x; return r;}
proc f64 center_1f64(rng1f64 r)                             {f64 c = (r.min+r.max)/2.0; return c;}
proc b8 contains_1f64(rng1f64 r, f64 x)                     {b8 c = (r.min <= x && x < r.max); return c;}
proc f64 dim_1f64(rng1f64 r)                                {f64 c = ((r.max > r.min) ? (r.max - r.min) : 0.0); return c;}
proc rng1f64 union_1f64(rng1f64 a, rng1f64 b)               {rng1f64 c = {min(a.min, b.min), max(a.max, b.max)}; return c;}
proc rng1f64 intersect_1f64(rng1f64 a, rng1f64 b)           {rng1f64 c = {max(a.min, b.min), min(a.max, b.max)}; return c;}
proc f64 clamp_1f64(rng1f64 r, f64 v)                       {v = clamp(r.min, v, r.max); return v;}

//--------------------------------------------------------------------------------
// double axis range ops

proc rng2s16 rng_2s16(vec2s16 min, vec2s16 max)             {rng2s16 r = {min, max}; return r;}
proc rng2s16 shift_2s16(rng2s16 r, vec2s16 x)               {r.min = add_2s16(r.min, x); r.max = add_2s16(r.max, x); return r;}
proc rng2s16 pad_2s16(rng2s16 r, s16 x)                     {vec2s16 xv = {x, x}; r.min = sub_2s16(r.min, xv); r.max = add_2s16(r.max, xv); return r;}
proc vec2s16 center_2s16(rng2s16 r)                         {vec2s16 c = {(s16)((r.min.x+r.max.x)/2), (s16)((r.min.y+r.max.y)/2)}; return c;}
proc b8 contains_2s16(rng2s16 r, vec2s16 x)                 {b8 c = (r.min.x <= x.x && x.x < r.max.x && r.min.y <= x.y && x.y < r.max.y); return c;}
proc vec2s16 dim_2s16(rng2s16 r)                            {vec2s16 dim = {(s16)(((r.max.x > r.min.x) ? (r.max.x - r.min.x) : 0)), (s16)(((r.max.y > r.min.y) ? (r.max.y - r.min.y) : 0))}; return dim;}
proc rng2s16 union_2s16(rng2s16 a, rng2s16 b)               {rng2s16 c; c.p0.x = min(a.min.x, b.min.x); c.p0.y = min(a.min.y, b.min.y); c.p1.x = max(a.max.x, b.max.x); c.p1.y = max(a.max.y, b.max.y); return c;}
proc rng2s16 intersect_2s16(rng2s16 a, rng2s16 b)           {rng2s16 c; c.p0.x = max(a.min.x, b.min.x); c.p0.y = max(a.min.y, b.min.y); c.p1.x = min(a.max.x, b.max.x); c.p1.y = min(a.max.y, b.max.y); return c;}
proc vec2s16 clamp_2s16(rng2s16 r, vec2s16 v)               {v.x = clamp(r.min.x, v.x, r.max.x); v.y = clamp(r.min.y, v.y, r.max.y); return v;}

proc rng2s32 rng_2s32(vec2s32 min, vec2s32 max)             {rng2s32 r = {min, max}; return r;}
proc rng2s32 shift_2s32(rng2s32 r, vec2s32 x)               {r.min = add_2s32(r.min, x); r.max = add_2s32(r.max, x); return r;}
proc rng2s32 pad_2s32(rng2s32 r, s32 x)                     {vec2s32 xv = {x, x}; r.min = sub_2s32(r.min, xv); r.max = add_2s32(r.max, xv); return r;}
proc vec2s32 center_2s32(rng2s32 r)                         {vec2s32 c = {(r.min.x+r.max.x)/2, (r.min.y+r.max.y)/2}; return c;}
proc b8 contains_2s32(rng2s32 r, vec2s32 x)                 {b8 c = (r.min.x <= x.x && x.x < r.max.x && r.min.y <= x.y && x.y < r.max.y); return c;}
proc vec2s32 dim_2s32(rng2s32 r)                            {vec2s32 dim = {((r.max.x > r.min.x) ? (r.max.x - r.min.x) : 0), ((r.max.y > r.min.y) ? (r.max.y - r.min.y) : 0)}; return dim;}
proc rng2s32 union_2s32(rng2s32 a, rng2s32 b)               {rng2s32 c; c.p0.x = min(a.min.x, b.min.x); c.p0.y = min(a.min.y, b.min.y); c.p1.x = max(a.max.x, b.max.x); c.p1.y = max(a.max.y, b.max.y); return c;}
proc rng2s32 intersect_2s32(rng2s32 a, rng2s32 b)           {rng2s32 c; c.p0.x = max(a.min.x, b.min.x); c.p0.y = max(a.min.y, b.min.y); c.p1.x = min(a.max.x, b.max.x); c.p1.y = min(a.max.y, b.max.y); return c;}
proc vec2s32 clamp_2s32(rng2s32 r, vec2s32 v)               {v.x = clamp(r.min.x, v.x, r.max.x); v.y = clamp(r.min.y, v.y, r.max.y); return v;}

proc rng2s64 rng_2s64(vec2s64 min, vec2s64 max)             {rng2s64 r = {min, max}; return r;}
proc rng2s64 shift_2s64(rng2s64 r, vec2s64 x)               {r.min = add_2s64(r.min, x); r.max = add_2s64(r.max, x); return r;}
proc rng2s64 pad_2s64(rng2s64 r, s64 x)                     {vec2s64 xv = {x, x}; r.min = sub_2s64(r.min, xv); r.max = add_2s64(r.max, xv); return r;}
proc vec2s64 center_2s64(rng2s64 r)                         {vec2s64 c = {(r.min.x+r.max.x)/2, (r.min.y+r.max.y)/2}; return c;}
proc b8 contains_2s64(rng2s64 r, vec2s64 x)                 {b8 c = (r.min.x <= x.x && x.x < r.max.x && r.min.y <= x.y && x.y < r.max.y); return c;}
proc vec2s64 dim_2s64(rng2s64 r)                            {vec2s64 dim = {((r.max.x > r.min.x) ? (r.max.x - r.min.x) : 0), ((r.max.y > r.min.y) ? (r.max.y - r.min.y) : 0)}; return dim;}
proc rng2s64 union_2s64(rng2s64 a, rng2s64 b)               {rng2s64 c; c.p0.x = min(a.min.x, b.min.x); c.p0.y = min(a.min.y, b.min.y); c.p1.x = max(a.max.x, b.max.x); c.p1.y = max(a.max.y, b.max.y); return c;}
proc rng2s64 intersect_2s64(rng2s64 a, rng2s64 b)           {rng2s64 c; c.p0.x = max(a.min.x, b.min.x); c.p0.y = max(a.min.y, b.min.y); c.p1.x = min(a.max.x, b.max.x); c.p1.y = min(a.max.y, b.max.y); return c;}
proc vec2s64 clamp_2s64(rng2s64 r, vec2s64 v)               {v.x = clamp(r.min.x, v.x, r.max.x); v.y = clamp(r.min.y, v.y, r.max.y); return v;}

proc rng2f32 rng_2f32(vec2f32 min, vec2f32 max)             {rng2f32 r = {min, max}; return r;}
proc rng2f32 shift_2f32(rng2f32 r, vec2f32 x)               {r.min = add_2f32(r.min, x); r.max = add_2f32(r.max, x); return r;}
proc rng2f32 pad_2f32(rng2f32 r, f32 x)                     {vec2f32 xv = {x, x}; r.min = sub_2f32(r.min, xv); r.max = add_2f32(r.max, xv); return r;}
proc vec2f32 center_2f32(rng2f32 r)                         {vec2f32 c = {(r.min.x+r.max.x)/2.f, (r.min.y+r.max.y)/2.f}; return c;}
proc b8 contains_2f32(rng2f32 r, vec2f32 x)                 {b8 c = (r.min.x <= x.x && x.x < r.max.x && r.min.y <= x.y && x.y < r.max.y); return c;}
proc vec2f32 dim_2f32(rng2f32 r)                            {vec2f32 dim = {((r.max.x > r.min.x) ? (r.max.x - r.min.x) : 0.f), ((r.max.y > r.min.y) ? (r.max.y - r.min.y) : 0)}; return dim;}
proc rng2f32 union_2f32(rng2f32 a, rng2f32 b)               {rng2f32 c; c.p0.x = min(a.min.x, b.min.x); c.p0.y = min(a.min.y, b.min.y); c.p1.x = max(a.max.x, b.max.x); c.p1.y = max(a.max.y, b.max.y); return c;}
proc rng2f32 intersect_2f32(rng2f32 a, rng2f32 b)           {rng2f32 c; c.p0.x = max(a.min.x, b.min.x); c.p0.y = max(a.min.y, b.min.y); c.p1.x = min(a.max.x, b.max.x); c.p1.y = min(a.max.y, b.max.y); return c;}
proc vec2f32 clamp_2f32(rng2f32 r, vec2f32 v)               {v.x = clamp(r.min.x, v.x, r.max.x); v.y = clamp(r.min.y, v.y, r.max.y); return v;}

//--------------------------------------------------------------------------------
// miscellaneous ops

proc vec3f32
hsv_from_rgb(vec3f32 rgb)
{
	f32 c_max = max(rgb.x, max(rgb.y, rgb.z));
	f32 c_min = min(rgb.x, min(rgb.y, rgb.z));
	f32 delta = c_max - c_min;
	f32 h = ((delta == 0.f) ? 0.f :
		(c_max == rgb.x) ? mod_f32((rgb.y - rgb.z)/delta + 6.f, 6.f) :
		(c_max == rgb.y) ? (rgb.z - rgb.x)/delta + 2.f :
		(c_max == rgb.z) ? (rgb.x - rgb.y)/delta + 4.f :
		0.f);
	f32 s = (c_max == 0.f) ? 0.f : (delta/c_max);
	f32 v = c_max;
	vec3f32 hsv = {h/6.f, s, v};
	return hsv;
}

proc vec3f32
rgb_from_hsv(vec3f32 hsv)
{
	f32 h = mod_f32(hsv.x * 360.f, 360.f);
	f32 s = hsv.y;
	f32 v = hsv.z;

	f32 c = v*s;
	f32 x = c*(1.f - abs_f32(mod_f32(h/60.f, 2.f) - 1.f));
	f32 m = v - c;

	f32 r = 0;
	f32 g = 0;
	f32 b = 0;

	if ((h >= 0.f && h < 60.f) || (h >= 360.f && h < 420.f)){
		r = c;
		g = x;
		b = 0;
	}
	else if (h >= 60.f && h < 120.f){
		r = x;
		g = c;
		b = 0;
	}
	else if (h >= 120.f && h < 180.f){
		r = 0;
		g = c;
		b = x;
	}
	else if (h >= 180.f && h < 240.f){
		r = 0;
		g = x;
		b = c;
	}
	else if (h >= 240.f && h < 300.f){
		r = x;
		g = 0;
		b = c;
	}
	else if ((h >= 300.f && h <= 360.f) || (h >= -60.f && h <= 0.f)){
		r = c;
		g = 0;
		b = x;
	}

	vec3f32 rgb = {r + m, g + m, b + m};
	return(rgb);
}

proc vec4f32
hsva_from_rgba(vec4f32 rgba)
{
	vec3f32 rgb = v3f32(rgba.x, rgba.y, rgba.z);
	vec3f32 hsv = hsv_from_rgb(rgb);
	vec4f32 hsva = v4f32(hsv.x, hsv.y, hsv.z, rgba.w);
	return hsva;
}

proc vec4f32
rgba_from_hsva(vec4f32 hsva)
{
	vec3f32 hsv = v3f32(hsva.x, hsva.y, hsva.z);
	vec3f32 rgb = rgb_from_hsv(hsv);
	vec4f32 rgba = v4f32(rgb.x, rgb.y, rgb.z, hsva.w);
	return rgba;
}

proc vec4f32
rgba_from_u32(u32 hex)
{
	vec4f32 result = v4f32(
		((hex&0xff000000)>>24)/255.f,
		((hex&0x00ff0000)>>16)/255.f,
		((hex&0x0000ff00)>> 8)/255.f,
		((hex&0x000000ff)>> 0)/255.f);
	return result;
}

proc u32
u32_from_rgba(vec4f32 rgba)
{
	u32 result = 0;
	result |= ((u32)((u8)(rgba.x*255.f))) << 24;
	result |= ((u32)((u8)(rgba.y*255.f))) << 16;
	result |= ((u32)((u8)(rgba.z*255.f))) <<  8;
	result |= ((u32)((u8)(rgba.w*255.f))) <<  0;
	return result;
}

//--------------------------------------------------------------------------------
// list type functions

proc void
rng1u64_list_push(Arena *arena, rng1u64_list *list, rng1u64 rng)
{
	rng1u64_node *node = push_array(arena, rng1u64_node, 1);
	memory_copy_struct(&node->v, &rng);

	rng1u64_node **ptr = check_nil(list->head, nil) ? &list->head : &list->tail->next;
	*ptr = node; list->tail = node;
	node->next = nil;
	list->count += 1;
}

proc void
rng1u64_list_concat(rng1u64_list *list, rng1u64_list *to_concat)
{
	if (to_concat->head) {
		if(list->head) {
			list->tail->next = to_concat->head;
			list->tail       = to_concat->tail;
		}
		else {
			list->head = to_concat->head;
			list->tail  = to_concat->tail;
		}
		memory_zero_struct(to_concat);
	}
}

proc rng1u64_array
rng1u64_array_from_list(Arena *arena, rng1u64_list *list) {
	rng1u64_array arr = {0};
	arr.count        = list->count;
	arr.v            = push_array_no_zero(arena, rng1u64, arr.count);
	u64 idx = 0;
	for(rng1u64_node *n = list->head; n != 0; n = n->next)
	{
		arr.v[idx] = n->v;
		idx += 1;
	}
	return arr;
}

proc void
rng1s64_list_push(Arena *arena, rng1s64_list *list, rng1s64 rng) {
	rng1s64_node *node = push_array(arena, rng1s64_node, 1);
	memory_copy_struct(&node->v, &rng);

	rng1s64_node **ptr = check_nil(list->head, nil) ? &list->head : &list->tail->next;
	*ptr = node; list->tail = node;
	node->next = nil;
	list->count += 1;
}

proc rng1s64_array
rng1s64_array_from_list(Arena *arena, rng1s64_list *list)
{
	rng1s64_array arr = {0};
	arr.count = list->count;
	arr.v = push_array_no_zero(arena, rng1s64, arr.count);
	u64 idx = 0;
	for(rng1s64_node *n = list->head; n != 0; n = n->next) {
		arr.v[idx] = n->v;
		idx += 1;
	}
	return arr;
}
