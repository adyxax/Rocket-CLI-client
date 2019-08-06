#ifndef RESTAPI_AUTH_H_
#define RESTAPI_AUTH_H_

const char * // returns authToken if ok, NULL otherwise
restapi_login(const char* username, const char* password);
void restapi_logout(void);

#endif
