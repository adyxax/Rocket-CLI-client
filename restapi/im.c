#include <cjson/cJSON.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common/http.h"
#include "im.h"

#define LOGIN_ARG_PRE "{ \"username\": \""
#define LOGIN_ARG_POST "\" }"

char // returns 0 if ok, greater than 0 otherwise
restapi_im_close(const char* username)
{
    char ret = 0;

    size_t pre_len = strlen(LOGIN_ARG_PRE);
    size_t user_len = strlen(username);
    size_t post_len = strlen(LOGIN_ARG_POST);
    char* login_args = malloc(pre_len + user_len + post_len + 1);
    strcpy(login_args, LOGIN_ARG_PRE);
    strcpy(login_args + pre_len, username);
    strcpy(login_args + pre_len + user_len, LOGIN_ARG_POST);
    http_add_header("Content-type", "application/json");
    const char* buffer = http_post("/api/v1/im.close", login_args);
    free(login_args);

    if (buffer == NULL) {
        fprintf(stderr, "Error while im_close, http post didn't return any data.\n");
        return 1;
    }

    cJSON* json = cJSON_Parse(buffer);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
            fprintf(stderr, "Json parsing error before: %s\n", error_ptr);
        fprintf(stderr, "Error while im_close, couldn't parse json output :\n%s\n", buffer);
        ret = 2;
        goto login_json_cleanup;
    }

    const cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "status");
    if (cJSON_IsString(status) && status->valuestring != NULL && strcmp(status->valuestring, "success") == 0) {
        printf("user %s removed\n", username);
    } else {
        const cJSON* msg = cJSON_GetObjectItemCaseSensitive(json, "message");
        if (cJSON_IsString(msg) && msg->valuestring != NULL)
            fprintf(stderr, "Error while im_close: %s\n", msg->valuestring);
        else
            fprintf(stderr, "Error while im_close.\n%s\n", buffer);
        ret = 3;
    }
login_json_cleanup:
    cJSON_Delete(json);
    return ret;
}
