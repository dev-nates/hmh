
#include <time.h>

proc Date_Time
_date_time_from_tm(struct tm in, u32 msec);

proc struct tm
_tm_from_date_time(Date_Time dt);

proc struct timespec
_timespec_from_date_time(Date_Time dt);

proc Dense_Time
_dense_time_from_timespec(struct timespec in);
