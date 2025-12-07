#define POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <signal.h>

#include "common/shared_state.h"
#include "common/log_utils.h"
#include "four_six/config_4_6.h"

volatile sig_atomic_t g_stop = 0;

static void handle_sigint(int signo) {
    (void) signo;
    g_stop = 1;
    log_event(-1, "parent: SIGINT received, stopping...");
}

extern void run_programmer_4_6(int id, SharedState *state);

int main(int argc, char **argv) {
    (void) argc;
    (void) argv;

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
    }

    SharedState *state = mmap(NULL, sizeof(SharedState),
                              PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS,
                              -1, 0);
    if (state == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    shared_state_init(state);
    shared_state_set_total_limit(state, TOTAL_ACCEPTED_LIMIT);
    log_event(-1, "parent: shared state initialized");

    pid_t pids[N_PROGRAMMERS];

    for (int i = 0; i < N_PROGRAMMERS; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            g_stop = 1;
            break;
        } if (pid == 0) {
            run_programmer_4_6(i, state);
            _exit(0);
        }
        pids[i] = pid;
    }

    for (int i = 0; i < N_PROGRAMMERS; ++i) {
        if (pids[i] > 0) {
            waitpid(pids[i], NULL, 0);
        }
    }

    const int total_ok = shared_state_get_total_accepted(state);
    char msg[128];
    snprintf(msg, sizeof(msg),
             "parent: all children finished, total accepted = %d", total_ok);
    log_event(-1, msg);

    shared_state_destroy(state);
    if (munmap(state, sizeof(SharedState)) == -1) {
        perror("munmap");
    }

    log_event(-1, "parent: exiting");
    return 0;
}
