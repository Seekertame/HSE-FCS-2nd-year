#define POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "seven_eight/ipc_7_8.h"
#include "nine/config_9.h"
#include "common/log_utils.h"

void run_programmer_9(int id, IpcContext7_8 *ctx);

volatile sig_atomic_t g_stop = 0;

static void handle_sigint(const int sig) {
    (void) sig;
    g_stop = 1;
}

static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s <programmer_id 0..%d>\n",
            prog, N_PROGRAMMERS - 1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    char *end = NULL;
    const long id_long = strtol(argv[1], &end, 10);
    if (*end != '\0' || id_long < 0 || id_long >= N_PROGRAMMERS) {
        usage(argv[0]);
        return 1;
    }
    const int id = (int) id_long;

    struct sigaction sa = {0};
    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);

    IpcContext7_8 ctx;
    if (ipc7_8_connect_existing(&ctx) != 0) {
        fprintf(stderr,
                "os_9_programmer: failed to connect to IPC "
                "(did you run os_7_8_init?).\n");
        return 1;
    }

    const int obs_fd = open(OBSERVER_FIFO_PATH_9, O_WRONLY | O_NONBLOCK);
    if (obs_fd < 0) {
        fprintf(stderr,
                "os_9_programmer[%d]: cannot open observer fifo '%s': %s. "
                "Running without external observer.\n",
                id, OBSERVER_FIFO_PATH_9, strerror(errno));
    } else {
        log_set_observer_fd(obs_fd);
    }

    log_event(id, "P%d (9) started", id);

    run_programmer_9(id, &ctx);

    log_event(id, "P%d (9) exiting", id);

    if (obs_fd >= 0) {
        close(obs_fd);
        log_set_observer_fd(-1);
    }

    ipc7_8_close(&ctx);
    return 0;
}
