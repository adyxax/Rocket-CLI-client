#include <cjson/cJSON.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "auth.h"
#include "common/config.h"
#include "common/http.h"

#define USER_STR "user="
#define PASS_STR "&password="

char * authToken = NULL;

const char * // returns authToken if ok, NULL otherwise
restapi_login(const char* username, const char* password)
{
    http_init();
    size_t user_str_len = strlen(USER_STR);
    size_t user_len = strlen(username);
    size_t pass_str_len = strlen(PASS_STR);
    size_t pass_len = strlen(password);
    char* login_args = malloc(user_str_len + user_len + pass_str_len + pass_len + 1);
    strcpy(login_args, USER_STR);
    strcpy(login_args + user_str_len, username);
    strcpy(login_args + user_str_len + user_len, PASS_STR);
    strcpy(login_args + user_str_len + user_len + pass_str_len, password);
    const char* buffer = http_post("/api/v1/login", login_args);
    free(login_args);
    if (buffer == NULL) {
        fprintf(stderr, "Error while authenticating, http post didn't return any data.\n");
        return NULL;
    }

    cJSON* json = cJSON_Parse(buffer);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
            fprintf(stderr, "Json parsing error before: %s\n", error_ptr);
        fprintf(stderr, "Error while authenticating, couldn't parse json output :\n%s\n", buffer);
        goto login_json_cleanup;
    }

    const cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "status");
    if (cJSON_IsString(status) && status->valuestring != NULL && strcmp(status->valuestring, "success") == 0) {
        cJSON* data = cJSON_GetObjectItemCaseSensitive(json, "data");
        if (!cJSON_IsObject(data)) {
            fprintf(stderr, "Error while authenticating, couldn't parse json output :\n%s\n", buffer);
            goto login_json_cleanup;
        }
        cJSON* userId = cJSON_GetObjectItemCaseSensitive(data, "userId");
        cJSON* jauthToken = cJSON_GetObjectItemCaseSensitive(data, "authToken");
        if (!cJSON_IsString(userId) || userId->valuestring == NULL || !cJSON_IsString(jauthToken) || jauthToken->valuestring == NULL) {
            fprintf(stderr, "Error while authenticating, couldn't parse json output :\n%s\n", buffer);
            goto login_json_cleanup;
        }

        printf("userid: %s\nauthtoken: %s\n", userId->valuestring, jauthToken->valuestring);
        http_add_header("X-User-Id", userId->valuestring);
        http_add_header("X-Auth-Token", jauthToken->valuestring);
        http_add_header("Content-type", "application/json");
        http_add_header("Expect", "");
        authToken = malloc(strlen(jauthToken->valuestring) + 1);
        strcpy(authToken, jauthToken->valuestring);
    } else {
        const cJSON* msg = cJSON_GetObjectItemCaseSensitive(json, "message");
        if (cJSON_IsString(msg) && msg->valuestring != NULL)
            fprintf(stderr, "Error while authenticating: %s\n", msg->valuestring);
        else
            fprintf(stderr, "Error while authenticating.\n%s\n", buffer);
    }
login_json_cleanup:
    cJSON_Delete(json);
    return authToken;
}

void restapi_logout(void)
{
    (void) http_post("/api/v1/logout", NULL);
    http_clean();
    free(authToken);
}
