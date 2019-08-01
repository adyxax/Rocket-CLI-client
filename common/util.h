#ifndef _UTIL_H_
#define _UTIL_H_

#include <features.h>
#include <time.h>

#ifndef UNUSED
#define UNUSED __attribute__ ((__unused__))
#endif

void convert_iso8601(const char *time_string, int ts_len, struct tm *tm_data);

#endif
