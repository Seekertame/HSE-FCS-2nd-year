#define POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "seven_eight/ipc_7_8.h"
#include "common/log_utils.h"

volatile sig_atomic_t g_stop = 0;

static void handle_sigint(int signo) {
    (void) signo;
    g_stop = 1;
    log_event(-1, "SIGINT received, stopping...");
}

extern void run_programmer_7_8(int id, const IpcContext7_8 *ctx);

int main(const int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <programmer_id 0..2>\n", argv[0]);
        return 1;
    }

    const int id = atoi(argv[1]);
    if (id < 0 || id >= N_PROGRAMMERS) {
        fprintf(stderr, "Invalid programmer id: %d\n", id);
        return 1;
    }

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    IpcContext7_8 ctx;
    if (ipc7_8_connect_existing(&ctx) != 0) {
        fprintf(stderr, "P%d: failed to connect to IPC (did you run os_7_8_init?)\n", id);
        return 1;
    }

    char msg[128];
    snprintf(msg, sizeof(msg), "P%d (7-8) started", id);
    log_event(id, msg);

    run_programmer_7_8(id, &ctx);

    log_event(id, "P: exiting (7-8)");

    ipc7_8_close(&ctx);
    return 0;
}
