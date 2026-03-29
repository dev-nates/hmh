
typedef struct Mutex Mutex;
struct Mutex { u64 v[1]; };
proc Mutex mutex_zero(void) { return cast(Mutex)zero_struct; }
proc b8 mutex_match(Mutex a, Mutex b) { return cast(b8)(a.v[0] == b.v[0]); }

typedef struct RW_Mutex RW_Mutex;
struct RW_Mutex { u64 v[1]; };
proc RW_Mutex rw_mutex_zero(void) { return cast(RW_Mutex)zero_struct; }
proc b8 rw_mutex_match(RW_Mutex a, RW_Mutex b) { return cast(b8)(a.v[0] == b.v[0]); }

typedef struct Cond_Var Cond_Var;
struct Cond_Var { u64 v[1]; };
proc Cond_Var cond_var_zero(void) { return cast(Cond_Var)zero_struct; }
proc b8 cond_var_match(Cond_Var a, Cond_Var b) { return cast(b8)(a.v[0] == b.v[0]); }

typedef struct Semaphore Semaphore;
struct Semaphore { u64 v[1]; };
proc Semaphore semaphore_zero(void) { return cast(Semaphore)zero_struct; }
proc b8 semaphore_match(Semaphore a, Semaphore b) { return cast(b8)(a.v[0] == b.v[0]); }

// -------------------------------------------------------------------------------------------------
// Mutex

proc Mutex
mutex_alloc(Arena *arena);

proc void
mutex_delete(Mutex mutex);

proc void
mutex_take(Mutex mutex);

proc void
mutex_drop(Mutex mutex);

// -------------------------------------------------------------------------------------------------
// RW Mutex

proc RW_Mutex
rw_mutex_alloc(Arena *arena);

proc void
rw_mutex_delete(RW_Mutex mutex);

proc void
rw_mutex_take_r(RW_Mutex mutex);

proc void
rw_mutex_drop_r(RW_Mutex mutex);

proc void
rw_mutex_take_w(RW_Mutex mutex);

proc void
rw_mutex_drop_w(RW_Mutex mutex);

// -------------------------------------------------------------------------------------------------
// Conditional Variables

proc Cond_Var
cond_var_alloc(Arena *arena);

proc void
cond_var_delete(Cond_Var cv);

proc b8 /*timed out*/
cond_var_wait(Cond_Var cv, Mutex mutex, u64 end_time_us);

proc void
cond_var_signal(Cond_Var cv);

proc void
cond_var_broadcast(Cond_Var cv);

// -------------------------------------------------------------------------------------------------
// Semaphores

// @Todo: semaphore_make_shared_memory

proc Semaphore
semaphore_alloc(s64 initial_count, s64 max_count);

proc void
semaphore_delete(Semaphore semaphore);

proc b8/*wouldn't time out*/
semaphore_try(Semaphore semaphore);

proc b8/*timed out*/
semaphore_wait(Semaphore semaphore, u64 end_time_us);

proc b8/*less than max count*/
semaphore_post(Semaphore semaphore);
