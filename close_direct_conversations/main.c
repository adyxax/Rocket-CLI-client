#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include "common/config.h"
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
    char* termlogin = NULL;
    if (login == NULL) {
        size_t len = 0;
        printf("Login: ");
        ssize_t read = getline(&termlogin, &len, stdin);
        if (read > 1) termlogin[read-1] = 0;
        login = termlogin;
    }

    const char* password = config_get_password();
    char* termpassword = NULL;
    if (password == NULL) {
        struct termios oflags, nflags;
        tcgetattr(fileno(stdin), &oflags);
        nflags = oflags;
        nflags.c_lflag &= ~ECHO;
        nflags.c_lflag |= ECHONL;

        if (tcsetattr(fileno(stdin), TCSADRAIN, &nflags) != 0) {
            perror("tcsetattr");
            return -1;
        }

        size_t len = 0;
        printf("Password: ");
        size_t read = getline(&termpassword, &len, stdin);
        if (read > 1) termpassword[read-1] = 0;

        if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
            perror("tcsetattr");
            return -1;
        }
        password = termpassword;
    }

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
    free(termlogin);
    free(termpassword);

    return 0;
}
