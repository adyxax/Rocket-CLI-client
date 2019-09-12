#ifndef LIVEAPI_MESSAGES_H_
#define LIVEAPI_MESSAGES_H_

void liveapi_connect(void);
void liveapi_login(const char * authToken);
void liveapi_mark_read(const char * rid);

#endif
