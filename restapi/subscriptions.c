#include <cjson/cJSON.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common/http.h"
#include "subscriptions.h"

struct subscription*  // returns NULL if error or a uthash of subscriptions that needs to be freed by the caller
restapi_subscriptions_get(void)
{
    struct subscription* subscriptions = NULL;

    const char* buffer = http_get("/api/v1/subscriptions.get");

    if (buffer == NULL) {
        fprintf(stderr, "Error while subscriptions_get, http get didn't return any data.\n");
        return NULL;
    }

    cJSON* json = cJSON_Parse(buffer);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
            fprintf(stderr, "Json parsing error before: %s\n", error_ptr);
        fprintf(stderr, "Error while subscriptions_get, couldn't parse json output :\n%s\n", buffer);
        goto get_json_cleanup;
    }

    const cJSON* success = cJSON_GetObjectItemCaseSensitive(json, "success");
    if (cJSON_IsTrue(success)) {
        cJSON* updates = cJSON_GetObjectItemCaseSensitive(json, "update");
        if (!cJSON_IsArray(updates)) {
            fprintf(stderr, "Error while subscriptions_get, couldn't parse json output :\n%s\n", buffer);
            goto get_json_cleanup;
        }
        const cJSON* update = NULL;
        cJSON_ArrayForEach(update, updates) {
            const cJSON* id = cJSON_GetObjectItemCaseSensitive(update, "_id");
            const cJSON* name = cJSON_GetObjectItemCaseSensitive(update, "name");
            const cJSON* type = cJSON_GetObjectItemCaseSensitive(update, "t");
            const cJSON* open = cJSON_GetObjectItemCaseSensitive(update, "open");
            const cJSON* unread = cJSON_GetObjectItemCaseSensitive(update, "unread");
            enum subscription_type etype;
            if (!cJSON_IsString(id) || id->valuestring == NULL || !cJSON_IsString(name) || name->valuestring == NULL || !cJSON_IsString(type) || type->valuestring == NULL || !cJSON_IsTrue(open) || !cJSON_IsNumber(unread))
                continue;
            if (strcmp(type->valuestring, "c") == 0)
                etype = SUBSCRIPTION_CHANNEL;
            else if (strcmp(type->valuestring, "d") == 0)
                etype = SUBSCRIPTION_DIRECT;
            else if (strcmp(type->valuestring, "p") == 0)
                etype = SUBSCRIPTION_PRIVATE;
            else {
                fprintf(stderr, "Bug found : Unknown subscription type %s\n%s\n", type->valuestring, buffer);
                exit(999);
            }
            common_subscription_add(&subscriptions, id->valuestring, name->valuestring, etype, unread->valueint);
        }
    }
get_json_cleanup:
    cJSON_Delete(json);
    return subscriptions;
}
