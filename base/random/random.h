
typedef struct Random_Series Random_Series;
struct Random_Series {
	u64 a;
	u64 b;
	u64 c;
	u64 d;
};

proc Random_Series
make_random_series(u64 seed);

proc u64
random_u64(Random_Series *series);

proc b8
random_choice(Random_Series *series);

//--------------------------------------------------------------------------------
// random 0 to 1 || -1 to 1 float

proc f32
random_01_f32(Random_Series *series);

proc f32
random_n11_f32(Random_Series *series);


proc f64
random_01_f64(Random_Series *series);

proc f64
random_n11_f64(Random_Series *series);

//--------------------------------------------------------------------------------
// random ranges

proc f32
random_r1f32(Random_Series *series, rng1f32 range);

proc f64
random_r1f64(Random_Series *series, rng1f64 range);


proc s64
random_r1s64(Random_Series *series, rng1s64 range);

proc u64
random_r1u64(Random_Series *series, rng1u64 range);

