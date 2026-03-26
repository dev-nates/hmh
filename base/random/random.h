
typedef struct $random_series $random_series;
struct $random_series {
	u64 a;
	u64 b;
	u64 c;
	u64 d;
};

proc $random_series
mk_random_series(u64 seed);

proc u64
random_u64($random_series *series);

proc b8
random_choice($random_series *series);

//--------------------------------------------------------------------------------
// random 0 to 1 || -1 to 1 float

proc f32
random_01_f32($random_series *series);

proc f32
random_n11_f32($random_series *series);


proc f64
random_01_f64($random_series *series);

proc f64
random_n11_f64($random_series *series);

//--------------------------------------------------------------------------------
// random ranges

proc f32
random_r1f32($random_series *series, rng1f32 range);

proc f64
random_r1f64($random_series *series, rng1f64 range);


proc s64
random_r1s64($random_series *series, rng1s64 range);

proc u64
random_r1u64($random_series *series, rng1u64 range);

