#ifndef RESTAPI_SUBSCRIPTIONS_H_
#define RESTAPI_SUBSCRIPTIONS_H_

#include "common/subscriptions.h"

struct subscription*  // returns NULL if error or a uthash of subscriptions that needs to be freed by the caller
restapi_subscriptions_get(void);

#endif
