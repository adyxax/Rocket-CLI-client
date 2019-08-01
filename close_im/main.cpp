#include <cstdlib>
#include <string>
#include <unistd.h>

#include "common/config.h"
#include "rest/rest.hpp"
#include "common/util.h"

int main(void)
{
    char* login = NULL;
    size_t len = 0;
    printf("Login: ");
    ssize_t read = getline(&login, &len, stdin);
    if (read > 1) login[read-1] = 0;

    char* password = getpass("Password: ");

    rest_init(WEB_URL);

    if (rest_login(login, password)) {
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
            rest_im_close(buff);
            free(buff);
        }
    } else {
        printf("Couldn't init rest api.\n");
    }

    rest_logout();
    rest_clear();

    free(login);

    return 0;
}
