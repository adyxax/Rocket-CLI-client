#ifndef LIVEAPI_LIVEAPI_H_
#define LIVEAPI_LIVEAPI_H_

#include <cjson/cJSON.h>

typedef void (*liveapi_stdin_cb) (const char * input);

void liveapi_activate_stdin(liveapi_stdin_cb * cb);
void liveapi_send_json(size_t id, cJSON* json);
char liveapi_init(const char * authToken);
char liveapi_loop(void);
char liveapi_step(void);
void liveapi_clean(void);

#endif
