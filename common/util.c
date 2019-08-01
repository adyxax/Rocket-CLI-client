#include <string.h>

#include "util.h"

void convert_iso8601(const char *time_string, int ts_len, struct tm *tm_data)
{
    tzset();

    char temp[64];
    memset(temp, 0, sizeof(temp));
    strncpy(temp, time_string, ts_len);

    struct tm ctime;
    memset(&ctime, 0, sizeof(struct tm));
    strptime(temp, "%FT%T%z", &ctime);

    long ts = mktime(&ctime) - timezone;
    localtime_r(&ts, tm_data);
}
