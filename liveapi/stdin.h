#ifndef LIVEAPI_STDIN_H_
#define LIVEAPI_STDIN_H_

#include <cjson/cJSON.h>
#include <features.h>
#include "libuwsc/uwsc.h"

#include "liveapi.h"

void liveapi_activate_stdin_with_callback(struct uwsc_client *cl, liveapi_stdin_cb * cb);

#endif
