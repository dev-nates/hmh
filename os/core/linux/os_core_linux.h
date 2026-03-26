
#ifndef OS_CORE_LINUX_H
#define OS_CORE_LINUX_H

#include <fcntl.h>
#include <errno.h>

#include <dirent.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/sendfile.h>

#include <limits.h>
#include <stdlib.h>

#include <pthread.h>
#include <semaphore.h>

typedef struct Linux_File_Iter Linux_File_Iter;
struct Linux_File_Iter {
	DIR *dir;
	OS_File_Iter_Flags flags;
	string path;
};
static_assert(size_of(Linux_File_Iter) <= OS_FILE_ITERATOR_OPAQUE_SIZE, os_linux_file_iterator_check_size);

typedef enum Linux_Entity_Kind Linux_Entity_Kind;
enum Linux_Entity_Kind {
	Linux_Entity_Kind_Mutex,
	Linux_Entity_Kind_RW_Mutex,
	Linux_Entity_Kind_CV,
	Linux_Entity_Kind_Thread,
};

typedef struct Linux_Entity Linux_Entity;
struct Linux_Entity {
	Linux_Entity *next;
	Linux_Entity_Kind kind;

	union {
		struct {
			pthread_mutex_t handle;
		} mutex;

		struct {
			pthread_rwlock_t handle;
		} rw_mutex;

		struct {
			pthread_cond_t handle;
		} cv;

		struct {
			pthread_t handle;
			OS_Thread_Proc *procedure;
			rawptr params;
			string name;
		} thread;
	};
};

typedef struct Linux_State Linux_State;
struct Linux_State {
	Arena *arena;
	OS_System_Info system_info;
	OS_Process_Info process_info;

	pthread_mutex_t entity_mutex;
	Linux_Entity *entity_freelist;
};

// Some data is initialized a startup and never touched again
// making that data safe to use multi-threaded because it's read-only.
// Other data will required synchronization mechanisms like mutexes to
// access shared data.
global Linux_State linux_state = zero_struct;

// -------------------------------------------------------------------------------------------------
// entities

proc Linux_Entity* os_lnx_entity_alloc(Linux_Entity_Kind kind);
proc void          os_lnx_entity_release(Linux_Entity *entity);

// -------------------------------------------------------------------------------------------------
// Helpers

proc File_Properties
_file_properties_from_stat(struct stat *s);
#endif // OS_CORE_LINUX_H
