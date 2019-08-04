#ifndef COMMON_HTTP_H_
#define COMMON_HTTP_H_

void http_add_header(const char* name, const char* value);
void http_clean(void);
const char * http_get(const char* path);
void http_init(void);
const char * http_post(const char* path, const char* postfields);

#endif
