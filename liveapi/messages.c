#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>

#include <features.h>
#include "libuwsc/uwsc.h"

#include "liveapi.h"
#include "messages.h"

struct methodid {
    size_t id;
    char sid[33];
};

static const struct methodid* get_next_method_id(void)
{
    static struct methodid methodid = { .id = 1, .sid = "" };
    snprintf(methodid.sid, sizeof(methodid.sid), "%zu", ++methodid.id);
    return &methodid;
}

void liveapi_connect(void)
{
    cJSON* json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "msg", cJSON_CreateString("connect"));
    cJSON_AddItemToObject(json, "version", cJSON_CreateString("1"));

    cJSON* support = cJSON_CreateArray();
    cJSON_AddItemToArray(support, cJSON_CreateString("1"));
    cJSON_AddItemToArray(support, cJSON_CreateString("pre2"));
    cJSON_AddItemToArray(support, cJSON_CreateString("pre1"));
    cJSON_AddItemToObject(json, "support", support);

    liveapi_send_json(0, json);
}

void liveapi_login(const char * authToken)
{
    const struct methodid * id = get_next_method_id();

    cJSON* json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "msg", cJSON_CreateString("method"));
    cJSON_AddItemToObject(json, "id", cJSON_CreateString(id->sid));
    cJSON_AddItemToObject(json, "method", cJSON_CreateString("login"));

    cJSON* params = cJSON_CreateArray();
    cJSON* resume = cJSON_CreateObject();
    cJSON_AddItemToObject(resume, "resume", cJSON_CreateString(authToken));
    cJSON_AddItemToArray(params, resume);
    cJSON_AddItemToObject(json, "params", params);

    liveapi_send_json(id->id, json);
}

void liveapi_mark_read(const char * rid)
{
    const struct methodid * id = get_next_method_id();

    cJSON* json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "msg", cJSON_CreateString("method"));
    cJSON_AddItemToObject(json, "id", cJSON_CreateString(id->sid));
    cJSON_AddItemToObject(json, "method", cJSON_CreateString("readMessages"));

    cJSON* params = cJSON_CreateArray();
    cJSON_AddItemToArray(params, cJSON_CreateString(rid));
    cJSON_AddItemToObject(json, "params", params);

    liveapi_send_json(id->id, json);
}
