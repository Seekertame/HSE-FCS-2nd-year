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
#include "ten/config_10.h"
#include "common/log_utils.h"

void run_programmer_10(int id, IpcContext7_8 *ctx);

volatile sig_atomic_t g_stop = 0;

static void handle_sigint(const int sig) {
    (void)sig;
    g_stop = 1;
}

static void usage(const char *prog) {
    fprintf(stderr, "Usage: %s <programmer_id 0..%d>\n",
            prog, N_PROGRAMMERS - 1);
}

int main(const int argc, char *argv[]) {
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
    const int id = (int)id_long;

    struct sigaction sa = {0};
    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);

    IpcContext7_8 ctx;
    if (ipc7_8_connect_existing(&ctx) != 0) {
        fprintf(stderr,
                "os_10_programmer: failed to connect to IPC "
                "(did you run os_7_8_init?).\n");
        return 1;
    }

    int observer_fds[MAX_OBSERVERS_10];
    for (int i = 0; i < MAX_OBSERVERS_10; ++i) {
        observer_fds[i] = -1;
        char path[128];
        snprintf(path, sizeof path, OBSERVER_FIFO_TEMPLATE_10, i);

        const int fd = open(path, O_WRONLY | O_NONBLOCK);
        if (fd < 0) {
            fprintf(stderr,
                    "os_10_programmer[%d]: cannot open observer fifo '%s': %s. "
                    "Skipping this observer.\n",
                    id, path, strerror(errno));
            continue;
        }

        observer_fds[i] = fd;
        log_add_observer_fd(fd);
    }

    log_event(id, "P%d (10) started", id);

    run_programmer_10(id, &ctx);

    log_event(id, "P%d (10) exiting", id);

    for (int i = 0; i < MAX_OBSERVERS_10; ++i) {
        if (observer_fds[i] >= 0) {
            close(observer_fds[i]);
            observer_fds[i] = -1;
        }
    }
    log_clear_observers();

    ipc7_8_close(&ctx);
    return 0;
}
