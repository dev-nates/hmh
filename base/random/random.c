
proc $random_series
mk_random_series(u64 seed) {
	$random_series series = {
		.a = 0xf1ea5eed,
		.b = seed,
		.c = seed,
		.d = seed,
	};
	for (s64 i = 0; i < 20; i += 1) {
		u64 e = series.a - rotl64(series.b, 7);
		series.a = series.b ^ rotl64(series.c, 13);
		series.b = series.c + rotl64(series.d, 37);
		series.c = series.d + e;
		series.d = e + series.a;
	}
	return series;
}

proc u64
random_u64($random_series *series) {
	u64 e = series->a - rotl64(series->b, 7);
	series->a = series->b ^ rotl64(series->c, 13);
	series->b = series->c + rotl64(series->d, 37);
	series->c = series->d + e;
	series->d = e + series->a;
	return series->d;
}

proc b8
random_choice($random_series *series) {
	u64 val = random_u64(series);
	b8 result = count_bits_set64(val)&1;
	return result;
}

proc f32
random_01_f32($random_series *series) {
	return (f32)random_01_f64(series);
}

proc f32
random_n11_f32($random_series *series) {
	return (f32)random_n11_f64(series);
}

proc f64
random_01_f64($random_series *series) {
	u64 val = random_u64(series);
	f64 result = (f64)val / (f64)max_u64;
	return result;
}

proc f64
random_n11_f64($random_series *series) {
	u64 val = random_u64(series);
	f64 result = (f64)val / (f64)max_u64;
	result = (result*2.0) - 1.0;
	return result;
}

proc f32
random_r1f32($random_series *series, rng1f32 range) {
	f32 t = random_01_f32(series);
	f32 result = mix_f32(range.min, range.max, t);
	return result;
}

proc f64
random_r1f64($random_series *series, rng1f64 range) {
	f64 t = random_01_f64(series);
	f64 result = mix_f64(range.min, range.max, t);
	return result;
}


proc s64
random_r1s64($random_series *series, rng1s64 range) {
	f64 t = random_01_f64(series);
	s64 result = (s64)mix_f64((f64)range.min, (f64)range.max, t);
	return result;
}

proc u64
random_r1u64($random_series *series, rng1u64 range) {
	f64 t = random_01_f64(series);
	u64 result = (u64)mix_f64((f64)range.min, (f64)range.max, t);
	return result;
}
