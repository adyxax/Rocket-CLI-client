#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <uthash.h>

#include <features.h>
#include "libuwsc/uwsc.h"

#include "common/config.h"
#include "common/util.h"
#include "liveapi.h"
#include "messages.h"
#include "stdin.h"

static struct uwsc_client * webclient;
static struct ev_loop *loop = NULL;
static char stepping = 1;

static const char * _authToken;

struct result {
    int id;
    cJSON* json;
    UT_hash_handle hh;
};

static struct result * results = NULL;

void liveapi_activate_stdin(liveapi_stdin_cb * cb)
{
    liveapi_activate_stdin_with_callback(webclient, cb);
}

static void liveapi_send(const char* msg)
{
#ifdef DEBUG_WEBSOCKETS
    printf("==>:%s\n", msg);
#endif
    webclient->send_ex(webclient, UWSC_OP_TEXT, 1, strlen(msg), msg);
    // TODO handle errors
}

void liveapi_send_json(size_t id, cJSON* json)
{
    char * msg = cJSON_Print(json);
    if (id > 0) {
        struct result * result = malloc(sizeof(struct result));
        result->id = id;
        result->json = json;
        HASH_ADD_INT(results, id, result);
    }
    liveapi_send(msg);
    if (id == 0)
        cJSON_Delete(json);
    free(msg);
}

static void uwsc_onopen(struct uwsc_client *cl UNUSED)
{
    uwsc_log_info("onopen\n");
}

static void uwsc_onmessage(struct uwsc_client *cl,
                           void *data, size_t len, bool binary)
{
#ifdef DEBUG_WEBSOCKETS
    printf("<==:");
#endif

    if (binary) {
        fprintf(stderr, "Websocket binary messages are not unsupported\n");
#ifdef DEBUG_WEBSOCKETS
        size_t i;
        uint8_t *p = data;

        for (i = 0; i < len; i++) {
            printf("%02hhX ", p[i]);
            if (i % 16 == 0 && i > 0)
                puts("");
        }
        puts("");
#endif
    } else {
#ifdef DEBUG_WEBSOCKETS
        printf("%.*s\n", (int)len, (char *)data);
#endif
        char* buffer = malloc (len + 1);
        strncpy(buffer, data, len);
        buffer[len] = 0;
        cJSON* json = cJSON_Parse(buffer);
        if (json == NULL) {
            const char *error_ptr = cJSON_GetErrorPtr();
            if (error_ptr != NULL)
                fprintf(stderr, "Json parsing error reading websocket message: %s\n", error_ptr);
            fprintf(stderr, "Error while parsing websocket message, couldn't parse json output :\n%s\n", buffer);
            goto onmessage_json_cleanup;
        }


        const cJSON* server_id = cJSON_GetObjectItemCaseSensitive(json, "server_id");
        if (cJSON_IsString(server_id) && server_id->valuestring != NULL) {
            liveapi_connect();
            goto onmessage_json_cleanup;
        }
        const cJSON* msg = cJSON_GetObjectItemCaseSensitive(json, "msg");
        if (cJSON_IsString(msg) && msg->valuestring != NULL) {
            if (strcmp(msg->valuestring, "ping") == 0) {
                liveapi_send("{ \"msg\": \"pong\" }");
            } else if (strcmp(msg->valuestring, "connected") == 0) {
                liveapi_login(_authToken);
                ev_break(cl->loop, EVBREAK_ALL);
            } else if (strcmp(msg->valuestring, "result") == 0) {
                const cJSON* jid = cJSON_GetObjectItemCaseSensitive(json, "id");
                if (!cJSON_IsString(jid) || jid->valuestring == NULL) {
                    fprintf(stderr, "Invalid websocket result : %s\n", buffer);
                    goto onmessage_json_cleanup;
                }
                size_t id = (size_t) strtoll(jid->valuestring, NULL, 10);
                struct result * result = NULL;
                HASH_FIND_INT(results, &id, result);
                if (result == NULL) {
                    fprintf(stderr, "Got a websocket result message that doesn't match any method we sent : %s\n", buffer);
                    goto onmessage_json_cleanup;
                }
                cJSON_Delete(result->json);
                HASH_DEL(results, result);
                free(result);
                // if we were stepping and got no more results we break out of the loop
                if (stepping == 1 && HASH_COUNT(results) == 0)
                    ev_break(cl->loop, EVBREAK_ALL);
            } else {
                fprintf(stderr, "Unhandled websocket message : %s\n", msg->valuestring);
            }
        } else {
            fprintf(stderr, "Unhandled websocket without msg : %s\n", buffer);
        }
onmessage_json_cleanup:
        cJSON_Delete(json);
        free(buffer);
    }
}

static void uwsc_onerror(struct uwsc_client *cl, int err, const char *msg)
{
    uwsc_log_err("onerror:%d: %s\n", err, msg);
    ev_break(cl->loop, EVBREAK_ALL);
}

static void uwsc_onclose(struct uwsc_client *cl, int code, const char *reason)
{
    uwsc_log_err("onclose:%d: %s\n", code, reason);
    ev_break(cl->loop, EVBREAK_ALL);
}

static void signal_cb(struct ev_loop *loop, ev_signal *w, int revents)
{
    if (w->signum == SIGINT) {
        (void) loop;
        //ev_break(loop, EVBREAK_ALL);
        uwsc_log_info("Normal quit\n");
        webclient->send_close(webclient, 0, "got signal");
    }
    (void) revents;
}

char liveapi_init(const char * authToken)
{
    _authToken = authToken;

    const char *url = config_get_wss_url();
    loop = EV_DEFAULT;
    struct ev_signal signal_watcher;
    int ping_interval = 10;    /* second */

    uwsc_log_info("Libuwsc: %s\n", UWSC_VERSION_STRING);

    webclient = uwsc_new(loop, url, ping_interval, NULL);
    if (!webclient)
        return -1;

    uwsc_log_info("Start connect...\n");

    webclient->onopen = uwsc_onopen;
    webclient->onmessage = uwsc_onmessage;
    webclient->onerror = uwsc_onerror;
    webclient->onclose = uwsc_onclose;

    ev_signal_init(&signal_watcher, signal_cb, SIGINT);
    ev_signal_start(loop, &signal_watcher);

    ev_run(loop, 0);

    return 0;
}

char liveapi_loop(void)
{
    stepping = 0;
    ev_run(loop, 0);
    return 0;
}

char liveapi_step(void)
{
    if (HASH_COUNT(results) > 0) {
        stepping = 1;
        ev_run(loop, 0);
    }
    return 0;
}

void liveapi_clean(void)
{
    struct result *s, *tmp;

    HASH_ITER(hh, results, s, tmp) {
        cJSON_Delete(s->json);
        HASH_DELETE(hh, results, s);
        free(s);
    }

    free(webclient);
}
