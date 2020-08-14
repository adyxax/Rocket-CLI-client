#include <stdlib.h>
#include <stdio.h>

#include "common/config.h"
#include "common/cli.h"
#include "common/util.h"
#include "liveapi/liveapi.h"
#include "liveapi/messages.h"
#include "restapi/auth.h"
#include "restapi/subscriptions.h"

void maar_subscription(const struct subscription* subscription)
{
    if (subscription->unread >0 || subscription->alert) {
        printf("%s\n", subscription->name);
        liveapi_mark_read(subscription->rid);
        liveapi_step();
    }
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

    const char * authToken = restapi_login(login, password);
    if (authToken != NULL) {
        liveapi_init(authToken);
        struct subscription* subscriptions = restapi_subscriptions_get();
        printf("The following active subscriptions are marked as read :\n");
        common_subscriptions_const_walk(subscriptions, &maar_subscription);
        common_subscriptions_free(subscriptions);
        liveapi_step();
        liveapi_clean();
        restapi_logout();
    } else {
        printf("Couldn't init rest api.\n");
    }

    config_clean();
    common_cli_free();

    return 0;
}
