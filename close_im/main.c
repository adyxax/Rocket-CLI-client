#include "common/util.h"
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include "common/config.h"
#include "restapi/auth.h"
#include "restapi/im.h"

int main(void)
{
    if (config_load(CONFIG_PATH) != 0) {
        return 1;
    }

    char* login = NULL;
    size_t len = 0;
    printf("Login: ");
    ssize_t read = getline(&login, &len, stdin);
    if (read > 1) login[read-1] = 0;

    struct termios oflags, nflags;
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSADRAIN, &nflags) != 0) {
        perror("tcsetattr");
        return -1;
    }

    char* password = NULL;
    printf("Password: ");
    read = getline(&password, &len, stdin);
    if (read > 1) password[read-1] = 0;

    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
        perror("tcsetattr");
        return -1;
    }

    if (restapi_login(login, password) == 0) {
        while(1) {
            char* buff = NULL;
            size_t len2;
            printf("IM to close: ");
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
    free(login);
    free(password);

    return 0;
}
