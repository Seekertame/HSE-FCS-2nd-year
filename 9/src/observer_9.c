#define POSIX_C_SOURCE 200809L

#include "nine/config_9.h"
#include "common/log_utils.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

static volatile sig_atomic_t g_stop = 0;

static void handle_sigint(const int sig) {
    (void)sig;
    g_stop = 1;
}

int observer_9_open_fifo(void) {
    if (mkfifo(OBSERVER_FIFO_PATH_9, 0666) < 0) {
        if (errno != EEXIST) {
            perror("mkfifo");
            return -1;
        }
    }

    const int fd = open(OBSERVER_FIFO_PATH_9, O_RDONLY);
    if (fd < 0) {
        perror("open fifo for read");
        return -1;
    }

    const int dummy = open(OBSERVER_FIFO_PATH_9, O_WRONLY | O_NONBLOCK);
    if (dummy < 0) {
        /* не критично */
    }

    return fd;
}

void observer_9_run(const int fifo_fd) {
    struct sigaction sa = {0};
    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);

    char buf[OBSERVER_LINE_MAX_9];

    log_event(-1, "observer (9) started, listening on %s", OBSERVER_FIFO_PATH_9);

    while (!g_stop) {
        const ssize_t n = read(fifo_fd, buf, sizeof buf - 1);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("observer read");
            break;
        }
        if (n == 0) {
            log_event(-1, "observer (9): EOF on fifo, exiting");
            break;
        }

        buf[n] = '\0';

        fprintf(stdout, "[OBS] %s", buf);
        fflush(stdout);
    }

    log_event(-1, "observer (9) stopped");
}
