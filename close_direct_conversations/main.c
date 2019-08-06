#include <stdlib.h>
#include <stdio.h>

#include "common/config.h"
#include "common/cli.h"
#include "common/util.h"
#include "restapi/auth.h"
#include "restapi/im.h"
#include "restapi/subscriptions.h"

void print_subscription(const struct subscription* sub)
{
    if (sub->type == SUBSCRIPTION_DIRECT)
        printf("\t%s\n", sub->name);
}

int main(void)
{
    if (config_load(CONFIG_PATH) != 0) {
        return 1;
    }

    const char* login = config_get_login();
    if (login == NULL)
        login = common_cli_get_login();

    const char* password = config_get_password();
    if (password == NULL)
        password = common_cli_get_password();

    if (restapi_login(login, password) == 0) {
        struct subscription* subscriptions = restapi_subscriptions_get();

        printf("Active direct conversations :\n");
        common_subscriptions_const_walk(subscriptions, &print_subscription);
        common_subscriptions_free(subscriptions);

        while(1) {
            char* buff = NULL;
            size_t len2;
            printf("Direct conversation to close: ");
            ssize_t entry = getline(&buff, &len2, stdin);
            if (entry > 1) {
                buff[entry-1] = 0;
            } else {
                free(buff);
                break;
            }
            restapi_im_close(buff);
            free(buff);
        }
    } else {
        printf("Couldn't init rest api.\n");
    }

    restapi_logout();
    config_clean();
    common_cli_free();

    return 0;
}
