#define POSIX_C_SOURCE 200809L

#include "ten/config_10.h"
#include "common/log_utils.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

static volatile sig_atomic_t g_stop_obs = 0;

static void handle_sigint_obs(int sig) {
    (void)sig;
    g_stop_obs = 1;
}

int observer_10_open_fifo(const int obs_id) {
    char path[128];
    snprintf(path, sizeof path, OBSERVER_FIFO_TEMPLATE_10, obs_id);

    if (mkfifo(path, 0666) < 0) {
        if (errno != EEXIST) {
            perror("mkfifo");
            return -1;
        }
    }

    const int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("open fifo for read");
        return -1;
    }

    const int dummy = open(path, O_WRONLY | O_NONBLOCK);
    if (dummy < 0) {}

    return fd;
}

void observer_10_run(const int obs_id, const int fifo_fd) {
    struct sigaction sa = {0};
    sa.sa_handler = handle_sigint_obs;
    sigaction(SIGINT, &sa, NULL);

    char buf[OBSERVER_LINE_MAX_10];

    log_event(-1, "observer #%d (10) started, listening on its fifo", obs_id);

    while (!g_stop_obs) {
        const ssize_t n = read(fifo_fd, buf, sizeof buf - 1);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("observer_10 read");
            break;
        }
        if (n == 0) {
            log_event(-1, "observer #%d (10): EOF on fifo, exiting", obs_id);
            break;
        }

        buf[n] = '\0';

        fprintf(stdout, "[OBS#%d] %s", obs_id, buf);
        fflush(stdout);
    }

    log_event(-1, "observer #%d (10) stopped", obs_id);
}
