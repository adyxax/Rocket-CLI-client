#ifndef RESTAPI_AUTH_H_
#define RESTAPI_AUTH_H_

extern char * userId;
extern char * authToken;

char // returns 0 if ok, greater than 0 otherwise
restapi_login(const char* username, const char* password);
void restapi_logout(void);

#endif
