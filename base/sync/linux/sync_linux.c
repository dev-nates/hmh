
// -------------------------------------------------------------------------------------------------
// Mutex

proc Mutex
mutex_alloc(Arena *arena) {
	pthread_mutexattr_t attr;

	errno = 0;
	pthread_mutexattr_init(&attr);
	_LOG_ERRNO("pthread_mutexattr_init");

	errno = 0;
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	_LOG_ERRNO("pthread_mutexattr_settype");

	pthread_mutex_t *handle = push_struct(arena, pthread_mutex_t);
	errno = 0;
	int result = pthread_mutex_init(handle, &attr);
	_LOG_ERRNO("pthread_mutex_init");

	errno = 0;
	pthread_mutexattr_destroy(&attr);
	_LOG_ERRNO("pthread_mutexattr_destroy");

	Mutex mutex = mutex_zero();
	mutex.v[0] = (result >= 0) ? cast(u64)(handle) : 0;
	return mutex;
}

proc void
mutex_delete(Mutex mutex) {
	assert(!mutex_match(mutex, mutex_zero()));
	pthread_mutex_t *handle = cast(pthread_mutex_t*)(mutex.v[0]);

	errno = 0;
	pthread_mutex_destroy(handle);
	_LOG_ERRNO("pthread_mutex_destroy");
}

proc void
mutex_take(Mutex mutex) {
	assert(!mutex_match(mutex, mutex_zero()));
	pthread_mutex_t *handle = cast(pthread_mutex_t*)(mutex.v[0]);

	errno = 0;
	pthread_mutex_lock(handle);
	_LOG_ERRNO("pthread_mutex_lock");
}

proc void
mutex_drop(Mutex mutex) {
	assert(!mutex_match(mutex, mutex_zero()));
	pthread_mutex_t *handle = cast(pthread_mutex_t*)(mutex.v[0]);

	errno = 0;
	pthread_mutex_unlock(handle);
	_LOG_ERRNO("pthread_mutex_unlock");
}

// -------------------------------------------------------------------------------------------------
// RW Mutex

proc RW_Mutex
rw_mutex_alloc(Arena *arena) {
	pthread_rwlock_t *handle = push_struct(arena, pthread_rwlock_t);

	errno = 0;
	int result = pthread_rwlock_init(handle, 0);
	_LOG_ERRNO("pthread_rwlock_init");

	RW_Mutex mutex = rw_mutex_zero();
	mutex.v[0] = (result >= 0) ? cast(u64)(handle) : 0;
	return mutex;
}

proc void
rw_mutex_delete(RW_Mutex mutex) {
	assert(!rw_mutex_match(mutex, rw_mutex_zero()));
	pthread_rwlock_t *handle = cast(pthread_rwlock_t*)(mutex.v[0]);

	errno = 0;
	pthread_rwlock_destroy(handle);
	_LOG_ERRNO("pthread_rwlock_destroy");
}

proc void
rw_mutex_take_r(RW_Mutex mutex) {
	assert(!rw_mutex_match(mutex, rw_mutex_zero()));
	pthread_rwlock_t *handle = cast(pthread_rwlock_t*)(mutex.v[0]);

	errno = 0;
	pthread_rwlock_rdlock(handle);
	_LOG_ERRNO("pthread_rwlock_rdlock");
}

proc void
rw_mutex_drop_r(RW_Mutex mutex) {
	assert(!rw_mutex_match(mutex, rw_mutex_zero()));
	pthread_rwlock_t *handle = cast(pthread_rwlock_t*)(mutex.v[0]);

	errno = 0;
	pthread_rwlock_unlock(handle);
	_LOG_ERRNO("pthread_rwlock_unlock");
}

proc void
rw_mutex_take_w(RW_Mutex mutex) {
	assert(!rw_mutex_match(mutex, rw_mutex_zero()));
	pthread_rwlock_t *handle = cast(pthread_rwlock_t*)(mutex.v[0]);

	errno = 0;
	pthread_rwlock_wrlock(handle);
	_LOG_ERRNO("pthread_rwlock_wrlock");
}

proc void
rw_mutex_drop_w(RW_Mutex mutex) {
	assert(!rw_mutex_match(mutex, rw_mutex_zero()));
	pthread_rwlock_t *handle = cast(pthread_rwlock_t*)(mutex.v[0]);

	errno = 0;
	pthread_rwlock_unlock(handle);
	_LOG_ERRNO("pthread_rwlock_unlock");
}

// -------------------------------------------------------------------------------------------------
// Conditional Variables

proc Cond_Var
cond_var_alloc(Arena *arena) {
	pthread_cond_t *handle = push_struct(arena, pthread_cond_t);

	errno = 0;
	int result = pthread_cond_init(handle, nil);
	_LOG_ERRNO("pthread_cond_init");

	Cond_Var cv = cond_var_zero();
	cv.v[0] = (result >= 0) ? cast(u64)(handle) : 0;
	return cv;
}

