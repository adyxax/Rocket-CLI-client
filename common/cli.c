#include <stdlib.h>
#include <stdio.h>
#include <termios.h>

#include "cli.h"

char * login = NULL;
char * password = NULL;

void common_cli_free(void)
{
    free(login);
    login = NULL;
    free(password);
    password = NULL;
}

const char* common_cli_get_login(void)
{
    while (1) {
        size_t len = 0;
        printf("Login: ");
        ssize_t read = getline(&login, &len, stdin);
        if (read > 1) {
            login[read-1] = 0;
            return login;
        }
    }
}

const char* common_cli_get_password(void)
{
    struct termios oflags, nflags;
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSADRAIN, &nflags) != 0) {
        perror("tcsetattr");
        exit(998);
    }

    while (1) {
        size_t len = 0;
        printf("Password: ");
        size_t read = getline(&password, &len, stdin);
        if (read > 1) {
            password[read-1] = 0;
            break;
        }
    }
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
        perror("tcsetattr 2");
        exit(998);
    }
    return password;
}
