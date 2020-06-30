#include <common/util.h>

#include "stdin.h"

static liveapi_stdin_cb * stdin_cb = NULL;

static void stdin_read_cb(struct ev_loop *loop UNUSED, struct ev_io *w, int revents UNUSED)
{
    struct uwsc_client *cl = w->data;
    char buf[128] = "";
    int n;

    n = read(w->fd, buf, sizeof(buf));
    if (n > 1) {
        buf[n - 1] = 0;

        if (buf[0] == 'q')
            cl->send_close(cl, UWSC_CLOSE_STATUS_NORMAL, "ByeBye");
        else
            cl->send(cl, buf, strlen(buf) + 1,  UWSC_OP_TEXT);
    }
}

void liveapi_activate_stdin_with_callback(struct uwsc_client *cl, liveapi_stdin_cb * cb)
{
    stdin_cb = cb;

    static struct ev_io stdin_watcher;
    stdin_watcher.data = cl;
    ev_io_init(&stdin_watcher, stdin_read_cb, STDIN_FILENO, EV_READ);
    ev_io_start(cl->loop, &stdin_watcher);
}
