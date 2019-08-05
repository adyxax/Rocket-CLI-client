#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "http.h"
#include "util.h"

// Curl internal handling
CURL* curl = NULL;
struct curl_slist *headers = NULL;

// Curl buffer handling
char* buffer = NULL;
size_t buffer_len = 0;
size_t buffer_fill = 0;

char initialized = 0;

#define HEADER_SEP ": "

static size_t writeCallback(char* buf, size_t size, size_t nmemb, void* userp);
const char * http_perform(const char* path, const char* postfields);

void http_add_header(const char* name, const char* value)
{
    if (!initialized) {
        fprintf(stderr, "BUG: http_init wasn't called!\n");
        return;
    }

    size_t name_len = strlen(name);
    size_t sep_len = strlen(HEADER_SEP);
    size_t value_len = strlen(value);
    char* header = malloc(name_len + sep_len + value_len + 1);
    strcpy(header, name);
    strcpy(header + name_len, HEADER_SEP);
    strcpy(header + name_len + sep_len, value);
    headers = curl_slist_append(headers, header);
    free(header);
}

void http_clean(void)
{
    if (!initialized)
        return;
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl = NULL;
    curl_global_cleanup();
    free(buffer);
}

const char * http_get(const char* path)
{
    curl_easy_reset(curl);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeCallback);

    curl_easy_setopt(curl, CURLOPT_POST, 0);
    return http_perform(path, NULL);
}

void http_init(void)
{
    if (initialized) {
        fprintf(stderr, "BUG: http_init already called!\n");
        return;
    }
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    initialized = 1;
}

const char * http_perform(const char* path, const char* postfields)
{
    if (!initialized) {
        fprintf(stderr, "BUG: http_init wasn't called!\n");
        return NULL;
    }

    if (buffer != NULL) {
        memset(buffer, 0, buffer_fill);
        buffer_fill = 0;
    }

    const char * weburl = config_get_web_url();
    size_t weburl_len = strlen(weburl);
    size_t path_len = strlen(path);
    char * url = malloc(weburl_len + path_len + 1);
    strcpy(url, weburl);
    strcpy(url + weburl_len, path);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    free(url);
    if (postfields != NULL)
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields);
    if (headers != NULL)
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        fprintf(stderr, "HTTP get to %s failed: %s\n", url, curl_easy_strerror(res));
        return NULL;
    }

    return buffer;
}

const char * http_post(const char* path, const char* postfields)
{
    curl_easy_reset(curl);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeCallback);

    if (postfields == NULL) {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0L);
    }
    return http_perform(path, postfields);
}

static size_t writeCallback(char* buf, size_t size, size_t nmemb, UNUSED void* userp)
{   //callback must have this declaration
    //buf is a pointer to the data that curl has for us
    //size*nmemb is the size of the buffer

    register size_t sn = size * nmemb;
    if (buffer == NULL) {
        buffer_len = sn;
        buffer = malloc (buffer_len + 1);
    } else if (buffer_fill + sn >= buffer_len) {
        buffer_len += sn;
        buffer = realloc(buffer, buffer_len + 1);
    }
    for (unsigned int c = 0; c < size * nmemb; ++c) {
        buffer[buffer_fill + c] = buf[c];
    }
    buffer_fill += sn;
    buffer[buffer_fill] = 0;
    return sn; //tell curl how many bytes we handled
}