proc void
cond_var_delete(Cond_Var cv) {
	assert(!cond_var_match(cv, cond_var_zero()));
	pthread_cond_t *handle = cast(pthread_cond_t*)(cv.v[0]);
	
	errno = 0;
	pthread_cond_destroy(handle);
	_LOG_ERRNO("pthread_cond_destroy");
}

proc b8 /*timed out*/
cond_var_wait(Cond_Var cv, Mutex mutex, u64 end_time_us) {
	assert(!cond_var_match(cv, cond_var_zero()));
	assert(!mutex_match(mutex, mutex_zero()));
	pthread_cond_t *cv_handle = cast(pthread_cond_t*)(cv.v[0]);
	pthread_mutex_t *mutex_handle = cast(pthread_mutex_t*)(mutex.v[0]);

	struct timespec timespec;
	timespec.tv_sec = end_time_us / millions(1);
	timespec.tv_nsec = thousands(1) * cast(s64)(end_time_us - (end_time_us/millions(1))*millions(1));
	assert(timespec.tv_nsec >= 0 && timespec.tv_nsec < billions(1));

	errno = 0;
	int result = pthread_cond_timedwait(cv_handle, mutex_handle, &timespec);
	_LOG_ERRNO_COND("pthread_cond_timedwait", (errno != ETIMEDOUT));
	b8 timed_out = cast(b8)(result != ETIMEDOUT);
	return timed_out;
}

proc void
cond_var_signal(Cond_Var cv) {
	assert(!cond_var_match(cv, cond_var_zero()));
	pthread_cond_t *handle = cast(pthread_cond_t*)(cv.v[0]);

	errno = 0;
	pthread_cond_signal(handle);
	_LOG_ERRNO("pthread_cond_signal");
}

proc void
cond_var_broadcast(Cond_Var cv) {
	assert(!cond_var_match(cv, cond_var_zero()));
	pthread_cond_t *handle = cast(pthread_cond_t*)(cv.v[0]);

	errno = 0;
	pthread_cond_signal(handle);
	_LOG_ERRNO("pthread_cond_signal");
}

// -------------------------------------------------------------------------------------------------
// Semaphores

proc Semaphore
semaphore_alloc(s64 initial_count, s64 max_count) {
	assert(initial_count >= 0 && max_count > 0);


	errno =0;
	sem_t *sem = cast(sem_t*)mmap(0, size_of(sem_t), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	ensure(sem != MAP_FAILED);
	_LOG_ERRNO("mmap");

	errno = 0;
	int result = sem_init(sem, 0, cast(u32)initial_count);
	_LOG_ERRNO("sem_init");
	
	Semaphore semaphore = semaphore_zero();
	semaphore.v[0] = (result == 0) ? cast(u64)(sem) : 0;
	return semaphore;
}

proc void
semaphore_delete(Semaphore semaphore) {
	assert(!semaphore_match(semaphore, semaphore_zero()));
	sem_t *sem = cast(sem_t*)(semaphore.v[0]);

	errno = 0;
	int result = munmap(cast(rawptr)sem, size_of(sem_t));
	_LOG_ERRNO("munmap");
	ensure(result == 0);
}

proc b8/*wouldn't time out*/
semaphore_try(Semaphore semaphore) {
	assert(!semaphore_match(semaphore, semaphore_zero()));
	sem_t *sem = cast(sem_t*)(semaphore.v[0]);

	errno = 0;
	int err = sem_trywait(sem);
	_LOG_ERRNO_COND("sem_trywait", (errno != EAGAIN));

	b8 result = cast(b8)(err != EAGAIN);
	return result;
}

proc b8/*timed out*/
semaphore_wait(Semaphore semaphore, u64 end_time_us) {
	assert(!semaphore_match(semaphore, semaphore_zero()));
	sem_t *sem = cast(sem_t*)(semaphore.v[0]);

	struct timespec timespec;
	timespec.tv_sec = end_time_us/millions(1);
	timespec.tv_nsec = thousands(1) * (s64)(end_time_us - (end_time_us/millions(1))*millions(1));
	assert(timespec.tv_nsec >= 0 && timespec.tv_nsec < billions(1));

	errno = 0;
	int err = sem_timedwait(sem, &timespec);
	_LOG_ERRNO_COND("sem_timedwait", (errno != ETIMEDOUT));

	b8 result = cast(b8)(err != ETIMEDOUT);
	return result;
}

proc b8/*less than max count*/
semaphore_post(Semaphore semaphore) {
	assert(!semaphore_match(semaphore, semaphore_zero()));
	sem_t *sem = cast(sem_t*)(semaphore.v[0]);

	errno = 0;
	int err = sem_post(sem);
	_LOG_ERRNO_COND("sem_post", (errno != EOVERFLOW));

	b8 result = cast(b8)(err != EOVERFLOW);
	return result;
}
