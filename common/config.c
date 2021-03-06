#include <libconfig.h>
#include <stdlib.h>

#include "config.h"

config_t * config = NULL;

char // returns 0 if ok, greater than 0 otherwise
config_load(const char *config_file)
{
    config = malloc(sizeof(config_t));
    config_init(config);
    config_set_tab_width(config, 4);
    if (config_read_file(config, config_file) != CONFIG_TRUE) {
        switch(config_error_type(config)) {
          case CONFIG_ERR_NONE:
            fprintf(stderr, "Configuration read error with none type reported... This shouldn't happen!\n");
            break;
          case CONFIG_ERR_FILE_IO:
            fprintf(stderr, "Configuration I/O error, the most common cause is a file not found at %s\n", CONFIG_PATH);
            break;
          case CONFIG_ERR_PARSE:
            fprintf(stderr, "Configuration parse error\n");
            break;
        }
        fprintf(stderr, "Configuration read error occured at %s:%d %s\n", config_error_file(config), config_error_line(config), config_error_text(config));
        return 1;
    }
    return 0;

}

const char * config_get_web_url(void)
{
    const char * url;
    if (config_lookup_string(config, "web_url", &url) != CONFIG_TRUE) {
        return DEFAULT_WEB_URL;
    }
    return url;
}

const char * config_get_wss_url(void)
{
    const char * url;
    if (config_lookup_string(config, "wss_url", &url) != CONFIG_TRUE) {
        return DEFAULT_WSS_URL;
    }
    return url;
}

const char * config_get_login(void)
{
    const char * login;
    if (config_lookup_string(config, "login", &login) != CONFIG_TRUE) {
        return NULL;
    }
    return login;
}

const char * config_get_password(void)
{
    const char * password;
    if (config_lookup_string(config, "password", &password) != CONFIG_TRUE) {
        return NULL;
    }
    return password;
}

void config_clean(void)
{
    if (config != NULL) {
        config_destroy(config);
        free(config);
        config = NULL;
    }
}
